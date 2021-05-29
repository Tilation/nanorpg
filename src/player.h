#pragma once
#include <Arduino.h>
#include <entity.h>

class Player : public Entity
{
public:
    word exp;
    word expforlevelup;

    Player(char name[6])
    {
        strcpy(Name, name);

        Dmg = 4;
        Def = 2;
        Lvl = 1;
        MaxHp = 10;
        Hp = 10;
        spr_idx = 0;
    }

    bool AddExp(word amount)
    {
        Serial.print("Experience gained: ");
        Serial.println(amount);
        exp += amount;
        bool leveledup = false;
        do
        {
            expforlevelup = Lvl + (((Lvl * Lvl) / 3.0) * 45) + 10;
            Lvl = ((exp >= expforlevelup) ? Lvl + 1 : Lvl);
            leveledup = (exp >= expforlevelup) ? true : leveledup;
        } while (exp >= expforlevelup);

        if (leveledup)
        {
            Serial.print("level up! you're now lvl: ");
            Serial.println(Lvl);
        }
        Serial.print("exp progress: ");
        Serial.print(exp);
        Serial.print("/");
        Serial.println(expforlevelup);

        return leveledup;
    }
};