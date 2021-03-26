#ifndef HELPER_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define HELPER_H

#include <Arduino.h>
#include <WiFi.h>

#include "../typing/typing.h"

int connect_wifi(Config configs[], int number_of_configs);
void espDelay();
time_t convertToTime(String calTimestamp);
String getSplitPosition(String data, char separator, int index);

// thanks https://stackoverflow.com/a/15891800 for explaining how to modularize
#endif
