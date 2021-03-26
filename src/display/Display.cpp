#include "Display.h"

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library
TFT_eSprite img = TFT_eSprite(&tft);


void Display::setup_display()
{
    tft.init();
    tft.setRotation(3);
    //tft.fillScreen(TFT_BLACK);
    tft.fillScreen(0x005);
    if (TFT_BL > 0)
    {                               // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);    // Set backlight pin to output mode
        digitalWrite(TFT_BL, HIGH); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }
    img.createSprite(240, 135);
    img.fillSprite(0x005);
}

void Display::fillSprite(int color)
{
    img.fillSprite(color);
}

void Display::pushSprite(int32_t x, int32_t y)
{
    img.pushSprite(x,y);
}

void Display::pushAll()
{
    img.pushSprite(0,0);
}

void Display::showError(String message) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(message, tft.width() / 2, tft.height() / 2);
}

void Display::drawTopLine()
{
    img.setTextDatum(TL_DATUM);
    img.fillRect(0, 0, 240, 8, TFT_YELLOW);
    img.setTextFont(GLCD);
    img.setTextColor(0x005, TFT_YELLOW);
    img.drawString("Linie", 1, 0);
    img.drawString("Ziel", 33, 0);
    img.drawString("Gleis", 128, 0);
    img.drawString("A", 169, 0);
    img.drawString("B", 184, 0);
    img.drawString("C", 200, 0);
    img.drawString("Min", 220, 0);
}

void Display::drawDeparture(int display_line, String line, String destination, String track, int wagon, int minutes)
{
    char str_buffer[10];
    int line_height = 15;
    int offset = 9;
    int offest_string = 7;
    int y_display = display_line * line_height + offset;
    int y_display_string = display_line * line_height + offest_string;

    //img.setFreeFont(FF17);
    // img.setFreeFont(&FreeSans9pt8b);
    img.setFreeFont(&BAHNSCHRIFT9pt7b);
    img.setTextDatum(TL_DATUM);

    //line
    if (line[0] == 'S') //Sbahn
    {
        uint16_t rect_color = 0x005;
        uint16_t font_color = TFT_WHITE;
        switch (line[1])
        {
        case '1':
            rect_color = 0x05FF;
            break;

        case '2':
            rect_color = 0x05F0;
            break;

        case '3':
            rect_color = TFT_MAGENTA;
            break;

        case '4':
            rect_color = 0xE800;
            break;

        case '6':
            rect_color = 0x03E0;
            break;

        case '7':
            rect_color = 0xB963;
            break;

        case '8':
            rect_color = TFT_BLACK;
            font_color = TFT_YELLOW;
            break;

        default:
            break;
        }
        img.fillRoundRect(0, y_display, 28, 15, 7, rect_color);
        img.setTextColor(font_color);
        img.drawString(line, 3, y_display_string);
    }
    else if (line[0] == 'U') //Ubahn
    {
        uint16_t rect_color = 0x005;
        uint16_t font_color = TFT_WHITE;
        switch (line[1])
        {
        case '1':
            rect_color = 0x03E0;
            break;

        case '2':
            rect_color = 0xE800;
            break;

        case '3':
            rect_color = TFT_ORANGE;
            break;

        case '4':
            rect_color = 0x05F0;
            break;

        case '5':
            rect_color = 0xB963;
            break;

        case '6':
            rect_color = TFT_BLUE;
            break;

        case '7':
            img.fillTriangle(28, y_display, 1, y_display, 1, y_display + 14, 0x03E0);       //Green Half
            img.fillTriangle(1, y_display + 14, 28, y_display, 28, y_display + 14, 0xE800); //Red Half
            break;

        case '8':
            img.fillTriangle(28, y_display, 1, y_display, 1, y_display + 14, 0xE800);           //Red Half
            img.fillTriangle(1, y_display + 14, 28, y_display, 28, y_display + 14, TFT_ORANGE); //Orange Half
            break;

        default:
            break;
        }
        if (!(line[1] == '7' || line[1] == '8')) //line U1-U6
        {
            img.fillRect(1, y_display + 1, 28, 14, rect_color);
        }
        img.setTextColor(font_color);
        img.drawString(line, 3, y_display_string);
    }
    else //default
    {
        img.setTextColor(TFT_WHITE);
        img.drawString(line, 3, y_display_string);
    }

    //destination
    img.setTextColor(TFT_WHITE);
    if (track == "" && wagon == 0)
    {
        img.drawString(destination.substring(0, 20), 32, y_display_string);
    }
    else
    {
        img.drawString(destination.substring(0, 11), 32, y_display_string);
    }

    //track
    if (track != "")
    {
        img.drawString(track, 150, y_display_string);
    }

    //wagon
    if (wagon == 1 || wagon == 2 || wagon == 3)
        img.drawString("_ _ _", 167, y_display_string);
    switch (wagon)
    {
    case 3:
        img.fillRoundRect(195, y_display + 4, 14, 11, 3, TFT_WHITE); //rechts
    case 2:
        img.fillRoundRect(165, y_display + 4, 14, 11, 3, TFT_WHITE); //links
    case 1:
        img.fillRoundRect(180, y_display + 4, 14, 11, 3, TFT_WHITE); //mitte
    default:
        break;
    }

    //minutes
    img.setTextDatum(TR_DATUM);
    sprintf(str_buffer, "%u", minutes);
    img.drawString(str_buffer, 240, y_display_string);
}




/* Modified DB things */

void Display::drawDBTopLine()
{
    img.setTextDatum(TL_DATUM);
    img.fillRect(0, 0, 240, 8, TFT_YELLOW);
    img.setTextFont(GLCD);
    img.setTextColor(0x005, TFT_YELLOW);
    img.drawString("Linie", 1, 0);
    img.drawString("Ziel", 62, 0);
    img.drawString("Gleis", 180, 0);
    // could have platform distribution for ICE but useless for rest so removed
    // now more space for destination
    img.drawString("Min", 218, 0);
}

void Display::drawDBDeparture(int display_line, String line, String destination, String track, int minutes)
{
    char str_buffer[10];
    int line_height = 15;
    int offset = 9;
    int offest_string = 7;
    int y_display = display_line * line_height + offset;
    int y_display_string = display_line * line_height + offest_string;

    //img.setFreeFont(FF17);
    // img.setFreeFont(&FreeSans9pt8b);
    img.setFreeFont(&BAHNSCHRIFT9pt7b);
    img.setTextDatum(TL_DATUM);

    //line types
    String lineType = line;
    lineType.toUpperCase();
    if (lineType[0] == 'S' && lineType[1] != 'T') //S-Bahn    not STR Bahn
    {
        unsigned long rect_color = 0x005e29;
        unsigned long font_color = TFT_WHITE;
        img.fillRoundRect(0, y_display, 28, 15, 7, rect_color);
        img.setTextColor(font_color);
        img.drawString(line, 3, y_display_string);
    } 
    else if(lineType.substring(0,3) == "STR") {  // STR Bahn
        unsigned long font_color = TFT_BLACK;

        img.fillTriangle(60, y_display, 1, y_display, 1, y_display + 14, 0xE800);           //Red Half
        img.fillTriangle(1, y_display + 14, 60, y_display, 60, y_display + 14, TFT_ORANGE); //Orange Half

        // img.fillRoundRect(0, y_display, 28, 15, 7, rect_color);
        img.setTextColor(font_color);
        img.drawString(line, 3, y_display_string);       
    }
    else if (lineType[0] == 'U') //U-Bahn
    {
        unsigned long rect_color = 0x661a6;
        unsigned long font_color = TFT_WHITE;

        img.fillRect(1, y_display + 1, 28, 14, rect_color);
        img.setTextColor(font_color);
        img.drawString(line, 3, y_display_string);
    }
    else if(lineType.substring(0,3) == "ICE") // ICE
    {
        unsigned long ice_font_color = TFT_DARKGREY;
        unsigned long ice_bg_red = TFT_RED;
        img.fillRect(1, y_display + 1, 60, 14, ice_bg_red);
        img.setTextColor(ice_font_color);
        img.drawString(line, 3, y_display_string);
    }
    else if(lineType.substring(0,2) == "RE" || lineType.substring(0,2) == "RB") // Regio Trains
    {
        unsigned long regio_bg_red = 0xcc061d;
        img.fillRect(1, y_display + 1, 28, 14, regio_bg_red);
        img.setTextColor(TFT_WHITE);
        img.drawString(line, 3, y_display_string);    
    }
    else if(lineType.substring(0,3) == "BUS") // le Bus
    {
        unsigned long bus_bg_turquoise = 0x139faa;
        img.fillRect(1, y_display + 1, 60, 14, bus_bg_turquoise);
        img.setTextColor(TFT_BLACK);
        img.drawString(line, 3, y_display_string);    
    }
    else if(lineType.substring(0,3) == "AST" || lineType.substring(0,4) == "TAXI") // Taxi
    {
        unsigned long taxi_yellow_font = 0xf7be04;
        img.fillRect(1, y_display + 1, 28, 14, TFT_BLACK);
        img.setTextColor(taxi_yellow_font);
        img.drawString(line, 3, y_display_string);    
    }
    else //default for other modes of transport
    {
        img.setTextColor(TFT_WHITE);
        img.drawString(line, 3, y_display_string);
    }

    //destination
    img.setTextColor(TFT_WHITE);
    if (track == "" || track == "null")
    {
        img.drawString(destination.substring(0, 16), 62, y_display_string);
    }
    else
    {
        img.drawString(destination.substring(0, 12), 62, y_display_string);
    }

    //track
    if (track != "" && track != "null")
    {
        img.drawString(track, 180, y_display_string);
    }

    //minutes
    img.setTextDatum(TR_DATUM);
    sprintf(str_buffer, "%u", minutes);
    img.drawString(str_buffer, 234, y_display_string);
}