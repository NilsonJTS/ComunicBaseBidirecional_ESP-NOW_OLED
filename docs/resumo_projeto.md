### Conteúdo do Documento de Resumo

```markdown
# Documento de Handoff & Resumo de Contexto - Projeto Estação de Sensores ESP32

## **Status do Projeto:** Funcional (Comunicação ESP-NOW + Servidor Web Assíncrono com LittleFS + HTML/CSS/JS Decouplados)

## 1. Visão Geral e Objetivo

Desenvolvimento de um sistema de monitoramento de sensores distribuído utilizando placas ESP32. O sistema consiste em módulos **Emissores** (que leem sensores como DHT11 e transmitem via ESP-NOW) e um módulo **Receptor/Central** (que recebe os dados via rádio, exibe no display local OLED, hospeda uma página Web local no LittleFS e fornece uma API JSON em tempo real via Wi-Fi para smartphones/navegadores).

## 1.1 Objetivo

## Chegar a um protótipo tipo maquete com um sistema aplicavel em que poderemos acionar um aquecedor (secador de cabelos no protótipo) via site hospedado em host, causando elevação de temperatura, e forçando o sistema de exaustão (ventilador) trazendo a temperatura de volta ao normal, com acionamento e desligamento automático.

## 2. Hardware e Infraestrutura

- **Placas:** ESP32 LoRa LyliGo (TTGO T-Display / DevKit v1) - 2 unidades
  - interface para ESP32 LoRa REV2.0/2022 com fonte de alimentação integrada 127vac~240vav-5vdc 0.6A - 2 unidades
- **Mapeamento de Portas COM (Hub USB com Fonte Externa 5v 3A):**
  - **COM3:** Placa Receptora (Central Web)
  - **COM4:** Placa Emissora (Sensor DHT11)
- **Periféricos & Sensores:**
  - Display OLED I2C (SSD1306): Pinos SDA=21, SCL=22 - cada unidade ESP32 carrega um Oled
  - Sensor DHT11 (na placa emissora) 1 unidade
  - Relé de estado solido FOTEK SSR-25DA 25A 3-32vdc/24-380vac com dissipadores/fixadores de trilho DIN TS35 - 2 unidades
  - placa de relé HW-316 com 4 relés independentes JQC3F-05vdc-c 250vac 10A
  - 4 mini protoboards
  - 1 secador de cabelos residencial de 3500W 127v
  - 1 ventilador residencial de 50W 127v
- **Rede / Wi-Fi:**
  - Modo Wi-Fi do Receptor: `WIFI_AP_STA` (Canal 1 fixo)
  - SSID da Rede Local: `ESP32_Estacao` | Senha: `12345678`
  - IP de Acesso: `192.168.4.1`
    **Recursos extras disponíveis no laboratório**
  - Multimetro simples de uso geral para medições de componentes, continuidade e Vac Vdc
  - Roteador Wi-Fi residencial `NILSON 2.4`
  - Senha `81111270in`
  - IP da primeira transmissão `192.168.15.84` observar para ver se mantém, se preciso, aprender a fixar um ip.
  - Canal de transmissão `11`
  - Assinatura Hostgator M anual
  **Conexão / DB hostgator**
  - $servername = "localhost";           // local fisico do DB
  - $username   = "estude43_userNilson"; // usuário com liberdade total
  - $password   = "GjTX@@jqyD@E";        // senha
  - $dbname     = "estude43_estacao_db"; // nome do banco

---

## 3. Arquitetura de Software e Decisões Técnicas

1. **Ambiente de Desenvolvimento:** VS Code + PlatformIO.
2. **Separação de Ambientes (`platformio.ini`):**
   - Uso da diretiva `build_src_filter` para compilar apenas o arquivo relevante em cada placa sem necessidade de renomear extensões.
   - Isolamento de dependências de bibliotecas (`lib_deps`) por ambiente (Emissor não carrega servidor Web).
3. **Sistema de Arquivos (`LittleFS`):**
   - Arquivos web armazenados na pasta `data/` do projeto e gravados na Flash do ESP32 via target `uploadfs`.
   - Estrutura Web totalmente modularizada e desacoplada:
     - `data/index.html` (Estrutura da página)
     - `data/style.css` (Estilização)
     - `data/script.js` (Lógica AJAX com `fetch()` assíncrono a cada 2s com cache-busting)
4. **Comunicação e Servidor:**
   - **Emissor -> Receptor:** ESP-NOW (envio de struct de dados com temperatura, umidade e contador de pacotes).
   - **Receptor -> Cliente Web:** Servidor assíncrono (`ESPAsyncWebServer`) servindo arquivos estáticos e endpoint `/dados` fornecendo JSON.

---

## 4. Estrutura de Pastas do Projeto

MEU_PROJETO/
├── platformio.ini # Configuração dos ambientes emissor e receptor
│
├── data/ # Arquivos do Servidor Web (LittleFS - Receptor)
│ ├── index.html # Estrutura HTML
│ ├── style.css # Estilo visual
│ └── script.js # Atualização dinâmica (AJAX fetch)
│
├── docs/ # Arquivos de documentação e codigos externos complementares 
│ ├── anotacoesNilson.md
│ ├── leituras_sensores.sql // criação de bando sql no hostgator
│ ├── resumo_projeto.md // documento completo de documentação projeto
│ └── salvar_dados.php // codigo de recepção de dados e create em leituras_sensores
│
└── src/
├── emissor.cpp # Código leve: Leitura do DHT11 + Envio ESP-NOW
└── receptor.cpp # Código Central: ESP-NOW + OLED + WebServer + LittleFS

---

## 5. Arquivos de Configuração e Código Atual

### 5.1. `platformio.ini`

[env]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
board_build.filesystem = littlefs

# CONFIGURAÇÃO DA PLACA EMISSORA

[env:ttgo_emissor]
upload_port = COM4
monitor_port = COM4
build_src_filter = +<\*> -<receptor.cpp> -<main.cpp>
lib_deps =
adafruit/DHT sensor library
adafruit/Adafruit Unified Sensor
adafruit/Adafruit GFX Library
adafruit/Adafruit SSD1306

# CONFIGURAÇÃO DA PLACA RECEPTORA

[env:ttgo_receptor]
upload_port = COM3
monitor_port = COM3
build_src_filter = +<\*> -<emissor.cpp> -<main.cpp>
lib_deps =
adafruit/Adafruit GFX Library
adafruit/Adafruit SSD1306
https://github.com/me-no-dev/ESPAsyncWebServer.git
https://github.com/me-no-dev/AsyncTCP.git

### 5.2. `src/emissor.cpp`

colar aqui

### 5.3. `src/receptor.cpp`

colar aqui

### 5.4. Arquivos da Pasta `data/`
#### `data/index.html`

#### `data/style.css`

#### `data/script.js`

---

## 6. Fluxo de Comandos e Gravação (PlatformIO CLI)

1. **Upload do LittleFS (Receptor / COM3):**
   `pio run -e ttgo_receptor -t uploadfs`
2. **Upload do Firmware do Receptor (COM3):**
   `pio run -e ttgo_receptor -t upload`
3. **Upload do Firmware do Emissor (COM4):**
   `pio run -e ttgo_emissor -t upload`

---

## 7. Próximos Passos

8 - inserção de site para acesso global na hostgator, tão simples quanto o site do LittleFS
9 - inserção de logo e relatorio de dados do banco sql no site
9.1 - Transição de rádio: Migrar a comunicação entre Emissor e Receptor de:
        protocolo ESP-NOW 2,4GHz para a biblioteca nativa do chip LoRa SX127x 915MHz, mantendo o Wi-Fi do Receptor dedicado apenas à internet.
10 - teste de campo para verificação de funcionamento à distancia com powerbank alimentando o receptor, e teste via internet com smartfone
11 - configurar nova unidade esp32 LyLigo oled (que ainda será adquirida) para servir de gateway e aumentar alcance
12 - definir todas as necessidades de aplicações com seus respectivos sensores (apenas quando todos os passos anteriores forem consolidados)
13 - Refatorar o código C++ aplicando **Programação Orientada a Objetos (POO)** com classes dedicadas.
        (`DisplayManager`, `WebServerManager`, `SensorManager`).
14 - Suporte a múltiplos transmissores.
15 - Sistema de verificação de tensão e corrente com sensor sct-013 com bias ou ofset (usando entrada analógica ADC-Analog to Digital Converter).
        Definir 3 estados de verificação, desligado->corrente 0, normal, falha->corrente muito abaixo ou muito acima, objetivo é verificar não só liga/desliga, mas também aparelhos com mal funcionamento.
16 - Adquirir kit de baixo custo para nó emissor remoto composto por:
        * Microcontrolador/LoRa: Placa ESP32 LoRa 915MHz sem OLED (ex: Wireless Stick Lite ou TTGO T-Beam).
        * Alimentação Solar: Painel solar 5V (3W), carregador solar MPPT/TP4056, 1 x bateria Li-Ion 18650 (3,7V) e regulador LDO 3,3V.
        * Proteção: Caixa estanque IP65 para testes e operação prolongada em ambiente externo.
17 - Sistema de filtro de registro de dados em banco sql, para não registrar cada segundo de verificação.
18 - Implementar sistema com POO de forma a facilitar o recurso de instalação de novas unidades e código organizado e reutilizavel.

---

## 8. Informações gerais de parametros para IA auxiliar

- respostas diretas e sem abstrações.
- cada fase, e cada passo deve ser respeitado, só passamos adiante depois de: funcionamento confirmado, commit local e nuvem, incrementação de documentação do projeto.
- quando indicar o passo a passo de ligações eletrônicas, pode ser uma lista de itens curtos, pois tenho maior domínio.
- manter os números dos pinos descritos nos códigos, ou, avisar quando precisar redefinir algum em nova versão de código.

## 8.1 Lista de commits com descrições e respectivas datas

- Date: Tue Jul 28 12:22:20 2026 MiracaoReceptorParaModoSTA(WiFi residencia)canal11_IPdinamicoEintegraçãoESP-NOWcomEmissor
- Date: Mon Jul 27 23:07:33 2026 sistemaExaustaoComAcionamentoDesligamentoAutoIndicadorHTMLNoRetornoGPIO35
- Date: Mon Jul 27 12:49:25 2026 atualização de documentação
- Date: Sun Jul 26 22:18:42 2026 botaoHTML_AionaAquecedor30seg/retornoEnergiaConfirmaAcionamentoAcendeBolinhaHTML
- Date: Sat Jul 25 17:39:53 2026 Adição de documento resumo projeto
- Date: Sat Jul 25 17:13:46 2026 adiciona serv web assincrono com LittleFs (html,css,js)
- Date: Wed Jul 22 00:48:32 2026 inserção DHT11 sensor temperatura/umidade
- Date: Tue Jul 21 15:17:37 2026 Comunic Bidirecional base ESP-NOW OLED
```
