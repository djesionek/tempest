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


/*
Small Webserver Tutorial
https://tttapa.github.io/ESP8266/Chap11%20-%20SPIFFS.html
Web and JSON
https://diyprojects.io/esp8266-web-server-part-4-arduinojson-load-save-files-spiffs/
*/



void setup() {
    Serial.begin(115200);
    while(!Serial);

    for (int i = 0; i < 5; i++){
        delay(1000);
        Serial.printf("Waiting: %d\n", i);
        WDT_FEED();
    }

    // prepare SPIFFS    
    spiffsInit();

    // prepare webserver
    webInit();

    // Open AP
    wifiInit();
}

void loop() {
    // put your main code here, to run repeatedly:
    serverHandleClient();
}


/*void openf(){
    // this opens the file "f.txt" in read-mode
    File f = SPIFFS.open("/f.txt", "r");

    if (!f) {
        Serial.println("File doesn't exist yet. Creating it");

        // open the file in write mode
        File f = SPIFFS.open("/f.txt", "w");
        if (!f) {
            Serial.println("file creation failed");
        }
        // now write two lines in key/value style with  end-of-line characters
        f.println("ssid=abc");
        f.println("password=123455secret");
    } else {
        // we could open the file
        while(f.available()) {
            //Lets read line by line from the file
            String line = f.readStringUntil('\n');
            Serial.println(line);
        }

    }
    f.close();
}*/