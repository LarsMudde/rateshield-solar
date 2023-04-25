#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <ezTime.h>
#include "Repository.h"
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
