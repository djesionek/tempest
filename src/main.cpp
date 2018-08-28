#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#include "FS.h"

#include "spiffs.h"
#include "web.h"

#define DEBUG

TempestServer server(80, IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));

void modeSwitchInterrupt(){
    Serial.printf("Interrupt. Pin 12 Value: %i\n",digitalRead(12));

    if (digitalRead(12) == 0){
        Serial.printf("Entering sleep mode\n");
        server.disable();
    } else {
        Serial.printf("Leaving sleep mode\n");
        server.init();
    }
}

void setup() {
    Serial.begin(115200);
    while(!Serial);

    for (int i = 0; i < 5; i++){
        delay(1000);
        Serial.printf("Waiting: %d\n", i);
        WDT_FEED();
    }


    pinMode(12, INPUT_PULLUP);
    attachInterrupt(12, modeSwitchInterrupt, CHANGE);

    // prepare SPIFFS    
    spiffsInit();

    // Init the server suite (http, dns and access point)
    server.init();

}


void loop() {
    if (digitalRead(12) == 1){
        server.handle();
    }
    yield();
}