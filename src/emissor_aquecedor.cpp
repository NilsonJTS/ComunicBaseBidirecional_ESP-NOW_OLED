#include "emissor_aquecedor.h"
#include "emissor_pinos.h"
#include <Arduino.h>

static bool aquecedorAtivo = false;
static unsigned long tempoInicioAquecedor = 0;
static const unsigned long DURACAO_AQUECEDOR = 30000; // 30s

void iniciarAquecedor() {
    pinMode(PINO_AQUECEDOR, OUTPUT);
}

void ligarAquecedor() {
    digitalWrite(PINO_AQUECEDOR, HIGH);
    aquecedorAtivo = true;
    tempoInicioAquecedor = millis();
    Serial.println(">>> Aquecedor Ligado! <<<");
}

bool aquecedorEstaAtivo() {
    return aquecedorAtivo;
}

void atualizarAquecedor() {
    if (aquecedorAtivo && (millis() - tempoInicioAquecedor >= DURACAO_AQUECEDOR)) {
        Serial.println(">>> DESLIGAMENTO AUTOMATICO <<<");
        digitalWrite(PINO_AQUECEDOR, LOW);
        aquecedorAtivo = false;
    }
}