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
  - Tabela      = "leituras_sensores";   // dados: id|estacao_id|temperatura|umidade|contador
                                            energia_ok|aquecedor_ligado|exaustao_ligada|exaustao_ok|data_hora
  - Tabela      = "comandos";            // filaServiços, dados: id|acionar_aquecedor|ligar_exaustor
                                            executando|data_hora