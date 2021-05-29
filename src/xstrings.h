#pragma once
#include <enums.h>
#include <Arduino.h>

#define true 1
#define false 0

char strbuffer[32];


// un array de chars, parecen strings pero es un solo array de chars
// son strings codificados en un solo array de chars, separado cada string con el delimitador \0
const char strings_bytes[] PROGMEM =
    {
        "HP%3d\0"\
        "DM%3d\0"\
        "DF%3d\0"\
    };
const word stringcount = 3;

char * GetString(STRING targetidx)      // argumento targetidx es el indice del string que queremos 0 daria HP%3d, 1 daria DM%3d, etc
{                                       
    word offset = 0;                    // offset de lectura, 0 es el primer char, 1 es el segundo, etc
    word stringstart = 0;               // marcador de donde comienza el string, es el valor del offset en el momento que se encontro el string que queriamos
    word currentidx = 0;                // es el indice del string actual, no de los chars, se calcula contando los \0 del array
    char c;                             // buffer para guardar el char actual

    do
    {
        
        c = char(pgm_read_byte(&strings_bytes[offset]));                                    // leo un char de la memoria que esta en strings_bytes usando el offset

        currentidx += (c == '\0');                                                          // si es \0 el char, aumento currentidx en 1

        stringstart +=  (currentidx == targetidx) * (stringstart == 0) * (offset + 1);      // si el currentidx es igual a targetidx Y stringstart es cero, se le suma a stringstart 
                                                                                            // (que era 0) el offset + 1 (porque el offset actual da al \0, nosotros queremos el offset
                                                                                            // que apunta al primer caracter del string)
        
        strbuffer[(offset >= stringstart) * (offset - stringstart)] = c;                    // si el offset es mayor o igual a stringstart, escribimos el caracter al buffer de salida

        offset++;                                                                           // aumentamos el offset en 1

    } while (currentidx <= targetidx);
}