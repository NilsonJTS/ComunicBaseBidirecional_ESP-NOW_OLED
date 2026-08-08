#pragma once

#include <Adafruit_SSD1306.h>
#include "msgStruct.h"

extern Adafruit_SSD1306 display;

void iniciarDisplayEmissor();
void atualizarDisplayEmissor(const struct_mensagem &dados, int ultimoRssi);