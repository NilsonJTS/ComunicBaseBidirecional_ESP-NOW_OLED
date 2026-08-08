#include "receptor_estado.h"

struct_mensagem dadosRecebidos;
struct_comando comandoEnvio;
bool exaustaoLigada = false;
int ultimoRssiRecebido = 0;
volatile bool enviarHostgator = false;