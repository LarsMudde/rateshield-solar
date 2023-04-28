#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncDNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsync_WiFiManager.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ezTime.h>
#include "WebServer.h"
#include "Constants.h"

AsyncWebServer server(80);
AsyncDNSServer dnsServer;

ESPAsync_WiFiManager* wifiManager;

void setupFileSystem() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }
}

void setupWiFi() {
  wifiManager = new ESPAsync_WiFiManager(&server, &dnsServer);
  wifiManager->autoConnect(WIFI_SSID, WIFI_PASSWORD);
}

void setupTime() {
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());

  Timezone myTZ;
  myTZ.setLocation(F(TIMEZONE)); // Set timezone to Amsterdam
  Serial.println("Local time: " + myTZ.dateTime());
}

void setupWebServer() {
  // Only setup the web server if the Wi-Fi connection is successful
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to WiFi! IP address: ");
    Serial.println(WiFi.localIP());

    setupWebServer(&server);
    Serial.println("HTTP server started");
    setupTime();
  } else {
    Serial.println("Failed to connect to Wi-Fi");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting");

  setupFileSystem();
  setupWiFi();
  setupWebServer();
}

void loop() {
  // The loop function remains empty as the ezTime library handles time synchronization automatically
}
