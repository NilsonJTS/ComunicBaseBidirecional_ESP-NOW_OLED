#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

AsyncWebServer server(80);

uint8_t macDestino[] = {0xA0, 0xDD, 0x6C, 0x75, 0x0E, 0x14};

typedef struct struct_mensagem
{
  int contador;
  char status[10];
  float temperatura;
  float umidade;
} struct_mensagem;

struct_mensagem dadosRecebidos;
struct_mensagem dadosResposta;
esp_now_peer_info_t peerInfo;

void AoReceber(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
  memcpy(&dadosRecebidos, incomingData, sizeof(dadosRecebidos));

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("PLACA B (RECEPTOR)");
  display.setCursor(0, 16);
  display.print("Temp: ");
  display.print(dadosRecebidos.temperatura, 1);
  display.println(" C");
  display.setCursor(0, 32);
  display.print("Umid: ");
  display.print(dadosRecebidos.umidade, 1);
  display.println(" %");
  display.setCursor(0, 48);
  display.print("Pacote: #");
  display.println(dadosRecebidos.contador);
  display.display();

  dadosResposta.contador = dadosRecebidos.contador;
  strcpy(dadosResposta.status, "ACK OK");
  esp_now_send(macDestino, (uint8_t *)&dadosResposta, sizeof(dadosResposta));
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- INICIANDO RECEPTOR ---");

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(1);

  // 1. TENTA INICIALIZAR O LITTLEFS (Com opção de formatar em caso de erro na 1ª vez)
  if (!LittleFS.begin(true)) {
    Serial.println("❌ ERRO GRAVE: Falha ao montar o LittleFS!");
  } else {
    Serial.println("✅ LittleFS montado com sucesso!");
  }

  // 2. INICIALIZA MODO WI-FI
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("ESP32_Estacao", "12345678");
  
  Serial.print("✅ Ponto de Acesso Criado! IP: ");
  Serial.println(WiFi.softAPIP());

  // 3. INICIALIZA ESP-NOW
  if (esp_now_init() == ESP_OK) {
    Serial.println("✅ ESP-NOW Inicializado!");
    esp_now_register_recv_cb(AoReceber);
    memcpy(peerInfo.peer_addr, macDestino, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
  } else {
    Serial.println("❌ Erro ao iniciar ESP-NOW!");
  }

  // 4. CONFIGURAÇÃO DAS ROTAS DO SERVIDOR

  // Servir o arquivo JavaScript
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/script.js", "text/javascript");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("📱 Celular solicitou a pagina /");
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  server.on("/dados", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"temperatura\":" + String(dadosRecebidos.temperatura, 1) + ",";
    json += "\"umidade\":" + String(dadosRecebidos.umidade, 1) + ",";
    json += "\"contador\":" + String(dadosRecebidos.contador);
    json += "}";
    request->send(200, "application/json", json);
  });

  // 5. INICIA O SERVIDOR WEB
  server.begin();
  Serial.println("🚀 Servidor Web rodando na porta 80!");
}

void loop()
{
}