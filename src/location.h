#pragma once
#include <Arduino.h>

struct Location
{
    static const byte BYTES = 22;
    byte id;
    char Name[12];
    byte flags;
    byte neighbors[8];
};