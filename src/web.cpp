#include "web.h"
#include <FS.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "spiffs.h"
#include "Arduino/DNSServer.h"

#define DEBUG

#define KEY_WIFI_SSID "wifiSSID"
#define KEY_WIFI_PW "wifiPw"

TempestServer::TempestServer(int port, IPAddress ap, IPAddress netmask){
    this->server = new ESP8266WebServer(port);
    this->dnsServer = new DNSServer();

    this->ap = ap;
    this->netmask = netmask;
}

bool TempestServer::init(){
    initWeb();
    initDNS();
    initAP();    
}

boolean TempestServer::captivePortal() {
  if (this->server->hostHeader() != String("192.168.4.1") && this->server->hostHeader() != (String("tempest")+".com")) {
    Serial.println("Request redirected to captive portal");
    this->server->sendHeader("Location", String("/"), true);
    this->server->send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    this->server->client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

bool TempestServer::initWeb(){
    auto handleRoot =[this](){
        if (captivePortal()) { // If captive portal redirect instead of displaying the page.
            return;
        }
        this->server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        this->server->sendHeader("Pragma", "no-cache");
        this->server->sendHeader("Expires", "-1");
        this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
        handleFileRead(this->server->uri());
        this->server->client().stop(); // Stop is needed because we sent no content length
        
    };

    this->server->onNotFound(handleRoot);

    this->server->begin();
}

bool TempestServer::initDNS(){
    this->dnsServer->setErrorReplyCode(DNSReplyCode::Refused);
    //this->dnsServer->start(53, "*", ap);
    IPAddress invalidAddress = IPAddress(1, 1, 33, 42);
    this->dnsServer->addEntry(0, "tempest.com", IPAddress(192, 168, 4, 1));
    this->dnsServer->addEntry(1, "*", invalidAddress);
    this->dnsServer->start(53);
}

bool TempestServer::initAP(){
    JsonObject& settings = getSettings();

    WiFi.forceSleepWake();
    WiFi.mode(WiFiMode::WIFI_AP);

    Serial.println("Starting AP");
    WiFi.softAPConfig(ap,ap,netmask);

    char ssid[20];
    char pw[20];
    
    settings[KEY_WIFI_SSID].printTo(ssid);
    settings[KEY_WIFI_PW].printTo(pw);

    Serial.printf("SS: %s\n", ssid);
    Serial.printf("PW: %s\n", pw);

    boolean result = WiFi.softAP(ssid ,pw );

    if (result == true){
        Serial.println("AP Ready");
    } else {
        Serial.println("AP Error!");
    }
}

void TempestServer::disable(){
    WiFi.mode(WiFiMode::WIFI_OFF);
    WiFi.disconnect(true);
    WiFi.forceSleepBegin();
}

void TempestServer::handle(){
    this->dnsServer->processNextRequest();
    this->server->handleClient();
}

String TempestServer::getContentType(String filename){
    if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    return "text/plain";
}

bool TempestServer::handleFileRead(String path) { // send the right file to the client (if it exists)
    Serial.println("handleFileRead: " + path);
    if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
    String contentType = getContentType(path);            // Get the MIME type
    if (SPIFFS.exists(path)) {                            // If the file exists
        File file = SPIFFS.open(path, "r");                 // Open it
        size_t sent = this->server->streamFile(file, contentType); // And send it to the client
        file.close();                                       // Then close the file again
        return true;
    }
    Serial.println("\tFile Not Found");
    return false;                                         // If the file doesn't exist, return false
}