#pragma once

#include "msgStruct.h"

extern int ultimoRssi; // RSSI mais recente recebido do receptor

void iniciarLoRaEmissor();
void enviarDadosLoRa(const struct_mensagem &dados);
void verificarRespostaLoRa();