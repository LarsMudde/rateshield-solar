#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "WebServer.h"
#include <ArduinoJson.h> // Make sure to add ArduinoJson to your lib_deps in platform.ini

void saveConfig(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  if (!index) {
    if (!SPIFFS.exists("/config.json")) {
      File file = SPIFFS.open("/config.json", "w");
      if (file) {
        file.write(data, len);
        file.close();
      }
    } else {
      SPIFFS.remove("/config.json");
      File file = SPIFFS.open("/config.json", "w");
      if (file) {
        file.write(data, len);
        file.close();
      }
    }
  }
  if (index + len == total) {
    request->send(200, "text/plain", "OK");
  } else {
    request->send(500, "text/plain", "Error writing config.json");
  }
}

void setupWebServer(AsyncWebServer* server) {
  // Serve the index.html file when the root path ("/") is requested
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Serve the styles.css file when requested
  server->serveStatic("/styles.css", SPIFFS, "/styles.css");

  // Serve the scripts.js file when requested
  server->serveStatic("/scripts.js", SPIFFS, "/scripts.js");

  // Handle the /save-config route to save config.json
  server->on("/save-config", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, saveConfig);

  // Handle the /read-config route to read config.json
  server->on("/read-config", HTTP_GET, [](AsyncWebServerRequest *request){
    if (SPIFFS.exists("/config.json")) {
      request->send(SPIFFS, "/config.json", "application/json");
    } else {
      request->send(404, "text/plain", "Configuration file not found");
    }
  });

  server->begin();
}
