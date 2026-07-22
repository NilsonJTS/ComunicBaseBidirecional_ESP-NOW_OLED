#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// MAC da PLACA A
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

  // Exibe o recebido no OLED
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

  // Prepara e envia a resposta de confirmacao (ACK)
  dadosResposta.contador = dadosRecebidos.contador;
  strcpy(dadosResposta.status, "ACK OK");
  esp_now_send(macDestino, (uint8_t *)&dadosResposta, sizeof(dadosResposta));
}

void setup()
{
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
}

void loop()
{
  // O loop fica livre, a resposta eh disparada pela recepcao de dados
}