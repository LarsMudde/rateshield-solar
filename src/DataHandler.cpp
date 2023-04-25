#include <Arduino.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ezTime.h>

#include "DataHandler.h"
#include "Constants.h"

String addTimestampToJson(const String& apiData) {
  // Create a new JSON object to store the fetched data and timestamp
  DynamicJsonDocument doc(2048);

  // Parse the fetched API data
  DynamicJsonDocument apiDataDoc(1024);
  deserializeJson(apiDataDoc, apiData);

  // Add the fetched API data to the new JSON object under the "data" property
  doc["data"] = apiDataDoc.as<JsonObject>();

  // Add the lastFetchDateTime to the JSON
  Timezone myTZ;
  myTZ.setLocation(F(TIMEZONE));
  String formattedDateTime = myTZ.dateTime("Y-m-d H:i:s");
  doc["lastFetchDateTime"] = formattedDateTime;

  // Serialize the JSON with the added timestamp
  String updatedData;
  serializeJson(doc, updatedData);

  return updatedData;
}

void writeFile(const String& fileName, const String& data) {
  File file = SPIFFS.open(fileName, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  // Write the data to the file
  file.print(data);
  file.close();
}

String readFile(const String& fileName) {
  File file = SPIFFS.open(fileName, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return String();
  }
  String data = file.readString();
  file.close();
  return data;
}


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

bool shouldFetchForecast() {
  if (!SPIFFS.exists(FORECAST_FILE_NAME)) {
    Serial.println("forecast.json does not exist.");
    return true;
  }

  String data = readFile(FORECAST_FILE_NAME);
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, data);

  Timezone myTZ;
  myTZ.setLocation(F(TIMEZONE));
  unsigned long currentTime = myTZ.now();

  const char* lastFetchDateTimeStr = doc["lastFetchDateTime"].as<const char*>();
  struct tm lastFetchTm;
  strptime(lastFetchDateTimeStr, "%Y-%m-%d %H:%M:%S", &lastFetchTm);
  time_t lastFetchTimestamp = mktime(&lastFetchTm);

  if (currentTime - lastFetchTimestamp > FORECAST_MAX_AGE) {
    Serial.println("Data is more than 'FORECAST_MAX_AGE' hour(s) old.");
    return true;
  }

  Serial.println("Data is up-to-date enough.");
  return false;
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