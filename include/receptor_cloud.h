#pragma once

#include "msgStruct.h"

// Decide se a leitura atual deve ser gravada no banco (variação relevante ou heartbeat)
bool precisaEnviarParaNuvem(const struct_mensagem &dados);

// Atualiza os valores de referência usados por precisaEnviarParaNuvem
void atualizarHistoricoEnvio(const struct_mensagem &dados);

void enviarParaHostgator();
void lerComandoHostgator();