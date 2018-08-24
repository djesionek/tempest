#include <Arduino.h>
#include <ESP8266WebServer.h>

#ifndef WEB_H
#define WEB_H

//String getContentType(String filename);
bool webInit();
bool handleFileRead(String path);
void wifiInit();
void serverHandleClient();

#endif