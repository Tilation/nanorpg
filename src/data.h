/*
    Este es el punto de comunicacion entre los datos guardados en PROGMEM
    y el programa en si, todo archivo con el formato x*.h debe sólo ser
    referenciado por esta clase.
*/
#pragma once
#include <Arduino.h>
#include <xenemies.h>
#include <xclasses.h>
#include <xstrings.h>
#include <xlocations.h>
#include <enums.h>
#include <EEPROM.h>
#include <entity.h>
#include <player.h>

#define true 1
#define false 0
#define buffersize 32

class Data
{
    public:
    static char * strbuffer;
    static char * internalbuffer;
    
    static void Setup( char _strbuffer[buffersize], char _internalbuffer[buffersize])
    {
        strbuffer = _strbuffer;
        internalbuffer = _internalbuffer;
    }


    // Lee un string de maximo 32 bytes y lo devuelve como char[32], usa internalbuffer
    static char * xGetString(STRING targetidx) // argumento targetidx es el indice del string que queremos 0 daria HP%3d, 1 daria DM%3d, etc
    {
        unsigned int offset = 0;      // offset de lectura, 0 es el primer char, 1 es el segundo, etc
        unsigned int stringstart = 0; // marcador de donde comienza el string, es el valor del offset en el momento que se encontro el string que queriamos
        unsigned int currentidx = 0;  // es el indice del string actual, no de los chars, se calcula contando los \0 del array
        char c;               // buffer para guardar el char actual
        do
        {
            c = char(pgm_read_byte(&strings_bytes[offset])); // leo un char de la memoria que esta en strings_bytes usando el offset
            
            // Establecer desde donde arranca el string (si targetidx es 0, stringstart arranca en 0)
            // Se usa += porque si usara = se asignaria a 0 si el chequeo fuera falso
            // De esta manera, si no detectamos el inicio del string, se estaria sumando 0 al stringstart
            stringstart +=  (currentidx == targetidx) *             // Si el currentidx es igual al targetidx, y
                            (targetidx != 0) *                      // Si el targetidx es diferente a 0, y
                            ((stringstart + offset) == offset) *    // Si stringstart todavia no se asignó un valor, entonces
                            offset;                                 // Asignarle el offset a stringstart

            internalbuffer[(offset >= stringstart) * (offset - stringstart)] = c; // Si stringstart es valido, entonces esribo el char en su posicion

            currentidx += (c == '\0'); // si el char es NULL, aumento currentidx en 1
            offset++; // aumentamos el offset en 1
        } while (currentidx <= targetidx);
        return internalbuffer;
    }

    // Reads an enemy idx from PROGMEM and saves it to * entity
    static void GetEnemy(Entity * entity, byte idx)
    {
        xGetString(STRING::get_enemy_format);

        sprintf(entity->Name, internalbuffer,
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
    // Reads a location idx from PROGMEM and saves it to *location
    static void GetLocation(Location *location, byte idx)
    {
        location->id = pgm_read_byte(&locations_bytes[idx][0]);

        sprintf(location->Name, xGetString(STRING::get_enemy_format),
                char(pgm_read_byte(&locations_bytes[idx][1])),
                char(pgm_read_byte(&locations_bytes[idx][2])),
                char(pgm_read_byte(&locations_bytes[idx][3])),
                char(pgm_read_byte(&locations_bytes[idx][4])),
                char(pgm_read_byte(&locations_bytes[idx][5])),
                char(pgm_read_byte(&locations_bytes[idx][6])),
                char(pgm_read_byte(&locations_bytes[idx][7])),
                char(pgm_read_byte(&locations_bytes[idx][8])),
                char(pgm_read_byte(&locations_bytes[idx][9])),
                char(pgm_read_byte(&locations_bytes[idx][10])),
                char(pgm_read_byte(&locations_bytes[idx][11])),
                char(pgm_read_byte(&locations_bytes[idx][12])));

        location->flags = pgm_read_byte(&locations_bytes[idx][13]);

        location->neighbors[0] = pgm_read_byte(&locations_bytes[idx][14]);
        location->neighbors[1] = pgm_read_byte(&locations_bytes[idx][15]);
        location->neighbors[2] = pgm_read_byte(&locations_bytes[idx][16]);
        location->neighbors[3] = pgm_read_byte(&locations_bytes[idx][17]);
        location->neighbors[4] = pgm_read_byte(&locations_bytes[idx][18]);
        location->neighbors[5] = pgm_read_byte(&locations_bytes[idx][19]);
        location->neighbors[6] = pgm_read_byte(&locations_bytes[idx][20]);
        location->neighbors[7] = pgm_read_byte(&locations_bytes[idx][21]);
    }

    static void PlayerSaveToEEPROM(Player *p)
    {
        byte index = 0;
        
        EEPROM.put( index, p->Dmg);
        index += sizeof(p->Dmg);
        EEPROM.put( index, p->Def);
        index += sizeof(p->Def);
        EEPROM.put( index, p->Int);
        index += sizeof(p->Int);
        EEPROM.put( index, p->Name);
        index += sizeof(p->Name);
        EEPROM.put( index, p->Lvl);
        index += sizeof(p->Lvl);
        EEPROM.put( index, p->MaxHp);
        index += sizeof(p->MaxHp);
        EEPROM.put( index, p->Hp);
        index += sizeof(p->Hp);
        EEPROM.put( index, p->spr_idx);
        index += sizeof(p->spr_idx);
        EEPROM.put( index, p->playerclass);
        index += sizeof(p->playerclass);
        EEPROM.put( index, p->exp);
        index += sizeof(p->exp);
        EEPROM.put( index, p->expforlevelup);
        index += sizeof(p->expforlevelup);
        EEPROM.put( index, p->MaxMana);
        index += sizeof(p->MaxMana);
        EEPROM.put( index, p->Mana);
        index += sizeof(p->Mana);
    }

    static void PlayerLoadFromEEPROM(Player * p)
    {
        byte index = 0;
        
        EEPROM.get( index, p->Dmg);
        index += sizeof(p->Dmg);
        EEPROM.get( index, p->Def);
        index += sizeof(p->Def);
        EEPROM.get( index, p->Int);
        index += sizeof(p->Int);
        EEPROM.get( index, p->Name);
        index += sizeof(p->Name);
        EEPROM.get( index, p->Lvl);
        index += sizeof(p->Lvl);
        EEPROM.get( index, p->MaxHp);
        index += sizeof(p->MaxHp);
        EEPROM.get( index, p->Hp);
        index += sizeof(p->Hp);
        EEPROM.get( index, p->spr_idx);
        index += sizeof(p->spr_idx);
        EEPROM.get( index, p->playerclass);
        index += sizeof(p->playerclass);
        EEPROM.get( index, p->exp);
        index += sizeof(p->exp);
        EEPROM.get( index, p->expforlevelup);
        index += sizeof(p->expforlevelup);
        EEPROM.get( index, p->MaxMana);
        index += sizeof(p->MaxMana);
        EEPROM.get( index, p->Mana);
        index += sizeof(p->Mana);
    }

    static void PlayerAddLevelUpStats(Player *p)
    {
        p->MaxHp   += pgm_read_byte(&classes_bytes[p->playerclass][CLASS_LEVELUP::MAXHP]);
        p->MaxMana += pgm_read_byte(&classes_bytes[p->playerclass][CLASS_LEVELUP::MAXMANA]);
        p->Int     += pgm_read_byte(&classes_bytes[p->playerclass][CLASS_LEVELUP::INT]);
        p->Def     += pgm_read_byte(&classes_bytes[p->playerclass][CLASS_LEVELUP::DEF]);
        p->Dmg     += pgm_read_byte(&classes_bytes[p->playerclass][CLASS_LEVELUP::DMG]);
        p->Hp = p->MaxHp;
        p->Mana = p->MaxMana;
    }

    static void PlayerLoadBaseStats(Player *p)
    {
        p->MaxHp   += pgm_read_byte(&classes_bytes[p->playerclass][0]);
        p->MaxMana += pgm_read_byte(&classes_bytes[p->playerclass][1]);
        p->Int     += pgm_read_byte(&classes_bytes[p->playerclass][2]);
        p->Def     += pgm_read_byte(&classes_bytes[p->playerclass][3]);
        p->Dmg     += pgm_read_byte(&classes_bytes[p->playerclass][4]);
        p->Hp = p->MaxHp;
        p->Mana = p->MaxMana;
    }
};

