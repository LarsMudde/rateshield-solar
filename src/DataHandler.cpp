#include <Arduino.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "DataHandler.h"
#include "Constants.h"

void writeToFile(const String& data) {
  File file = SPIFFS.open(FORECAST_FILE_NAME, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  file.print(data);
  file.close();
}

void getFromEndpoint() {
  HTTPClient http;
  http.begin("https://jsonplaceholder.typicode.com/todos/1");

  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    writeToFile(payload);
  } else {
    Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void readFromEEPROM(AsyncWebServerRequest* request) {
  File file = SPIFFS.open(FORECAST_FILE_NAME, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    request->send(200, "text/plain", "Failed to read file");
    return;
  }
  String data = file.readString();
  file.close();
  request->send(200, "text/plain", data);
}
