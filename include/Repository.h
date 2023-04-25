#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <Arduino.h>

String addTimestampToJson(const String& apiData);
void writeFile(const String& fileName, const String& data);
String readFile(const String& fileName);
bool shouldFetchForecast();

#endif // REPOSITORY_H
