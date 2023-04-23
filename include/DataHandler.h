#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <ESPAsyncWebServer.h>

void getFromEndpoint();
void readFromEEPROM(AsyncWebServerRequest* request);

#endif // DATAHANDLER_H
