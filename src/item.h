#include <Arduino.h>

class Item
{
    public:
    char Name[6];
    void (*OnUse)();

};