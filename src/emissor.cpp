#include <Arduino.h>
#include <DHT.h>
#include "msgStruct.h"
#include "emissor_pinos.h"
#include "emissor_display.h"
#include "emissor_lora.h"
#include "emissor_aquecedor.h"

DHT dht(DHTPIN, DHTTYPE);

struct_mensagem dadosEnvio;
int contadorPacotes = 0;
unsigned long ultimoEnvio = 0;

void setup() {
    Serial.begin(115200);

    iniciarLoRaEmissor();
    iniciarDisplayEmissor();

    pinMode(DHTPIN, INPUT_PULLUP);
    dht.begin();
    delay(2000); // Estabilização do sensor

    iniciarAquecedor();

    pinMode(PINO_RETORNO_ENERGIA, INPUT);

    pinMode(PINO_EXAUSTAO, OUTPUT);
    digitalWrite(PINO_EXAUSTAO, LOW);
    pinMode(PINO_RETORNO_EXAUASTAO, INPUT);
}

void loop() {
    atualizarAquecedor();
    verificarRespostaLoRa();

    if (millis() - ultimoEnvio >= 2000) {
        ultimoEnvio = millis();

        float temp = dht.readTemperature();
        float umid = dht.readHumidity();

        if (!isnan(temp) && !isnan(umid)) {
            dadosEnvio.temperatura = temp;
            dadosEnvio.umidade = umid;
        }

        contadorPacotes++;
        dadosEnvio.contador = contadorPacotes;
        dadosEnvio.energiaOk = digitalRead(PINO_RETORNO_ENERGIA);
        dadosEnvio.exaustaoOK = digitalRead(PINO_RETORNO_EXAUASTAO);

        atualizarDisplayEmissor(dadosEnvio, ultimoRssi);
        enviarDadosLoRa(dadosEnvio);
    }
}