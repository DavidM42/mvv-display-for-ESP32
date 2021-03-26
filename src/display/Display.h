#ifndef DISPLAY_H // To make sure you don't declare the function more than once by including the header multiple times.
#define DISPLAY_H

// external imports
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "Wire.h"

// font stuff
#include "Free_Fonts.h"
// #include "Free_Sans.h"
#include "Bahnschrift9pt7b.h"

// own typings
#include "../typing/typing.h"

// definitions to be used
#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN 0x10
#endif

#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4 // Display backlight control pin

extern TFT_eSPI tft;
extern TFT_eSprite img;

// finally class to expose
class Display
{
private:
    // TFT_eSPI tft;
    // TFT_eSprite img;


public:
    // Display();

    void setup_display();

    /* Show Error message in middle of screen */
    void showError(String message);

    /* Fill the whole sprite with defined colour */
    void fillSprite(int color);
    /* Push the sprite to the TFT screen */
    void pushSprite(int32_t x, int32_t y);
    /* pushSprite but all via x and y = 0 */
    void pushAll();
    /* Draws top line (basically column names) */
    void drawTopLine();
    /* Draws a departure */
    void drawDeparture(int display_line, String line, String destination, String track, int wagon, int minutes);


    /* Draws top line for deutsche bahn api with different spacings */
    void drawDBTopLine();
    /* Draws a departure for deutsche bahn api with different spacing*/
    void drawDBDeparture(int display_line, String line, String destination, String track, int minutes);
};

#endif
