#ifndef APIS_H    // To make sure you don't declare the function more than once by including the header multiple times.
#define APIS_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoWebsockets.h>
#include <list>
#include <iterator>
using namespace std;


// mvg api timestamp in ms needs long long
#define ARDUINOJSON_USE_LONG_LONG 1
// geops api timestamp in ms needs double
#define ARDUINOJSON_USE_DOUBLE 1
// geops api uses unicode characters
#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>

#include "../typing/typing.h"
#include "../helper/helper.h"
#include "../display/Display.h"

#define MAX_JSON_DOCUMENT 20000
#define MAX_DEPARTURE_LIST_LENGTH 100

class Apis
{
private:
    StaticJsonDocument<MAX_JSON_DOCUMENT> doc;
    websockets::WebsocketsClient client;

    list<Departure> departure_list;
    Config configs[];
    int loaded_config_i;

    Display display;

public:
    Apis(Display displayIn);

    /* Change loaded config */
    void update_loaded_config(Config config);

    /* Handle db transport rest api see https://v5.db.transport.rest */
    void handle_db_transport_rest(Config config);

    /* Handler for Muenchner Verkehrs Gruppe Api */
    void handle_mvg_api(Config config);

    /* Initialisation of geops api */
    void init_geops_api(Config config);

    /* Handler for geops api */
    void handle_geops_api(Config config);
    
    /* Repeatadly ping geops api as is websockets connection */
    void ping_geops_api(Config config);
};

#endif
