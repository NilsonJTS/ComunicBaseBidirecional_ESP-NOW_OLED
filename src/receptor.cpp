#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <HTTPClient.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
AsyncWebServer server(80);

volatile bool enviarHostgator = false;

int canalRoteador = 1;
const float TEMPERATURA_LIGA_EXAUSTAO = 30.0;
const float TEMPERATURA_DESLIGA_EXAUSTAO = 28.0;

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
} struct_comando;

struct_mensagem dadosRecebidos;
struct_comando comandoEnvio;
esp_now_peer_info_t peerInfo;
bool exaustaoLigada = false;

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

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
            jsonPayload += "\"aquecedor_ligado\":" + String(comandoEnvio.acionarAquecedor ? "true" : "false");
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

void AoReceber(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    if (len == sizeof(dadosRecebidos)) {
        memcpy(&dadosRecebidos, incomingData, sizeof(dadosRecebidos));
        
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
            comandoEnvio.ligarExaustao = true;

            esp_now_send(macEmissor,
                         (uint8_t *)&comandoEnvio,
                         sizeof(comandoEnvio));

            Serial.println("Sistema de Exaustao LIGADO");
        }
        if (exaustaoLigada && dadosRecebidos.temperatura <= TEMPERATURA_DESLIGA_EXAUSTAO)
        {
            exaustaoLigada = false;

            comandoEnvio.acionarAquecedor = false;
            comandoEnvio.ligarExaustao = false;

            esp_now_send(macEmissor,
                        (uint8_t *)&comandoEnvio,
                        sizeof(comandoEnvio));

            Serial.println("Sistema de Exaustao DESLIGADO");
        }

        enviarHostgator = true;

    }
}

void setup() {
    Serial.begin(115200);
    delay(1000); // Tempo para estabilizar a Serial

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
    if (enviarHostgator)
    {
        enviarHostgator = false;
        enviarParaHostgator();
    }
}