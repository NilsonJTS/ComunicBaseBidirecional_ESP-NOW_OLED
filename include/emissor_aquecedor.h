#pragma once

void iniciarAquecedor();
void ligarAquecedor();
bool aquecedorEstaAtivo();
void atualizarAquecedor(); // chamar no loop(): desliga sozinho após DURACAO_AQUECEDOR