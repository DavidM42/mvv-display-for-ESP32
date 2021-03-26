#include "ConfigurationManager.h"

const char *filename = "/config.json";

void ConfigurationManager::init()
{
    while (!SPIFFS.begin())
    {
        Serial.println(F("Failed to initialize SPIFFS library"));
        delay(1000);
    }

    // Should load default config if run for the first time
    Serial.println(F("Loading configuration..."));
    loadConfigurations();
}

void ConfigurationManager::loadConfigurations()
{
    // Open file for reading
    File file = SPIFFS.open(filename);

    // Allocate a temporary JsonDocument
    // TODO maybe increase calc via https://arduinojson.org/v6/assistant/
    StaticJsonDocument<JSON_DOC_SIZE> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
        Serial.println(F("Failed to read file"));
    Serial.println(error.c_str());
    // return;

    // Copy values from the JsonDocument to the configs
    const int number_of_configs = doc.size();
    Serial.print("Number of configs: ");
    Serial.println(number_of_configs);

    for (int i = 0; i < number_of_configs; i++)
    {
        // TODO fix validation
        // if (
        //     !strcmp(doc[i]["type"], "mvg_api") &&
        //     !strcmp(doc[i]["type"], "geops_api"))
        // {
        //     Serial.print("Unknown api type: ");
        //     serializeJsonPretty(doc[i]["type"], Serial);
        //     return;
        // }
        // else
        // {
        // }
        configs[i].type = doc[i]["type"];

        // Serial.println(configs[i].type);

        // if (configs[i].type == mvg_api) {
        //     Serial.println("mvg api");
        // } else if(configs[i].type != mvg_api && configs[i].type != geops_api) {
        //     Serial.println("unsupported api type");
        // }
        // if (doc[i]["type"] == "mgv_api") {
        //     Serial.println("Type mvg");
        //     configs[i].type = mvg_api;
        //     Serial.println("Type mvg set");
        // } else if (doc[i]["type"] == "geops_api") {
        //     configs[i].type = geops_api;
        // } else {
        //     Serial.print("Unknown api type: ");
        //     serializeJsonPretty(doc[i]["type"], Serial);
        //     return;
        // }

        // strcpy(configs[i].wifi_name, doc[i]["wifi_name"]);
        configs[i].wifi_name = doc[i]["wifi_name"];
        configs[i].wifi_pass = doc[i]["wifi_pass"];
        configs[i].bahnhof = doc[i]["bahnhof"];

        for (int j = 0; j < doc[i]["include_type"].size(); j++)
        {
            configs[i].include_type[j] = doc[i]["include_type"][j];
        }

        for (int j = 0; j < doc[i]["include_line"].size(); j++)
        {
            configs[i].include_line[j] = doc[i]["include_line"][j];
        }

        for (int j = 0; j < doc[i]["exclude_destinations"].size(); j++)
        {
            configs[i].exclude_destinations[j] = doc[i]["exclude_destinations"][j];
        }
    }

    file.close();
}

void ConfigurationManager::saveConfiguration()
{
    // TODO fix this not destroying the values
    return;

    // Delete existing file, otherwise the configuration is appended to the file
    SPIFFS.remove(filename);

    // Open file for writing
    File file = SPIFFS.open(filename, FILE_WRITE);
    if (!file)
    {
        Serial.println(F("Failed to create file"));
        return;
    }

    // Allocate a temporary JsonDocument
    StaticJsonDocument<JSON_DOC_SIZE> doc;

    // Set the values in the document
    const int number_of_configs = (sizeof(configs) / sizeof(*configs));
    Serial.print("Number of configs: ");
    Serial.println(number_of_configs);

    for (int i = 0; i < number_of_configs; i++)
    {
        switch (configs[i].type)
        {
        case mvg_api:
            doc[i]["type"] = "mvg_api";
            break;
        case geops_api:
            doc[i]["type"] = "geops_api";
            break;
        case db_transport_rest:
            doc[i]["type"] = "db_transport_rest";
        default:
            doc[i]["type"] = NULL;
        }

        doc[i]["wifi_name"] = configs[i].wifi_name;
        doc[i]["wifi_pass"] = configs[i].wifi_pass;
        doc[i]["bahnhof"] = configs[i].bahnhof;

        const int type_length = (sizeof(configs[i].include_type) / sizeof(*configs[i].include_type));
        for (int j = 0; j < type_length; j++)
        {
            doc[i]["include_type"][j] = configs[i].include_type[j];
        }

        const int line_length = (sizeof(configs[i].include_line) / sizeof(*configs[i].include_line));
        for (int j = 0; j < line_length; j++)
        {
            doc[i]["include_line"][j] = configs[i].include_line[j];
        }

        const int exl_dest_length = (sizeof(configs[i].exclude_destinations) / sizeof(*configs[i].exclude_destinations));
        for (int j = 0; j < exl_dest_length; j++)
        {
            doc[i]["exclude_destinations"][j] = configs[i].exclude_destinations[j];
        }
    }

    // Serialize JSON to file
    if (serializeJson(doc, file) == 0)
    {
        Serial.println(F("Failed to write to file"));
    }
    else
    {
        Serial.println("Wrote config to FS");
    }

    // Close the file
    file.close();
}