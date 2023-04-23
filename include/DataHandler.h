#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <ESPAsyncWebServer.h>

void fetchForecastIfNeeded();
void getFromEndpoint();
void readFromEEPROM(AsyncWebServerRequest* request);
bool shouldFetchForecast();
String readFile(const char *filename);

#endif
