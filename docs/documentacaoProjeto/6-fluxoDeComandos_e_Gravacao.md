## 6. Fluxo de Comandos e Gravação (PlatformIO CLI)

1. **Upload do LittleFS (Receptor / COM3):**
   `pio run -e ttgo_receptor -t uploadfs`
2. **Upload do Firmware do Receptor (COM3):**
   `pio run -e ttgo_receptor -t upload`
3. **Upload do Firmware do Emissor (COM4):**
   `pio run -e ttgo_emissor -t upload`
