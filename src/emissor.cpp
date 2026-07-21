//bibliotecas
#include <esp_now.h> //bibl c funções protocolo esp
#include <WiFi.h> //funcionamento do radio esp32 no modo wifi correto
#include <Wire.h> //comunicacao
#include <Adafruit_GFX.h> //renderizacao grafica oled
#include <Adafruit_SSD1306.h> //renderizacao texto oled

#define SCREEN_WIDTH 128 //constantes p uso do adafruit
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //configuração adafruit para tela

// MAC da PLACA B
uint8_t macDestino[] = {0xA0, 0xDD, 0x6C, 0x67, 0xC6, 0x34}; //mac da receptora para envio

//estrutura de dados - pacote de comunicação
typedef struct struct_mensagem {
  int contador;
  char status[10];
} struct_mensagem;

struct_mensagem dadosEnvio;
struct_mensagem dadosRecebidos;
esp_now_peer_info_t peerInfo;

//função callback e recepção
void AoReceber(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&dadosRecebidos, incomingData, sizeof(dadosRecebidos));
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("PLACA A");
  display.setCursor(0, 20);
  display.print("Enviado: ");
  display.println(dadosEnvio.contador);
  display.setCursor(0, 40);
  display.print("Resp: ");
  display.println(dadosRecebidos.status);
  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(1);

  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(AoReceber);

  memcpy(peerInfo.peer_addr, macDestino, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  dadosEnvio.contador = 0;
}

void loop() {
  dadosEnvio.contador++;
  strcpy(dadosEnvio.status, "PING");

  // Envia dado para a Placa B
  esp_now_send(macDestino, (uint8_t *) &dadosEnvio, sizeof(dadosEnvio));

  delay(2000);
}