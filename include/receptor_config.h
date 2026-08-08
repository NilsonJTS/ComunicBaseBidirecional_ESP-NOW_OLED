#pragma once

// --- Controle automático de exaustão ---
const float TEMPERATURA_LIGA_EXAUSTAO = 30.0;
const float TEMPERATURA_DESLIGA_EXAUSTAO = 28.0;

// --- Filtro de envio para a nuvem (Hostgator) ---
const unsigned long INTERVALO_MAX_ENVIO = 300000; // 5 minutos (heartbeat)
const float DELTA_TEMP_MIN = 0.5;   // variação mínima de temperatura (°C)
const float DELTA_UMID_MIN = 2.0;   // variação mínima de umidade (%)
const int DELTA_RSSI_MIN = 5;       // variação mínima de sinal LoRa (dBm)