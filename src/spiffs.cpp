#include <ArduinoJson.h>

#include "spiffs.h"
#include "FS.h"

#define DEBUG

StaticJsonBuffer<1000> jsonBuffer;

String realSize = String(ESP.getFlashChipRealSize());
String ideSize = String(ESP.getFlashChipSize());
bool flashCorrectlyConfigured = realSize.equals(ideSize);

void spiffsInit(){
    bool result = false;

    if(flashCorrectlyConfigured) result = SPIFFS.begin();
    else Serial.println("flash incorrectly configured, SPIFFS cannot start, IDE size: " + ideSize + ", real size: " + realSize);
    
    Serial.printf("SPIFFS opened: %s\n", result ? "true" : "false");
}

JsonObject & getSettings(){
    File f = SPIFFS.open("/settings.json", "r");

    if (!f) {
        Serial.println("Settings not found!");
    } else {

        size_t size = f.size();
        if ( size == 0 ) {
            Serial.println("Settings file empty!");
        } else {
            std::unique_ptr<char[]> buf (new char[size]);
            f.readBytes(buf.get(), size);
            JsonObject& root = jsonBuffer.parseObject(buf.get());
            if (!root.success()) {
                Serial.println("Failed to read settings file.");
            } else {
                Serial.println("Loaded Settings!");

#ifdef DEBUG
                root.printTo(Serial);
                Serial.println();
#endif

                return root;
            }
        }
        f.close();

    }
}