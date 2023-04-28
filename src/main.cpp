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

unsigned long lastInteractionTime = 0;

void executeTask() {
  // Your task implementation here
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

bool shouldSleep() {
  unsigned long currentMillis = millis();
  unsigned long elapsedTime = currentMillis - lastInteractionTime;
  return elapsedTime >= IDLE_SLEEP_INTERVAL * 1000;
}

void goToSleep(int sleepTimeSeconds) {
  esp_sleep_enable_timer_wakeup(sleepTimeSeconds * 1000000LL);
  esp_deep_sleep_start();
}

void handleSleepAndTaskExecution() {
  Timezone myTZ;
  myTZ.setLocation(F(TIMEZONE));

  int currentHour = myTZ.hour();

  if (shouldSleep() && currentHour >= START_HOUR && currentHour <= END_HOUR) {
    executeTask();
    
    int secondsToNextHour = 3600 - (myTZ.minute() * 60) - myTZ.second();
    goToSleep(secondsToNextHour);
  }
}

void loop() {
  handleSleepAndTaskExecution();
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting");

  setupFileSystem();
  setupWiFi();
  setupWebServer();

  pinMode(GPIO_NUM_39, INPUT_PULLUP);
  attachInterrupt(GPIO_NUM_39, [] {
    lastInteractionTime = millis();
  }, RISING);
}
