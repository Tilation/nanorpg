#pragma once
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <xstrings.h>
#define DMG_VARIATION 4.0

class Entity
{
public:
  const static byte BYTES = 12;

  byte Dmg;
  byte Def;
  byte Int;
  char Name[6];
  byte Lvl;
  word MaxHp;
  word Hp;
  byte spr_idx;

  Entity()
  {
    //xGetString(STRING::empty_text);
    strcpy(Name, "EMPTY");
    MaxHp = 1;
    Hp = 1;
    Lvl = 0;
    Dmg = 0;
    Def = 0;
  }

  bool Damage(word incomingDamage)
  {
    word calcDmg = incomingDamage;
    if (Def >= incomingDamage) 
    {
      calcDmg = 1;
    }else
    {
      calcDmg = calcDmg - word(Def);
    }
    if (calcDmg <= 0)
    {
      calcDmg = 1;
    }
    if (calcDmg > Hp)
    {
      calcDmg = Hp;
    }
    Hp -= calcDmg;
    return Hp == 0;
  }

  word GenerateDamage()
  {
    return random(max(0, Dmg - (Dmg / DMG_VARIATION)), Dmg + (Dmg / DMG_VARIATION));
  }
};