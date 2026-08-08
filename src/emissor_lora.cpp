#include "emissor_lora.h"
#include "emissor_aquecedor.h"
#include "emissor_pinos.h"
#include "lora_pins.h"
#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>

int ultimoRssi = 0;

void iniciarLoRaEmissor() {
    SPI.begin(5, 19, 27, 18);
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(LORA_FREQUENCIA)) {
        Serial.println("Falha ao iniciar LoRa");
        while (true);
    }
    Serial.println("LoRa iniciado com sucesso");
}

void enviarDadosLoRa(const struct_mensagem &dados) {
    LoRa.beginPacket();
    LoRa.write((uint8_t *)&dados, sizeof(dados));
    LoRa.endPacket();
    Serial.println(">>> Pacote enviado via LoRa!");
}

void verificarRespostaLoRa() {
    int packetSize = LoRa.parsePacket();
    struct_comando comandoRecebido;

    if (packetSize == sizeof(comandoRecebido)) {
        LoRa.readBytes((uint8_t *)&comandoRecebido, sizeof(comandoRecebido));

        ultimoRssi = comandoRecebido.rssi;
        Serial.print("<<< Resposta LoRa recebida! RSSI: ");
        Serial.print(ultimoRssi);
        Serial.println(" dBm");

        if (comandoRecebido.acionarAquecedor && !aquecedorEstaAtivo()) {
            ligarAquecedor();
        }

        Serial.print("Exaustao: ");
        Serial.println(comandoRecebido.ligarExaustao);
        digitalWrite(PINO_EXAUSTAO, comandoRecebido.ligarExaustao ? HIGH : LOW);
    }
}