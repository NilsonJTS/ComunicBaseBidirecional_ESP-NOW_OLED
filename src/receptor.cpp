#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <LoRa.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
AsyncWebServer server(80);

volatile bool enviarHostgator = false;
unsigned long ultimoComando = 0;

int canalRoteador = 1;
const float TEMPERATURA_LIGA_EXAUSTAO = 30.0;
const float TEMPERATURA_DESLIGA_EXAUSTAO = 28.0;

int ultRssiEnviado = -999; // Guarda o último RSSI enviado ao banco
const int DELTA_RSSI_MIN = 5; // VariaçãoMinSinal dBm(5 dBm)

// --- VARIÁVEIS DE CONTROLE DE FILTRO DE ENVIO ---
float ultTempEnviada = -999.0;
float ultUmidEnviada = -999.0;
bool ultEnergiaEnviada = false;
bool ultExaustaoOKEnviada = false;     // Sensor de retorno da exaustão
bool ultExaustaoLigadaEnviada = false; // Estado do relé de exaustão
bool ultAquecedorEnviado = false;      // Estado do relé do aquecedor

bool primeiroEnvioRealizado = false;   // Força a 1ª leitura a ir para o banco
unsigned long tempoUltimoEnvioNuvem = 0; // Para o envio periódico (Heartbeat)
const unsigned long INTERVALO_MAX_ENVIO = 300000; // 5 minutos em ms (300.000 ms)

const float DELTA_TEMP_MIN = 0.5; // Variação mínima de Temp (0.5°C)
const float DELTA_UMID_MIN = 2.0; // Variação mínima de Umidade (2.0%)

uint8_t macEmissor[] = {0xA0, 0xDD, 0x6C, 0x75, 0x0E, 0x14}; // MAC do Emissor

// Struct idêntica à do Emissor
typedef struct struct_mensagem {
    int contador;
    float temperatura;
    float umidade;
    bool energiaOk;
    bool exaustaoOK;
} struct_mensagem;

typedef struct struct_comando {
    bool acionarAquecedor;
    bool ligarExaustao;
    int rssi; //ForçaDoSinalEnviarAoEmissor
} struct_comando;

int ultimoRssiRecebido = 0;

struct_mensagem dadosRecebidos;
struct_comando comandoEnvio;
esp_now_peer_info_t peerInfo;
bool exaustaoLigada = false;

void enviarRespostaLoRa() {
    comandoEnvio.rssi = ultimoRssiRecebido;
    comandoEnvio.ligarExaustao = exaustaoLigada;

    LoRa.beginPacket();
    LoRa.write((uint8_t *)&comandoEnvio, sizeof(comandoEnvio));
    LoRa.endPacket();

    // Reseta o gatilho do aquecedor após enviar uma vez
    comandoEnvio.acionarAquecedor = false;
}

bool precisaEnviarParaNuvem(const struct_mensagem& dados) {
    // 1. PRIMEIRO ENVIO: Força a gravação da 1ª leitura após o ESP32 ligar
    if (!primeiroEnvioRealizado) {
        Serial.println(">>> DISPARO BANCO | Motivo: 1ª leitura apos ligar");
        return true;
    }

    // 2. HEARTBEAT: Se passar de 5min sem variação, envia para atualizar o site/contador
    if (millis() - tempoUltimoEnvioNuvem >= INTERVALO_MAX_ENVIO) {
        Serial.println(">>> DISPARO BANCO | Motivo: Tempo maximo (5min) atingido");
        return true;
    }

    // 3. PROTEÇÃO: Se a leitura do DHT11 for inválida (NaN), ignora
    if (isnan(dados.temperatura) || isnan(dados.umidade)) {
        return false;
    }

    // 4. CONVERSÃO PARA DÉCIMOS INTEIROS (excelente ideia!)
    int tempAtualInt = round(dados.temperatura * 10.0);
    int tempUltimaInt = round(ultTempEnviada * 10.0);

    int umidAtualInt = round(dados.umidade * 10.0);
    int umidUltimaInt = round(ultUmidEnviada * 10.0);

    int deltaTempInt = round(DELTA_TEMP_MIN * 10.0); // 0.5 -> 5
    int deltaUmidInt = round(DELTA_UMID_MIN * 10.0); // 2.0 -> 20

    // 5. COMPARAÇÕES DE VARIAÇÃO TÉRMICA E UMIDADE
    bool tempMudou = abs(tempAtualInt - tempUltimaInt) >= deltaTempInt;
    bool umidMudou = abs(umidAtualInt - umidUltimaInt) >= deltaUmidInt;

    // 6. COMPARAÇÃO DO SINAL LORA (RSSI)
    bool rssiMudou = abs(ultimoRssiRecebido - ultRssiEnviado) >= DELTA_RSSI_MIN;

    // 7. COMPARAÇÃO DOS ESTADOS DOS RELÉS E SENSORES (Variáveis separadas!)
    bool estadoMudou = (dados.energiaOk != ultEnergiaEnviada) ||
                       (dados.exaustaoOK != ultExaustaoOKEnviada) ||
                       (exaustaoLigada != ultExaustaoLigadaEnviada) ||
                       (comandoEnvio.acionarAquecedor != ultAquecedorEnviado);

    // 7. SE HOUVER VARIAÇÃO RELEVANTE
    if (tempMudou || umidMudou || estadoMudou || rssiMudou) {
        Serial.print(">>> DISPARO BANCO | Motivo: ");
        if (tempMudou) Serial.print("[Temp] ");
        if (umidMudou) Serial.print("[Umid] ");
        if (estadoMudou) Serial.print("[Relês/Energia] ");
        if (rssiMudou) Serial.print("[Sinal LoRa (RSSI)]");
        Serial.println();
        return true;
    }

    return false; // Nenhuma variação suficiente
}

void atualizarHistoricoEnvio(const struct_mensagem& dados) {
    ultTempEnviada = dados.temperatura;
    ultUmidEnviada = dados.umidade;
    ultEnergiaEnviada = dados.energiaOk;
    ultExaustaoOKEnviada = dados.exaustaoOK;
    ultExaustaoLigadaEnviada = exaustaoLigada;
    ultAquecedorEnviado = comandoEnvio.acionarAquecedor;
    ultRssiEnviado = ultimoRssiRecebido;

    primeiroEnvioRealizado = true;
    tempoUltimoEnvioNuvem = millis();
}

void verificarRecebimentoLoRa() {
    int packetSize = LoRa.parsePacket();
    if (packetSize == sizeof(dadosRecebidos)) {
        LoRa.readBytes((uint8_t *)&dadosRecebidos, sizeof(dadosRecebidos));
        
        // 1. CAPTURA A FORÇA DO SINAL EM dBm (Nativo da biblioteca LoRa)
        ultimoRssiRecebido = LoRa.packetRssi();

        Serial.print(">>> Pacote LoRa Recebido! RSSI: ");
        Serial.print(ultimoRssiRecebido);
        Serial.println(" dBm");

        // 2. Atualiza o Display OLED da Placa B (Receptor)
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("PLACA B (RECEPTOR)");
        display.setCursor(0, 16);
        display.print("Temp: ");
        display.print(dadosRecebidos.temperatura, 1);
        display.println(" C");
        display.setCursor(0, 32);
        display.print("Umid: ");
        display.print(dadosRecebidos.umidade, 1);
        display.println(" %");
        display.setCursor(0, 48);
        display.print("Sinal: ");
        display.print(ultimoRssiRecebido);
        display.println(" dBm");
        display.display();

        // 3. Controle Automático de Exaustão por Temperatura
        if (!exaustaoLigada && dadosRecebidos.temperatura >= TEMPERATURA_LIGA_EXAUSTAO) {
            exaustaoLigada = true;
            Serial.println(">>> Exaustao LIGADA por alta temperatura <<<");
        }
        if (exaustaoLigada && dadosRecebidos.temperatura <= TEMPERATURA_DESLIGA_EXAUSTAO) {
            exaustaoLigada = false;
            Serial.println(">>> Exaustao DESLIGADA por temperatura normal <<<");
        }

        // 4. Responde ao Emissor via LoRa (Devolve a força do sinal para o OLED do carro)
        enviarRespostaLoRa();

        // 5. Aplica o nosso Filtro Inteligente antes de mandar para o banco na Hostgator
        if (precisaEnviarParaNuvem(dadosRecebidos)) {
            enviarHostgator = true;
            atualizarHistoricoEnvio(dadosRecebidos);
        }
    }
}

void enviarParaHostgator() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure(); // Ignora validação do certificado SSL

        HTTPClient http;
        
        if (http.begin(client, "https://estudecertoja.com.br/salvar_dados.php")) {
            http.addHeader("Content-Type", "application/json");
           
            float t = isnan(dadosRecebidos.temperatura) ? 0.0 : dadosRecebidos.temperatura;
            float h = isnan(dadosRecebidos.umidade) ? 0.0 : dadosRecebidos.umidade;

            String jsonPayload = "{";
            jsonPayload += "\"estacao_id\":\"ESTACAO_01\",";
            jsonPayload += "\"temperatura\":" + String(t, 1) + ",";
            jsonPayload += "\"umidade\":" + String(h, 1) + ",";
            jsonPayload += "\"contador\":" + String(dadosRecebidos.contador) + ",";
            jsonPayload += "\"energia_ok\":" + String(dadosRecebidos.energiaOk ? "true" : "false") + ",";
            jsonPayload += "\"exaustao_ligada\":" + String(exaustaoLigada ? "true" : "false") + ",";
            jsonPayload += "\"exaustao_ok\":" + String(dadosRecebidos.exaustaoOK ? "true" : "false") + ",";
            jsonPayload += "\"aquecedor_ligado\":" + String(comandoEnvio.acionarAquecedor ? "true" : "false") + ",";
            jsonPayload += "\"rssi\":" + String(ultimoRssiRecebido); 
            jsonPayload += "}";

            int httpResponseCode = http.POST(jsonPayload);

            Serial.print(">>> RESPOSTA HOSTGATOR (HTTP): ");
            Serial.println(httpResponseCode);

            if (httpResponseCode > 0) {
                String response = http.getString();
                Serial.print("Resposta do Servidor: ");
                Serial.println(response);
            } else {
                Serial.print("[ERRO HTTP] Motivo: ");
                Serial.println(http.errorToString(httpResponseCode).c_str());
            }

            http.end();
        } else {
            Serial.println("[ERRO] Nao foi possivel iniciar a conexao HTTPS");
        }
    }
}

void lerComandoHostgator()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiClientSecure client;
        client.setInsecure();

        HTTPClient http;

        if (http.begin(client, "https://estudecertoja.com.br/zoocontrol/ler_comando.php"))
        {
            int httpCode = http.GET();

            Serial.print("Resposta comando: ");
            Serial.println(httpCode);

            if (httpCode > 0)
            {
                String resposta = http.getString();


                //--- bloco q lê ultima linha db comandos ---
                JsonDocument doc;
                DeserializationError erro = deserializeJson(doc, resposta);

                if (erro)
                {
                    Serial.println("Erro ao interpretar JSON");
                    http.end();
                    return;
                }

                int idComando = doc["id"].as<int>();
                Serial.print("ID do comando: "); //2 printSerial p confirmar que idComando recebeu id certo
                Serial.println(idComando); 

                if (idComando == 0)
                {
                    http.end();
                    return;
                }

                comandoEnvio.acionarAquecedor = doc["acionar_aquecedor"].as<int>();
                // comandoEnvio.ligarExaustao = doc["ligar_exaustao"].as<int>(); //não mudar estado de exaustor pelo banco

                //bloco teste, desligadar após confirmar funcionamento
                Serial.print("***Comando enviado - Aquecedor: ");
                Serial.print(comandoEnvio.acionarAquecedor);
                Serial.print(" | Exaustao: ");
                Serial.println(comandoEnvio.ligarExaustao);

                //--- Bloco q envia comando recebido db comandos>net>receptor p emissor
                esp_err_t resultado = esp_now_send(macEmissor,
                                   (uint8_t *)&comandoEnvio,
                                   sizeof(comandoEnvio));

                Serial.print("ESP-NOW aquecedor: ");
                Serial.println(resultado);

                if (resultado == ESP_OK)
                {
                    WiFiClientSecure client;
                    client.setInsecure();

                    HTTPClient http;

                    String url = "https://estudecertoja.com.br/zoocontrol/executar_comando.php?id=" + String(idComando);

                    if (http.begin(client, url))
                    {
                        int resposta = http.GET();

                        Serial.print("Atualizar comando: ");
                        Serial.println(resposta);

                        http.end();
                    }
                }

                //------------------------------------------------

                Serial.print("Aquecedor: ");
                Serial.println(comandoEnvio.acionarAquecedor);

                Serial.print("Exaustao: ");
                Serial.println(comandoEnvio.ligarExaustao);

                //--------------------------------------

                Serial.println("JSON recebido:");
                Serial.println(resposta);
            }

            http.end();
        }
    }
}

void AoReceber(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    if (len == sizeof(dadosRecebidos)) {
        memcpy(&dadosRecebidos, incomingData, sizeof(dadosRecebidos));
        
        Serial.print("Temp: ");
        Serial.print(dadosRecebidos.temperatura, 1);
        Serial.print("  ExaustaoLigada: ");
        Serial.println(exaustaoLigada);

        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("PLACA B (RECEPTOR)");
        display.setCursor(0, 16);
        display.print("Temp: ");
        display.print(dadosRecebidos.temperatura, 1);
        display.println(" C");
        display.setCursor(0, 32);
        display.print("Umid: ");
        display.print(dadosRecebidos.umidade, 1);
        display.println(" %");
        display.setCursor(0, 48);
        display.print("Pacote: #");
        display.println(dadosRecebidos.contador);
        display.display();

        if (!exaustaoLigada && dadosRecebidos.temperatura >= TEMPERATURA_LIGA_EXAUSTAO)
        {
            exaustaoLigada = true;

            comandoEnvio.acionarAquecedor = false;
            Serial.print("Enviando LigarExaustao = ");
            comandoEnvio.ligarExaustao = true;
            Serial.println(comandoEnvio.ligarExaustao);

            esp_now_send(macEmissor,
                         (uint8_t *)&comandoEnvio,
                         sizeof(comandoEnvio));

            Serial.println("Sistema de Exaustao LIGADO");
        }
        if (exaustaoLigada && dadosRecebidos.temperatura <= TEMPERATURA_DESLIGA_EXAUSTAO)
        {
            exaustaoLigada = false;

            comandoEnvio.acionarAquecedor = false;

            Serial.print("Enviando LigarExaustao = ");
            comandoEnvio.ligarExaustao = false;
            Serial.println(comandoEnvio.ligarExaustao);

            esp_now_send(macEmissor,
                        (uint8_t *)&comandoEnvio,
                        sizeof(comandoEnvio));

            Serial.println("Sistema de Exaustao DESLIGADO");
        }

        //enviarHostgator = true;

        // Verifica se os dados sofreram variação relevante ou se deu o tempo do Heartbeat
        if (precisaEnviarParaNuvem(dadosRecebidos))
        {
            enviarHostgator = true;
            atualizarHistoricoEnvio(dadosRecebidos);
        }

    }
}

#define LORA_SS    18
#define LORA_RST   14
#define LORA_DIO0  26

void setup() {
    Serial.begin(115200);
    delay(1000); // Tempo para estabilizar a Serial

    Serial.println("Passo 1 Lora");
    SPI.begin(5, 19, 27, 18);
    Serial.println("Passo 2 Lora");
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    Serial.println("Passo 3 Lora");
    if (!LoRa.begin(915E6))
    {
        Serial.println("Falha ao iniciar LoRa");
        while (true);
    }
    Serial.println("LoRa iniciado com sucesso");

    Wire.begin(21, 22);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Conectando Wi-Fi...");
    display.display();

    if (!LittleFS.begin(true)) {
        Serial.println("Erro ao montar o LittleFS!");
    }

    // Tenta conectar no Wi-Fi Residencial
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin("NILSON 2.4", "81111270in"); // Mantido exatamente como no documento
    
    int tentativas = 0;
    while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
        delay(500);
        Serial.print(".");
        tentativas++;
    }

    display.clearDisplay();
    display.setCursor(0, 0);

    if (WiFi.status() == WL_CONNECTED) {
        canalRoteador = WiFi.channel();

        Serial.println("\n>>> WiFi conectado <<<");
        Serial.print("IP Receptor: ");
        Serial.println(WiFi.localIP());
        Serial.print("Canal WiFi Roteador: ");
        Serial.println(canalRoteador);

        // Exibe IP e Canal no OLED
        display.println("WIFI CONECTADO!");
        display.setCursor(0, 16);
        display.print("IP: ");
        display.println(WiFi.localIP());
        display.setCursor(0, 32);
        display.print("Canal: ");
        display.println(canalRoteador);
    } else {
        Serial.println("\n[ERRO] Falha ao conectar no Wi-Fi!");
        display.println("FALHA NO WI-FI!");
        display.setCursor(0, 16);
        display.println("Verifique SSID/Senha");
    }
    display.display();

    if (esp_now_init() == ESP_OK) {
        esp_now_register_recv_cb(AoReceber);
        memcpy(peerInfo.peer_addr, macEmissor, 6);
        peerInfo.channel = canalRoteador;
        peerInfo.encrypt = false;
        esp_now_add_peer(&peerInfo);
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html");
    });
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/style.css", "text/css");
    });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/script.js", "text/javascript");
    });

    server.on("/dados", HTTP_GET, [](AsyncWebServerRequest *request) {
        float t = isnan(dadosRecebidos.temperatura) ? 0.0 : dadosRecebidos.temperatura;
        float h = isnan(dadosRecebidos.umidade) ? 0.0 : dadosRecebidos.umidade;

        String json = "{";
        json += "\"temperatura\":" + String(t, 1) + ",";
        json += "\"umidade\":" + String(h, 1) + ",";
        json += "\"contador\":" + String(dadosRecebidos.contador) + ",";
        json += "\"energia_ok\":" + String(dadosRecebidos.energiaOk ? "true" : "false");
        json += ",\"exaustao_ligada\":" + String(exaustaoLigada ? "true" : "false");
        json += ",\"exaustao_ok\":" + String(dadosRecebidos.exaustaoOK ? "true" : "false");
        json += "}";
    
        request->send(200, "application/json", json);
    });

    server.on("/ligar-aquecedor", HTTP_POST, [](AsyncWebServerRequest *request) {
        comandoEnvio.acionarAquecedor = true;
        esp_err_t result = esp_now_send(macEmissor, (uint8_t *)&comandoEnvio, sizeof(comandoEnvio));
        
        if (result == ESP_OK) {
            request->send(200, "text/plain", "Comando enviado com sucesso");
        } else {
            request->send(500, "text/plain", "Erro ao enviar via ESP-NOW");
        }
    });

    server.begin();
}

void loop() 
{
    //EscutaPacotesLoraDoEmissor
    verificarRecebimentoLoRa();

    //VerificaBancoHostgatorPeriodicamente
    if (millis() - ultimoComando >= 5000)
    {
        ultimoComando = millis();
        lerComandoHostgator();
    }

    //SeFiltroDadosAutorizar,EnviaLeituraParaHostgator
    if (enviarHostgator)
    {
        enviarHostgator = false;
        enviarParaHostgator();
    }

}