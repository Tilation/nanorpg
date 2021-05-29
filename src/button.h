#pragma once
#include <Arduino.h>

class Button
{
  private:
    const byte DEBOUNCE = 75;
    const int HOLD = 500;
    const byte HOLDINTERVAL = 10;
    byte SwPin = 0;
    byte oldSwState = -1;
    int mSecCounter = 0;
    bool debouncing = false;
    bool oldpressed = false;
    bool pressed = false;
    bool holding = false;

    void (*onPressedEvent)();

    // Define Button States
  public:
    Button(int sw, void (*function)())
    {
      SwPin = sw;
      onPressedEvent = function;
    }
    int PIN(void) {
      return SwPin;
    }

    void buttonCallBack(void)
    {
      int readSw = digitalRead(SwPin);
   
      if (readSw == 1 && oldpressed == true)
      {
        if (holding)
        {
          mSecCounter++;
          if (mSecCounter > HOLDINTERVAL)
          {
            mSecCounter = 0;
            (*onPressedEvent)();
          }
          return;
        }
        mSecCounter++;

        if (mSecCounter >= HOLD)
        {
          mSecCounter = 0;
          holding = true;
        }
        return;
      }
      holding = false;
      if (oldpressed == false && pressed == true)
      {
        (*onPressedEvent)();
        oldpressed = true;
        pressed = false;
        mSecCounter = 0;
        return;
      }

      if (readSw == 0)
      {
        if (oldpressed == true)
        {
          oldpressed = false;
          pressed = false;
          mSecCounter = 0;
          return;
        }
      }

      //detect if debounce or real press
      if (debouncing == true)
      {
        mSecCounter++;
        if (mSecCounter > DEBOUNCE)
        {
          debouncing = false;
          oldpressed = false;
          pressed = readSw == 1;
          mSecCounter = 0;
        }
        return;
      }

      if (readSw == 1)
      {
        debouncing = true;
        mSecCounter = 0;
        return;
      }
    }
};
