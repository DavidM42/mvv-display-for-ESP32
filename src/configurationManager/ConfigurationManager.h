#ifndef CONFIGURATION_MANAGER_H // To make sure you don't declare the function more than once by including the header multiple times.
#define CONFIGURATION_MANAGER_H

// external imports
#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"

// flags needed in apis
#define ARDUINOJSON_USE_LONG_LONG 1
#define ARDUINOJSON_USE_DOUBLE 1
#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>

#include "../typing/typing.h"

// Change the capacity to match your requirements.
// Use arduinojson.org/assistant to compute the capacity.
const int JSON_DOC_SIZE = 2048;

class ConfigurationManager
{
private:
    /* loads in config json file */
    void loadConfigurations();

public:
    /* Array of configuration objects */
    Config configs[];

    void init();

    /* Saves the configuration to a file */
    void saveConfiguration();
};

#endif
