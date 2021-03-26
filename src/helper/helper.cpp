#include "helper.h"

int connect_wifi(Config configs[], int number_of_configs)
{
    Serial.print("Number of configs: ");
    Serial.println(number_of_configs);

    Serial.println("Wifi setup entered");
    int number_of_networks = WiFi.scanNetworks();
    if (number_of_networks == -1)
    {
        Serial.println("No networks available");
    }
    Serial.println("Scanning done"); // TODO remove
    int wait = 0;
    int wifi_retry = 0;

    for (int j = 0; j < number_of_configs; j++)
    {
        Serial.println(j);
        Serial.println("Current Wifi configuration:");
        Serial.println(configs[j].wifi_name);
        Serial.println(configs[j].wifi_pass);

        WiFi.begin(configs[j].wifi_name, configs[j].wifi_pass);
        while (WiFi.status() != WL_CONNECTED && wait < 5)
        {
            delay(1000);
            Serial.println("Retrying wifi...");
            ++wait;
        }
        while (WiFi.status() != WL_CONNECTED && wifi_retry < 5)
        {
            ++wifi_retry;
            Serial.println("WiFi not connected. Try to reconnect");
            WiFi.disconnect();
            WiFi.mode(WIFI_OFF);
            WiFi.mode(WIFI_STA);
            WiFi.begin(configs[j].wifi_name, configs[j].wifi_pass);
            Serial.println("Connecting to WiFi...");
            delay(5000);
        }
        if (wifi_retry >= 5)
        {
            Serial.println("Wifi setup failed :(  Check your configs password and usernames");
            Serial.println("Restart in 2s");
            delay(2000);
            ESP.restart();
        }
        else
        {
            char wifiname[256];
            strncpy(wifiname, "Connected to WiFi network: ", sizeof(wifiname));
            strncat(wifiname, configs[j].wifi_name, sizeof(wifiname));
            Serial.printf(wifiname);
            return j;
        }
    }

    return -1;
}

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

// from https://arduino.stackexchange.com/a/55258
// but modified since I have different format here
time_t convertToTime(String calTimestamp)
{
    // I have like 2021-03-26T00:33:00+01:00
    // while originally was like 20180810T143000Z

    struct tm tm;
    Serial.println("Parsing " + calTimestamp);
    String year = calTimestamp.substring(0, 4);
    String month = calTimestamp.substring(5, 7);
    if (month.startsWith("0"))
    {
        month = month.substring(1);
    }
    String day = calTimestamp.substring(8, 10);
    if (day.startsWith("0"))
    {
        month = day.substring(1);
    }

    /* All hours and timezone changes here */
    int hours = calTimestamp.substring(11, 13).toInt();
    int minutes = calTimestamp.substring(14, 17).toInt();

    int tzHourShift = calTimestamp.substring(20,22).toInt();
    int tzMinShift = calTimestamp.substring(23,25).toInt();

    if (calTimestamp.substring(19,20) == "+") {
        hours += tzHourShift;
        minutes += tzMinShift;
    } else if (calTimestamp.substring(19,20) == "-") {
        hours -= tzHourShift;
        minutes -= tzMinShift;
    }
    /***********************************/

    tm.tm_year = year.toInt() - 1900;
    tm.tm_mon = month.toInt() - 1;
    tm.tm_mday = day.toInt();
    tm.tm_hour = hours;
    tm.tm_min = minutes;
    tm.tm_sec = calTimestamp.substring(17, 19).toInt();
    Serial.println(tm.tm_year);
    Serial.println(tm.tm_mon);
    Serial.println(tm.tm_mday);
    Serial.println(tm.tm_hour);
    Serial.println(tm.tm_min);
    Serial.println(tm.tm_sec);
    return mktime(&tm);
}

// from https://stackoverflow.com/a/29673158
/*
    Split string via seperator and get specific index
*/
String getSplitPosition(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}