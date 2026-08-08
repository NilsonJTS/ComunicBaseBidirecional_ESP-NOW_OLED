#include "receptor_display.h"
#include "lora_pins.h"
#include <Wire.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void iniciarDisplayReceptor() {
    Wire.begin(21, 22);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Conectando Wi-Fi...");
    display.display();
}

void atualizarDisplayReceptor(const struct_mensagem &dados, int ultimoRssi) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("PLACA B (RECEPTOR)");
    display.setCursor(0, 16);
    display.print("Temp: ");
    display.print(dados.temperatura, 1);
    display.println(" C");
    display.setCursor(0, 32);
    display.print("Umid: ");
    display.print(dados.umidade, 1);
    display.println(" %");
    display.setCursor(0, 48);
    display.print("Sinal: ");
    display.print(ultimoRssi);
    display.println(" dBm");
    display.display();
}