#pragma once
#include <Arduino.h>
#include <data.h>
#include <OneBitDisplay.h>
#include <screen.h>

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

    void Draw(OBDISP *oled, byte startx, byte startcol, byte colshigh)
    {
        byte startidx = selected / colshigh;
        for (byte i = 0; i < colshigh; i++)
        {
            byte itemidx = (startidx * colshigh) + i;

            if (itemidx < menuSize)
            {
                obdWriteString(
                    oled,
                    0,
                    startx, startcol + i,
                    Data::xGetString(menuItems[itemidx]),
                    FONT_6x8,
                    selected == itemidx, 0);
            }
            if (selected == itemidx && draw_bar)
            {
                obdDrawLine(oled,
                             startx + (strlen(Data::internalbuffer)) * 6 + 1,
                             (startcol * 8) + (i * 8),
                             startx + (strlen(Data::internalbuffer)) * 6 + 1,
                             (startcol * 8) + (i * 8) + 7,
                            1,0
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
