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
