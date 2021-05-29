#pragma once
#include <Arduino.h>
#include <avr/pgmspace.h>

class Entity
{
public:
  const static byte BYTES = 12;

  byte Dmg;
  byte Def;
  char Name[6];
  byte Lvl;
  word MaxHp;
  word Hp;
  byte spr_idx;

  Entity()
  {
    strcpy(Name, "EMPTY");
    MaxHp = 1;
    Hp = 1;
    Lvl = 0;
    Dmg = 0;
    Def = 0;
  }

  bool Damage(word incomingDamage)
  {
    Hp -= min(incomingDamage, Hp);
    return Hp == 0;
  }

  word GenerateDamage()
  {
    return random(min(0, Dmg - Lvl), Dmg + Lvl);
  }

  word GenerateDefense()
  {
    return random(min(0, Def - Lvl), Def + Lvl);
  }
};