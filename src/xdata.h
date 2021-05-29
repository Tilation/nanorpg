#pragma once
#include <Arduino.h>
#include <entity.h>

const byte enemies_bytes[][Entity::BYTES] PROGMEM =
    {
        {
            'S', 'l', 'i', 'm', 'e', // Name: Slime
            0, 8,               // Max HP: 8
            1,                     // Level: 1
            2,                     // Damage: 2
            1,                     // Defense: 1
            0                     // Sprite Index: 0
        },
        {
            'W', 'o', 'l', 'f', '\0', // Name: Wolf
            0, 21,               // Max HP: 21
            2,                      // Level: 2
            4,                      // Damage: 4
            2,                      // Defense: 2
            0                      // Sprite Index: 0
        }
};

const word enemyCount = sizeof(enemies_bytes) / Entity::BYTES;

void GetEnemy(Entity * entity, byte idx)
{
    sprintf(entity->Name, "%c%c%c%c%c\0",
      char(pgm_read_byte(&enemies_bytes[idx][0])),
      char(pgm_read_byte(&enemies_bytes[idx][1])),
      char(pgm_read_byte(&enemies_bytes[idx][2])),
      char(pgm_read_byte(&enemies_bytes[idx][3])),
      char(pgm_read_byte(&enemies_bytes[idx][4]))
    );
    
    entity->MaxHp = word(pgm_read_byte(&enemies_bytes[idx][5]), pgm_read_byte(&enemies_bytes[idx][6]));
    entity->Hp = entity->MaxHp;

    entity->Lvl = pgm_read_byte(&enemies_bytes[idx][7]);

    entity->Dmg = pgm_read_byte(&enemies_bytes[idx][8]);
    entity->Def = pgm_read_byte(&enemies_bytes[idx][9]);

    entity->spr_idx = pgm_read_byte(&enemies_bytes[idx][10]);
}