#pragma once
#include <Arduino.h>
#include <location.h>
const byte locations_bytes[][Location::BYTES] PROGMEM =
    { // ID, Nombre del lugar, {id de vecinos, empieza en norte y va como las agujas del reloj}
        {
            //Id del lugar
            1,
            //Nombre del lugar, 12 bytes
            'C', 'a', 's', 'a', ' ', '1', ' ', ' ', ' ', ' ', ' ', ' ',
            //Flags del lugar
            0,
            //Vecinos
            0, 0, 0,
            0,    0,
            0, 0, 0

        }

};
