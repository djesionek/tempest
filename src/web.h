#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "spiffs.h"
#include "Arduino/DNSServer.h"

#ifndef WEB_H
#define WEB_H

class TempestServer{
public:
    TempestServer(int, IPAddress, IPAddress);
    bool init();
    void handle();
    void disable();
private:

    ESP8266WebServer *server;
    DNSServer *dnsServer;

    int port;
    IPAddress ap, netmask;

    bool handleFileRead(String);
    String getContentType(String);

    boolean captivePortal();

    bool initWeb();
    bool initDNS();
    bool initAP();

};

#endif