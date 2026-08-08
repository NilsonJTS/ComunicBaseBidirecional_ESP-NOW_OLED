#pragma once

// Struct enviada do Emissor -> Receptor (leituras dos sensores)
typedef struct struct_mensagem {
    int contador;
    float temperatura;
    float umidade;
    bool energiaOk;
    bool exaustaoOK;
} struct_mensagem;

// Struct enviada do Receptor -> Emissor (comandos e RSSI)
typedef struct struct_comando {
    bool acionarAquecedor;
    bool ligarExaustao;
    int rssi;
} struct_comando;