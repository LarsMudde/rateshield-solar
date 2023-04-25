#ifndef APICLIENT_H
#define APICLIENT_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void fetchForecastIfNeeded();
void getFromEndpoint();
void readFromEEPROM(AsyncWebServerRequest* request);
String readFile(const char *filename);

#endif // APICLIENT_H
