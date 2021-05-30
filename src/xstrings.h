#pragma once
#include <enums.h>
#include <Arduino.h>

#define true 1
#define false 0
#define buffersize 32

char strbuffer[buffersize];
char internalbuffer[buffersize];

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
    };
const word stringcount = 3;


void xGetString(STRING targetidx) // argumento targetidx es el indice del string que queremos 0 daria HP%3d, 1 daria DM%3d, etc
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
    delay(10);   
}