## **Status do Projeto:** Funcional (Comunicação ESP-NOW + Servidor Web Assíncrono com LittleFS + HTML/CSS/JS Decouplados)

## 1. Visão Geral e Objetivo

Desenvolvimento de um sistema de monitoramento de sensores distribuído utilizando placas ESP32. O sistema consiste em módulos **Emissores** (que leem sensores como DHT11 e transmitem via ESP-NOW) e um módulo **Receptor/Central** (que recebe os dados via rádio, exibe no display local OLED, hospeda uma página Web local no LittleFS e fornece uma API JSON em tempo real via Wi-Fi para smartphones/navegadores).

## 1.1 Objetivo

## Chegar a um protótipo tipo maquete com um sistema aplicavel em que poderemos acionar um aquecedor (secador de cabelos no protótipo) via site hospedado em host, causando elevação de temperatura, e forçando o sistema de exaustão (ventilador) trazendo a temperatura de volta ao normal, com acionamento e desligamento automático.
