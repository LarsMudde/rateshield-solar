#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "WebServer.h"
#include "ApiClient.h"

void setupWebServer(AsyncWebServer* server) {
  // Serve the index.html file when the root path ("/") is requested
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Serve the styles.css file when requested
  server->serveStatic("/styles.css", SPIFFS, "/styles.css");
  
  server->on("/getFromEndpoint", HTTP_GET, [](AsyncWebServerRequest* request) {
    getFromEndpoint();
    request->send(200, "text/plain", "Endpoint data fetched and stored in EEPROM");
  });

  server->on("/readFromEEPROM", HTTP_GET, [](AsyncWebServerRequest* request) {
    readFromEEPROM(request);
  });

  server->begin();
}
