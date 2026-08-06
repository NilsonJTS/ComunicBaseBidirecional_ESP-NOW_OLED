## 7. Próximos Passos

10 - teste de campo para verificação de funcionamento à distancia com powerbank alimentando o receptor, e teste via internet com smartfone
10.1 - Reorganização de código, preparando estrutura para POO
10.2 - Estudo direcionado ao código com objetivo de dominar cada parte do sistema.
10.3 - inserção de logo e relatorio de dados do banco sql no site
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

## 7.1 Passos Anteriores
1 - Criar comunicação simples apenas transmitindo contador de envio de pacotes a cada 1seg
2 - Inserir DHT11 fornecendo temperatura e umidade em ambos visores (emissor, receptor)
3 - Implementar site no receptor (LittleFS), para acesso e visualização de dados DHT11 via roteador esp32
4 - Implementar site: botão que aciona porta GPIO23 emissor por 30seg e led virtual que acende 30seg com retorno de GPIO23 em GPIO34
5 - Acionamento auto ventilador se temperatura >28.0 e desl auto <28.0, com retorno indicando funcionamento no html pelo GPIO35
5.1 - ConexãoResidencia: MigraçãoReceptor d modo AccessPointIsolado p RoteadorResidencia(Modo STA), obtendoIPlocal(192.168.15.84) e sintonizando a comunic ESP-NOWemissor no Canal11.
6 - criação de um banco de dados sql no provedor hostgator
7 - inserção de dados automaticamente (hora/temperatura/umidade) no banco de dados com PHP
8 - inserção de site para acesso global na hostgator, tão simples quanto o site do LittleFS
9 - Transição de rádio: Migrar a comunicação entre Emissor e Receptor de:
        protocolo ESP-NOW 2,4GHz para a biblioteca nativa do chip LoRa SX127x 915MHz, mantendo o Wi-Fi do Receptor dedicado apenas à internet.