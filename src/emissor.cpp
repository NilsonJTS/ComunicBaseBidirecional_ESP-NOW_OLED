#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define PINO_AQUECEDOR 23
#define PINO_RETORNO_ENERGIA 34

unsigned long tempoInicioAquecedor = 0;
bool aquecedorAtivo = false;
const unsigned long DURACAO_AQUECEDOR = 30000; // 30s

uint8_t macReceptor[] = {0xA0, 0xDD, 0x6C, 0x67, 0xC6, 0x34}; // MAC do Receptor

typedef struct struct_mensagem {
    int contador;
    float temperatura;
    float umidade;
    bool energiaOk;
} struct_mensagem;

typedef struct struct_comando {
    bool acionarAquecedor;
} struct_comando;

struct_mensagem dadosEnvio;
struct_comando comandoRecebido;
esp_now_peer_info_t peerInfo;

int contadorPacotes = 0;
unsigned long ultimoEnvio = 0;

void AoReceberComando(const uint8_t *mac, const uint8_t *incomingData, int len) {
    // Força o acionamento direto sem validar o tamanho estrito da struct
    digitalWrite(PINO_AQUECEDOR, HIGH);
    aquecedorAtivo = true;
    tempoInicioAquecedor = millis();
    Serial.println(">>> AQUECEDOR LIGADO VIA ESP-NOW! <<<");
}

void setup() {
    Serial.begin(115200);
    
    // Inicialização do OLED no Emissor
    Wire.begin(21, 22);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("PLACA A (EMISSOR)");
    display.display();

    // Configuração e inicialização do DHT11
    pinMode(DHTPIN, INPUT_PULLUP);
    dht.begin();
    delay(2000); // Estabilização do sensor

    pinMode(PINO_AQUECEDOR, OUTPUT);
    digitalWrite(PINO_AQUECEDOR, LOW);
    pinMode(PINO_RETORNO_ENERGIA, INPUT);

    WiFi.mode(WIFI_STA);

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);

    if (esp_now_init() == ESP_OK) {
        esp_now_register_recv_cb(AoReceberComando);
        memcpy(peerInfo.peer_addr, macReceptor, 6);
        peerInfo.channel = 1;
        peerInfo.encrypt = false;
        esp_now_add_peer(&peerInfo);
    }
}

void loop() {
    // Desligamento automático temporizado
    if (aquecedorAtivo && (millis() - tempoInicioAquecedor >= DURACAO_AQUECEDOR)) {
        digitalWrite(PINO_AQUECEDOR, LOW);
        aquecedorAtivo = false;
    }

    if (millis() - ultimoEnvio >= 2000) {
        ultimoEnvio = millis();

        float temp = dht.readTemperature();
        float umid = dht.readHumidity();

        if (!isnan(temp) && !isnan(umid)) {
            dadosEnvio.temperatura = temp;
            dadosEnvio.umidade = umid;
        }

        contadorPacotes++;
        dadosEnvio.contador = contadorPacotes;
        dadosEnvio.energiaOk = digitalRead(PINO_RETORNO_ENERGIA);

        // Atualização do Display OLED local no Emissor
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("PLACA A (EMISSOR)");
        display.setCursor(0, 16);
        display.print("Temp: ");
        display.print(dadosEnvio.temperatura, 1);
        display.println(" C");
        display.setCursor(0, 32);
        display.print("Umid: ");
        display.print(dadosEnvio.umidade, 1);
        display.println(" %");
        display.setCursor(0, 48);
        display.print("Pacote enviado: #");
        display.println(dadosEnvio.contador);
        display.display();

        esp_now_send(macReceptor, (uint8_t *)&dadosEnvio, sizeof(dadosEnvio));
    }
}