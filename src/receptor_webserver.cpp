#include "receptor_webserver.h"
#include "receptor_estado.h"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Arduino.h>

static AsyncWebServer server(80);

void iniciarWebServerReceptor() {
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
        // Sem ESP-NOW: o comando é guardado e vai ao emissor no próximo
        // ciclo de resposta LoRa (a cada pacote recebido do emissor).
        comandoEnvio.acionarAquecedor = true;
        request->send(200, "text/plain", "Comando registrado, sera enviado no proximo ciclo LoRa");
    });

    server.begin();
}