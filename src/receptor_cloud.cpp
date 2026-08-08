#include "receptor_cloud.h"
#include "receptor_estado.h"
#include "receptor_config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Arduino.h>

static float ultTempEnviada = -999.0;
static float ultUmidEnviada = -999.0;
static bool ultEnergiaEnviada = false;
static bool ultExaustaoOKEnviada = false;
static bool ultExaustaoLigadaEnviada = false;
static bool ultAquecedorEnviado = false;
static int ultRssiEnviado = -999;

static bool primeiroEnvioRealizado = false;
static unsigned long tempoUltimoEnvioNuvem = 0;

bool precisaEnviarParaNuvem(const struct_mensagem &dados) {
    if (!primeiroEnvioRealizado) {
        Serial.println(">>> DISPARO BANCO | Motivo: 1ª leitura apos ligar");
        return true;
    }

    if (millis() - tempoUltimoEnvioNuvem >= INTERVALO_MAX_ENVIO) {
        Serial.println(">>> DISPARO BANCO | Motivo: Tempo maximo (5min) atingido");
        return true;
    }

    if (isnan(dados.temperatura) || isnan(dados.umidade)) {
        return false;
    }

    int tempAtualInt = round(dados.temperatura * 10.0);
    int tempUltimaInt = round(ultTempEnviada * 10.0);

    int umidAtualInt = round(dados.umidade * 10.0);
    int umidUltimaInt = round(ultUmidEnviada * 10.0);

    int deltaTempInt = round(DELTA_TEMP_MIN * 10.0);
    int deltaUmidInt = round(DELTA_UMID_MIN * 10.0);

    bool tempMudou = abs(tempAtualInt - tempUltimaInt) >= deltaTempInt;
    bool umidMudou = abs(umidAtualInt - umidUltimaInt) >= deltaUmidInt;
    bool rssiMudou = abs(ultimoRssiRecebido - ultRssiEnviado) >= DELTA_RSSI_MIN;

    bool estadoMudou = (dados.energiaOk != ultEnergiaEnviada) ||
                        (dados.exaustaoOK != ultExaustaoOKEnviada) ||
                        (exaustaoLigada != ultExaustaoLigadaEnviada) ||
                        (comandoEnvio.acionarAquecedor != ultAquecedorEnviado);

    if (tempMudou || umidMudou || estadoMudou || rssiMudou) {
        Serial.print(">>> DISPARO BANCO | Motivo: ");
        if (tempMudou) Serial.print("[Temp] ");
        if (umidMudou) Serial.print("[Umid] ");
        if (estadoMudou) Serial.print("[Relês/Energia] ");
        if (rssiMudou) Serial.print("[Sinal LoRa (RSSI)]");
        Serial.println();
        return true;
    }

    return false;
}

void atualizarHistoricoEnvio(const struct_mensagem &dados) {
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

void enviarParaHostgator() {
    if (WiFi.status() != WL_CONNECTED) return;

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;

    if (!http.begin(client, "https://estudecertoja.com.br/salvar_dados.php")) {
        Serial.println("[ERRO] Nao foi possivel iniciar a conexao HTTPS");
        return;
    }

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
        Serial.print("Resposta do Servidor: ");
        Serial.println(http.getString());
    } else {
        Serial.print("[ERRO HTTP] Motivo: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    http.end();
}

void lerComandoHostgator() {
    if (WiFi.status() != WL_CONNECTED) return;

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;

    if (!http.begin(client, "https://estudecertoja.com.br/zoocontrol/ler_comando.php")) return;

    int httpCode = http.GET();

    Serial.print("Resposta comando: ");
    Serial.println(httpCode);

    if (httpCode > 0) {
        String resposta = http.getString();

        JsonDocument doc;
        DeserializationError erro = deserializeJson(doc, resposta);

        if (erro) {
            Serial.println("Erro ao interpretar JSON");
            http.end();
            return;
        }

        int idComando = doc["id"].as<int>();
        Serial.print("ID do comando: ");
        Serial.println(idComando);

        if (idComando == 0) {
            http.end();
            return;
        }

        // Guarda o comando; o envio real ao emissor acontece no próximo
        // ciclo de resposta LoRa (ver receptor_lora.cpp / enviarRespostaLoRa)
        comandoEnvio.acionarAquecedor = doc["acionar_aquecedor"].as<int>();

        Serial.print("***Comando armazenado - Aquecedor: ");
        Serial.println(comandoEnvio.acionarAquecedor);

        // Confirma ao Hostgator que o comando foi lido e aplicado localmente
        WiFiClientSecure clienteConfirma;
        clienteConfirma.setInsecure();
        HTTPClient httpConfirma;

        String url = "https://estudecertoja.com.br/zoocontrol/executar_comando.php?id=" + String(idComando);

        if (httpConfirma.begin(clienteConfirma, url)) {
            int respostaConfirma = httpConfirma.GET();
            Serial.print("Atualizar comando: ");
            Serial.println(respostaConfirma);
            httpConfirma.end();
        }
    }

    http.end();
}