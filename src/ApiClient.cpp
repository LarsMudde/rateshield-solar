#include <Arduino.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ezTime.h>
#include "ApiClient.h"
#include "Constants.h"
#include "Repository.h"

String getEnergyPricesUrl() {
  Timezone myTZ;
  myTZ.setLocation(F(TIMEZONE));

  // Calculate fromDate and tillDate here
  time_t fromDate_time = myTZ.now();
  time_t tillDate_time = myTZ.now() + 4 * 24 * 60 * 60; // 4 days ahead

  String fromDate = myTZ.dateTime(fromDate_time, "Y-m-d\\TH:i:s.000\\Z");
  String tillDate = myTZ.dateTime(tillDate_time, "Y-m-d\\TH:i:s.999\\Z");

  String url = String(ENERGY_PRICES_BASE_URL) + "?fromDate=" + fromDate + "&tillDate=" + tillDate +
               "&interval=" + String(INTERVAL) + "&usageType=1&inclBtw=" + String(INCL_BTW ? "true" : "false");

  return url;
}

void getFromEndpoint() {
  String url = getEnergyPricesUrl();
  HTTPClient http;
  http.begin(url);

  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    String updatedData = addTimestampToJson(payload);
    writeFile(FORECAST_FILE_NAME, updatedData);
  } else {
    Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void readFromEEPROM(AsyncWebServerRequest* request) {
  String data = readFile(FORECAST_FILE_NAME);
  if (!data.isEmpty()) {
    request->send(200, "text/plain", data);
  } else {
    request->send(200, "text/plain", "Failed to read file");
  }
}

void fetchForecastIfNeeded() {
  if (shouldFetchForecast()) {
    Serial.println("Fetching new data.");
    getFromEndpoint();
  }
}

String readFile(const char *filename) {
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return "";
  }
  String data = file.readString();
  file.close();
  return data;
}
