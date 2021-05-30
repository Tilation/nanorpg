#pragma once
#include <Arduino.h>
#include <entity.h>
#include <xclasses.h>

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
        LoadBaseStats();
        spr_idx = 0;
    }

    void LoadBaseStats()
    {
        MaxHp   += pgm_read_byte(&classes_bytes[playerclass][0]);
        MaxMana += pgm_read_byte(&classes_bytes[playerclass][1]);
        Int     += pgm_read_byte(&classes_bytes[playerclass][2]);
        Def     += pgm_read_byte(&classes_bytes[playerclass][3]);
        Dmg     += pgm_read_byte(&classes_bytes[playerclass][4]);
        Hp = MaxHp;
        Mana = MaxMana;
    
    }

    void OnLevelUp()
    {
        MaxHp   += pgm_read_byte(&classes_bytes[playerclass][CLASS_LEVELUP::MAXHP]);
        MaxMana += pgm_read_byte(&classes_bytes[playerclass][CLASS_LEVELUP::MAXMANA]);
        Int     += pgm_read_byte(&classes_bytes[playerclass][CLASS_LEVELUP::INT]);
        Def     += pgm_read_byte(&classes_bytes[playerclass][CLASS_LEVELUP::DEF]);
        Dmg     += pgm_read_byte(&classes_bytes[playerclass][CLASS_LEVELUP::DMG]);
        Hp = MaxHp;
        Mana = MaxMana;
    }

    bool AddExp(word amount)
    {
        exp += amount;
        bool leveledup = false;

        do
        {
            expforlevelup = Lvl + (((Lvl * Lvl) / 3.0) * 45) + 10;
            Lvl = ((exp >= expforlevelup) ? Lvl + 1 : Lvl);
            leveledup = (exp >= expforlevelup) ? true : leveledup;
            if (exp>= expforlevelup) OnLevelUp();
        } while (exp >= expforlevelup);

        return leveledup;
    }
};