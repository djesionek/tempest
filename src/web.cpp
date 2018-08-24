#include "web.h"
#include <FS.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "spiffs.h"
#include <ESP8266mDNS.h>
#include <DNSServer.h>

#define DEBUG

#define KEY_WIFI_SSID "wifiSSID"
#define KEY_WIFI_PW "wifiPw"

ESP8266WebServer server(80);
DNSServer dnsServer;

IPAddress apIP(192, 168, 4, 1);
IPAddress netmask(255, 255, 255, 0);

bool webInit(){

    auto handleRoot =[](){
        handleFileRead("/index.html");
    };

    server.on("/",handleRoot);
    server.on("/generate_204", handleRoot);  //Android captive portal
    server.on("/fwlink", handleRoot); //Microsoft captive portal

    server.onNotFound([]() {                              // If the client requests any URI
        if (!handleFileRead(server.uri()))                  // send it if it exists
            server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });
    
    server.begin();
}

void serverHandleClient(){
    dnsServer.processNextRequest();
    server.handleClient();
}

String getContentType(String filename){
    if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
    Serial.println("handleFileRead: " + path);
    if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
    String contentType = getContentType(path);            // Get the MIME type
    if (SPIFFS.exists(path)) {                            // If the file exists
        File file = SPIFFS.open(path, "r");                 // Open it
        size_t sent = server.streamFile(file, contentType); // And send it to the client
        file.close();                                       // Then close the file again
        return true;
    }
    Serial.println("\tFile Not Found");
    return false;                                         // If the file doesn't exist, return false
}

void wifiInit(){
    // this opens the file "f.txt" in read-mode
    
    JsonObject& settings = getSettings();

    Serial.println("Starting AP");
    WiFi.softAPConfig(apIP,apIP,netmask);
    boolean result = WiFi.softAP(settings[KEY_WIFI_SSID],settings[KEY_WIFI_PW]);

    if (result == true){
        Serial.println("AP Ready");
    } else {
        Serial.println("AP Error!");
    }

    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", apIP);

    /*if (!MDNS.begin(myHostname)) {
          Serial.println("Error setting up MDNS responder!");
        } else {
          Serial.println("mDNS responder started");
          // Add service to MDNS-SD
          MDNS.addService("http", "tcp", 80);
    }*/
}