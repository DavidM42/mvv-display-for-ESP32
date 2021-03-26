#ifndef TYPING_H // To make sure you don't declare the function more than once by including the header multiple times.
#define TYPING_H

#include <Arduino.h>

#define MAX_INCLUDE_TYPE 10
#define MAX_INCLUDE_LINE 10
#define MAX_EXCLUDE_DESTINATION 10

enum api_types
{
    mvg_api,
    geops_api,
    db_transport_rest
};

struct Config
{
    const char* wifi_name;
    const char* wifi_pass;
    enum api_types type;
    const char* bahnhof;
    const char* include_type[MAX_INCLUDE_TYPE];
    const char* include_line[MAX_INCLUDE_LINE];
    const char* exclude_destinations[MAX_EXCLUDE_DESTINATION];
};

typedef struct
{
    unsigned long long aimed_time;
    unsigned long long estimated_time;
    String line;
    String destination;
    String platform;
    int wagon;
} Departure;

// thanks https://stackoverflow.com/a/15891800 for explaining how to modularize
#endif
