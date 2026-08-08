#pragma once

#include "msgStruct.h"

// Última leitura recebida do emissor
extern struct_mensagem dadosRecebidos;

// Comando a ser enviado de volta ao emissor (aquecedor/exaustão/rssi)
extern struct_comando comandoEnvio;

// Estado atual do relé de exaustão
extern bool exaustaoLigada;

// Força do sinal do último pacote LoRa recebido do emissor
extern int ultimoRssiRecebido;

// Sinaliza para o loop() que há dados prontos para enviar ao Hostgator
extern volatile bool enviarHostgator;