**Codigos Bash**

//Codigo para upload de LittleFS (faz upload de html,css,js)
pio run -e ttgo_receptor -t uploadfs

//Codigo para upload de receptor
pio run -e ttgo_receptor -t upload

//conectar o smartfone usando navegador
wifi ESP32_Estaca - http://192.168.4.1

//uso do git e github
git status
git add .
git commit -m "mensagem aqui"
git push
git log //para ver historico de commits

//pesquisar!
protocolos industriais para transmissão de tensão em controladores por dezenas de metros
RS-485
CAN
Modbus

//possibilidades de montagem de sistema em poste

### 1º caixa dividindo modulo de processamento e transmissão, de alta tensão e sensores

      Caixa superior

┌───────────────────┐
│ ESP32 + LoRa      │
│ OLED + Antena     │
│ Fonte             │
└───────────────────┘
        │
        │ Conduíte rígido
        │
┌───────────────────┐
│ Caixa inferior    │
│ SSR               │
│ SCT-013           │
│ Bornes            │
│ Ligação da rede   │
└───────────────────┘

### 2º Sistema usando um esp Master acima e slave abaixo

                    Caixa Superior
        ┌─────────────────────────────────┐
        │ ESP32 Mestre                    │
        │ • LoRa                          │
        │ • OLED                          │
        │ • Interface Web                 │
        │ • Lógica principal              │
        │                                 │
        │ Fonte 5 V / 3,3 V               │
        └─────────────────────────────────┘
                     │
                     │ UART / RS-485
                     │
             Conduíte rígido
                     │
                     │
        ┌─────────────────────────────────┐
        │ Caixa Inferior                  │
        │                                 │
        │ ESP32 Escravo                   │
        │ • Aciona SSR                    │
        │ • Lê SCT-013                    │
        │ • Lê sensores                   │
        │ • Diagnóstico local             │
        │                                 │
        │ SSR 25 A                        │
        │ SCT-013                         │
        │ Bornes                          │
        │ Proteções                       │
        └─────────────────────────────────┘
                     │
        ┌────────────┴─────────────┐
        │                          │
    Aquecedor                   Sensores
   (127/220 V)           (Temperatura, outros...)

### Fluxo de informações:

          Usuário
             │
             ▼
     ESP32 Superior

(LoRa / Interface Web)
│
Comandos e Status
│
▼
ESP32 Inferior
(Controle Local)
│ │
▼ ▼
Sensores SSR
│ │
└─────► Aquecedor
▲
│
SCT-013
(Confirma energização)
