#pragma once
#include <Arduino.h>
#include <xstrings.h>
#include <ss_oled.h>
#include <screen.h>
#include <MemoryFree.h>

#define true 1
#define false 0

#define max_width 127
#define rows 4
#define menuwidth 30

class Menu
{
public:
    STRING *menuItems = 0;
    byte selected = 0;
    byte menuSize = 0;
    bool draw_bar;

    void SetupStrings(STRING opt[], byte count)
    {
        menuItems = opt;
        menuSize = count;
    }

    void Draw(SSOLED *oled, byte startx, byte startcol, byte colshigh)
    {
        byte startidx = selected / colshigh;
        for (byte i = 0; i < colshigh; i++)
        {
            byte itemidx = (startidx * colshigh) + i;

            if (itemidx < menuSize)
            {
                xGetString(menuItems[itemidx]);
                oledWriteString(oled, 0, startx, startcol + i, internalbuffer, FONT_6x8, selected == itemidx, 0);
            }
            if (selected == itemidx && draw_bar)
            {
                oledDrawLine(oled,
                             (strlen(internalbuffer)) * 6 + 1,
                             (startcol * 8) + (i * 8),
                             (strlen(internalbuffer)) * 6 + 1,
                             (startcol * 8) + (i * 8) + 7, 0
                             );
            }
        }
    }

    void ButtonSwitch()
    {
        selected++;
        selected = selected >= menuSize ? 0 : selected;
    }
};
