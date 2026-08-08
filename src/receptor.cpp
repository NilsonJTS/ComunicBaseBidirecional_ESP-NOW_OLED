#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include "receptor_display.h"
#include "receptor_lora.h"
#include "receptor_cloud.h"
#include "receptor_webserver.h"
#include "receptor_estado.h"

unsigned long ultimoComando = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);

    iniciarLoRaReceptor();
    iniciarDisplayReceptor();

    if (!LittleFS.begin(true)) {
        Serial.println("Erro ao montar o LittleFS!");
    }

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin("NILSON 2.4", "81111270in");

    int tentativas = 0;
    while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
        delay(500);
        Serial.print(".");
        tentativas++;
    }

    display.clearDisplay();
    display.setCursor(0, 0);

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n>>> WiFi conectado <<<");
        Serial.print("IP Receptor: ");
        Serial.println(WiFi.localIP());

        display.println("WIFI CONECTADO!");
        display.setCursor(0, 16);
        display.print("IP: ");
        display.println(WiFi.localIP());
    } else {
        Serial.println("\n[ERRO] Falha ao conectar no Wi-Fi!");
        display.println("FALHA NO WI-FI!");
        display.setCursor(0, 16);
        display.println("Verifique SSID/Senha");
    }
    display.display();

    iniciarWebServerReceptor();
}

void loop() {
    verificarRecebimentoLoRa();

    if (millis() - ultimoComando >= 5000) {
        ultimoComando = millis();
        lerComandoHostgator();
    }

    if (enviarHostgator) {
        enviarHostgator = false;
        enviarParaHostgator();
    }
}