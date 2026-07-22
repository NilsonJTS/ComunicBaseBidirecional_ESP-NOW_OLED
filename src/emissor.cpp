// bibliotecas
#include <esp_now.h>          //bibl c funções protocolo esp
#include <WiFi.h>             //funcionamento do radio esp32 no modo wifi correto
#include <Wire.h>             //comunicacao
#include <Adafruit_GFX.h>     //renderizacao grafica oled
#include <Adafruit_SSD1306.h> //renderizacao texto oled
#include <DHT.h>

#define SCREEN_WIDTH 128 // constantes p uso do adafruit
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // configuração adafruit para tela

// configuração do Sensor DHT11 no GPIO15
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// MAC da PLACA B
uint8_t macDestino[] = {0xA0, 0xDD, 0x6C, 0x67, 0xC6, 0x34}; // mac da receptora para envio

// estrutura de dados - pacote de comunicação
typedef struct struct_mensagem
{
  int contador;
  char status[10];
  float temperatura;
  float umidade;
} struct_mensagem;

struct_mensagem dadosEnvio;
struct_mensagem dadosRecebidos;
esp_now_peer_info_t peerInfo;

// função callback e recepção
void AoReceber(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
  memcpy(&dadosRecebidos, incomingData, sizeof(dadosRecebidos));

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("PLACA A (DHT11)");
  display.setCursor(0, 16);
  display.print("Temp: ");
  display.print(dadosEnvio.temperatura, 1);
  display.println(" C");
  display.setCursor(0, 32);
  display.print("Umid: ");
  display.print(dadosEnvio.umidade, 1);
  display.println(" %");
  display.setCursor(0, 48);
  display.print("Resp: ");
  display.println(dadosRecebidos.status);
  display.display();
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(1);

  dht.begin(); // inicializa o dht11

  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(AoReceber);

  memcpy(peerInfo.peer_addr, macDestino, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  dadosEnvio.contador = 0;
}

void loop()
{
  dadosEnvio.contador++;
  strcpy(dadosEnvio.status, "PING");

  // leitura do dht11
  dadosEnvio.umidade = dht.readHumidity();
  dadosEnvio.temperatura = dht.readTemperature();

  // Envia dado para a Placa B
  esp_now_send(macDestino, (uint8_t *)&dadosEnvio, sizeof(dadosEnvio));

  delay(2000);
}