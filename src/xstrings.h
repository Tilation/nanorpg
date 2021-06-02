#pragma once
#include <Arduino.h>

// un array de chars, parecen strings pero es un solo array de chars
// son strings codificados en un solo array de chars, separado cada string con el delimitador \0
const char strings_bytes[] PROGMEM =
    {
        "HP%3d\0"\
        "DM%3d\0"\
        "DF%3d\0"\
        "INFO: DM/DF:%3d/%-3d\0"\
        "%-5s    HP:%3d/%-3d\0"\
        "Attck\0"\
        "Objct\0"\
        "Info \0"\
        "Flee \0"\
        "Next up is \0"\
        "Killed\0"\
        "EMPTY\0"\
        "%c%c%c%c%c\0"\
        "Advnt\0"\
        "Items\0"\
        "Stats\0"\
        "Craft\0"\
        "Hideout\0"\
        "'s \0"\
        "' \0"\
        "Trade\0"\
        "Data\0"\
        "Save\0"\
        "Load\0"\
        " \0"\
        "La tribu Otiko ataco\0"\
        "nuevamente a Toko...\0"\
        "Pero esta vez, no se\0"\
        "salvo nadie...      \0"\
        "%c%c%c%c%c%c%c%c%c%c%c%c\0"\
    };