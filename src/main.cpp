//Quellen:
// https://github.com/Bodmer/TFT_eSPI/blob/master/examples/320%20x%20240/Free_Font_Demo/Free_Font_Demo.ino
// https://github.com/Xinyuan-LilyGO/TTGO-T-Display
#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
using namespace std;
#include <Button2.h>

// TODO testing mdns and web api
#include <ESPmDNS.h>
// TODO overblows flash size
// #include <ESPAsyncWebServer.h>
// AsyncWebServer server(80);

// first of all typings
#include <typing/typing.h>

// get config
#include "./config.h" // old version with header
const int number_of_configs = sizeof(configs) / sizeof(*configs);
// #include "configurationManager/ConfigurationManager.h"
// ConfigurationManager configManager;
// int number_of_configs;

Config loaded_config;

// then modules
#include <helper/helper.h>
#include <apis/apis.h>
#include <display/Display.h>

#define ADC_EN 14
#define ADC_PIN 34
#define BUTTON_1 35
#define BUTTON_2 0

// own modules creation
Display display;
Apis apis(display);

// Button2 btn1(BUTTON_1);
// Button2 btn2(BUTTON_2);

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
int config_number = -1;

/* re activate and fix after display class done
void button_init()
{
  btn1.setPressedHandler([](Button2 &b) {
    Serial.println("btn2 pressed");
    int r = digitalRead(TFT_BL);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
 btn1.setPressedHandler([](Button2 &b) {
    Serial.println("btn2 pressed");
    int r = digitalRead(TFT_BL);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Shutting down...", tft.width() / 2, tft.height() / 2);
    delay(3000);
    digitalWrite(TFT_BL, !r);

    tft.writecommand(TFT_DISPOFF);
    tft.writecommand(TFT_SLPIN);
    esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
    esp_deep_sleep_start();
    delay(100);
  });

  btn2.setPressedHandler([](Button2 &b) {
    Serial.println("btn2 pressed");
  });
}

void button_loop()
{
  btn1.loop();
  btn2.loop();
}
*/

void setup()
{
  Serial.begin(115200);
  Serial.println("Started");

  // TODO fix
  // configManager.init();
  // number_of_configs = sizeof(configManager.configs) / sizeof(*configManager.configs);

  // TODO re activate later
  // button_init();
  Serial.println("Button setup done");

  // Serial.println(number_of_configs);
  // Serial.println(configManager.configs[0].wifi_name);
  // Serial.println(configManager.configs[0].wifi_pass);

  config_number = connect_wifi(configs, number_of_configs);
  loaded_config = configs[config_number];
  // config_number = connect_wifi(configManager.configs, number_of_configs);
  // loaded_config = configManager.configs[config_number];

  display.setup_display();

  switch (loaded_config.type)
  {
  case mvg_api:
    break;
  case geops_api:
    apis.init_geops_api(loaded_config);
    break;
  case db_transport_rest:
    break;
  default:
    Serial.println("Unknown config type");
    break;
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // TODO testing mdns
  if (!MDNS.begin("abfahrt"))
  {
    Serial.println("Error starting mDNS");
    return;
  }
  MDNS.addService("http", "tcp", 80);
  // server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   request->send(200, "text/plain", "Hello World");
  // });

  // server.begin();
  /*******************************/
}

void loop()
{
  // TODO reactivate later
  // button_loop();
  if (WiFi.status() == WL_CONNECTED)
  {
    switch (loaded_config.type)
    {
    case mvg_api:
      apis.handle_mvg_api(loaded_config);
      break;
    case geops_api:
      static unsigned long last_time = 0;
      apis.handle_geops_api(loaded_config);
      if (millis() > last_time + 10000)
      {
        last_time = millis();
        apis.ping_geops_api(loaded_config);
      }
      break;
    case db_transport_rest:
      apis.handle_db_transport_rest(loaded_config);
      break;
    default:
      Serial.println("Unkown config type");
    }
  }
  else
  {
    Serial.println("Error in WiFi connection");
    Serial.println("Try to Reconnect WiFi");
    WiFi.disconnect();
    // WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);
    // config_number = connect_wifi(configManager.configs, number_of_configs);
    // loaded_config = configManager.configs[config_number];
    config_number = connect_wifi(configs, number_of_configs);
    loaded_config = configs[config_number];
  }
}