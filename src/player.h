#pragma once
#include <Arduino.h>
#include <entity.h>
#include <enums.h>

class Player : public Entity
{
public:
    CLASS playerclass;
    word exp;
    word expforlevelup;
    word MaxMana;
    word Mana;


    Player(char name[6], CLASS cls)
    {
        playerclass = cls;
        strcpy(Name, name);
        spr_idx = 0;
    }

    byte AddExp(word amount)
    {
        exp += amount;
        byte leveledup = 0;

        do
        {
            expforlevelup = Lvl + (((Lvl * Lvl) / 3.0) * 45) + 10;
            //Lvl = ((exp >= expforlevelup) ? Lvl + 1 : Lvl);
            Lvl += (exp >= expforlevelup);
            leveledup += (exp >= expforlevelup);
        } while (exp >= expforlevelup);

        return leveledup;
    }
};