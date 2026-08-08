#include "receptor_lora.h"
#include "receptor_estado.h"
#include "receptor_display.h"
#include "receptor_cloud.h"
#include "receptor_config.h"
#include "lora_pins.h"
#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>

static void enviarRespostaLoRa() {
    comandoEnvio.rssi = ultimoRssiRecebido;
    comandoEnvio.ligarExaustao = exaustaoLigada;

    LoRa.beginPacket();
    LoRa.write((uint8_t *)&comandoEnvio, sizeof(comandoEnvio));
    LoRa.endPacket();

    comandoEnvio.acionarAquecedor = false; // reseta o gatilho após enviar
}

void iniciarLoRaReceptor() {
    SPI.begin(5, 19, 27, 18);
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(LORA_FREQUENCIA)) {
        Serial.println("Falha ao iniciar LoRa");
        while (true);
    }
    Serial.println("LoRa iniciado com sucesso");
}

void verificarRecebimentoLoRa() {
    int packetSize = LoRa.parsePacket();
    if (packetSize != sizeof(dadosRecebidos)) return;

    LoRa.readBytes((uint8_t *)&dadosRecebidos, sizeof(dadosRecebidos));
    ultimoRssiRecebido = LoRa.packetRssi();

    Serial.print(">>> Pacote LoRa Recebido! RSSI: ");
    Serial.print(ultimoRssiRecebido);
    Serial.println(" dBm");

    atualizarDisplayReceptor(dadosRecebidos, ultimoRssiRecebido);

    if (!exaustaoLigada && dadosRecebidos.temperatura >= TEMPERATURA_LIGA_EXAUSTAO) {
        exaustaoLigada = true;
        Serial.println(">>> Exaustao LIGADA por alta temperatura <<<");
    }
    if (exaustaoLigada && dadosRecebidos.temperatura <= TEMPERATURA_DESLIGA_EXAUSTAO) {
        exaustaoLigada = false;
        Serial.println(">>> Exaustao DESLIGADA por temperatura normal <<<");
    }

    enviarRespostaLoRa();

    if (precisaEnviarParaNuvem(dadosRecebidos)) {
        enviarHostgator = true;
        atualizarHistoricoEnvio(dadosRecebidos);
    }
}