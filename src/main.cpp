#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncDNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsync_WiFiManager.h>

AsyncWebServer server(80);
AsyncDNSServer dnsServer;

ESPAsync_WiFiManager* wifiManager;

void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting");

  wifiManager = new ESPAsync_WiFiManager(&server, &dnsServer);
  wifiManager->autoConnect("RateShield Solar", "");

  Serial.print("Connected to WiFi! IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><body>";
    html += "<h1>RateShield Solar Configuration</h1>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
    wifiManager->startConfigPortal();
    request->send(200, "text/plain", "Started WiFi configuration portal");
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
}