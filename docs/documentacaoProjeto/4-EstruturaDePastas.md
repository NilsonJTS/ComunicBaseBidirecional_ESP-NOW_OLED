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