#pragma once
#include <Arduino.h>

const byte classes_bytes[][10] PROGMEM = 
{
    { // Class: Warrior
        // Base Stats:
        25,             // Max Hp
        0,              // Max Mana
        4,              // Inteligence
        6,              // Defense
        4,              // Attack
        // Gain on level up:
        8,              // Max Hp
        2,              // Max Mana
        0,              // Inteligence
        1,              // Defense
        2,              // Attack
    },

    { // Class: Wizard
        // Base Stats:
        10,             // Max Hp
        40,             // Max Mana
        8,              // Inteligence
        2,              // Defense
        3,              // Attack
        // Gain on level up:
        3,              // Max Hp
        10,              // Max Mana
        3,              // Inteligence
        0,              // Defense
        1,              // Attack
    },

    { // Class: Paladin
        // Base Stats:
        15,             // Max Hp
        0,              // Max Mana
        6,              // Inteligence
        3,              // Defense
        4,              // Attack
        // Gain on level up:
        7,              // Max Hp
        7,              // Max Mana
        1,              // Inteligence
        1,              // Defense
        1,              // Attack
    }
};