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
#include "DataHandler.h"

AsyncWebServer server(80);
AsyncDNSServer dnsServer;

ESPAsync_WiFiManager* wifiManager;

const unsigned long MAX_FILE_AGE = 8 * 60 * 60 * 1000UL;  // 8 hours in milliseconds

bool shouldFetchForecast() {
  File file = SPIFFS.open(FORECAST_FILE_NAME, "r");
  if (!file) {
    // File doesn't exist
    Serial.println("forecast.json doesn't exist");
    return true;
  }
  unsigned long lastModified = file.getLastWrite();
  file.close();
  unsigned long age = millis() - lastModified;
  if (age >= MAX_FILE_AGE) {
    // File is too old
    Serial.printf("forecast.json is %lu ms old, fetching new data\n", age);
    return true;
  }
  // File is up to date
  Serial.printf("forecast.json is %lu ms old, not fetching new data\n", age);
  return false;
}

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
  myTZ.setLocation(F(TIMEZONE));
  Serial.println("Local time: " + myTZ.dateTime());
}

void setupWebServer() {
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

void getForecast() {
  Serial.println("Fetching new forecast...");
  getFromEndpoint();
}

void syncTimeEveryWeek() {
  static unsigned long lastSyncTime = 0;
  const unsigned long oneWeek = 7 * 24 * 60 * 60 * 1000UL;
  
  if (millis() - lastSyncTime >= oneWeek) {
    waitForSync();
    lastSyncTime = millis();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting");

  setupFileSystem();
  setupWiFi();

  if (shouldFetchForecast()) {
    getForecast();
  }

  setupWebServer();
}

void loop() {
  syncTimeEveryWeek();
}
