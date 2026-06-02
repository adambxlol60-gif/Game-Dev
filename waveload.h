#ifndef WAVE_LOADER_H
#define WAVE_LOADER_H

#include <stdio.h>
#include <string.h>
#include "function.h"

// maximum number of waves the game can have
const int maxWaves = 100;
// maximum number of individual enemy spawns allowed in a single wave
const int maxSpawnsPerWave = 200;

// Spawn holds the data for one single enemy that will be spawned
struct Spawn {
    ALLEGRO_BITMAP* bitmap;
    int hp;
    float speed;
    int spawnInterval; // time in miliseconds when the enemy spawns
};

// Wave holds all the spawns for one full wave
struct Wave {
    Spawn spawns[maxSpawnsPerWave];
    int spawnCount; // number of spawns in this wave
};

// getBitmapByName takes the enemy name string from the text file
inline ALLEGRO_BITMAP* getBitmapByName(const char* name) {
    if (strcmp(name, "slime") == 0) return bitmaps[2];
    if (strcmp(name, "blueSlime") == 0) return bitmaps[5];
    if (strcmp(name, "blueCamoSlime") == 0) return bitmaps[6];
    if (strcmp(name, "greenCamoSlime") == 0) return bitmaps[7];
    if (strcmp(name, "metalSlime") == 0) return bitmaps[8];
    if (strcmp(name, "purpleSlime") == 0) return bitmaps[9];
    if (strcmp(name, "purpleCamoSlime") == 0) return bitmaps[10];
    if (strcmp(name, "rainbowSlime") == 0) return bitmaps[11];
    if (strcmp(name, "rainbowCamoSlime") == 0) return bitmaps[12];
    if (strcmp(name, "redSlime") == 0) return bitmaps[13];
    if (strcmp(name, "redCamoSlime") == 0) return bitmaps[14];
    if (strcmp(name, "yellowSlime") == 0) return bitmaps[15];
    if (strcmp(name, "yellowCamoSlime") == 0) return bitmaps[16];
    return bitmaps[2]; // default to basic slime if name not found
}

// loadWaves reads waves.txt and fills the allWaves array
inline int loadWaves(const char* filename, Wave allWaves[], int maxW) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return 0; //if the file can't be opened, it returns 0 waves
    }

    int waveCount = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        //skips comments or any blank lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        int waveNum, count, hp, spawnInterval;
        float speed;
        char type[64];

        // scanfs the 6 expected values from the line, it skips it if its wrong
        if (sscanf(line, "%d %s %d %d %f %d", &waveNum, type, &count, &hp, &speed, &spawnInterval) != 6) continue;

        // it skips the number if its out of range
        if (waveNum < 1 || waveNum > maxW) continue;

        // if statement tracks the highest wave number seen so far
        if (waveNum > waveCount) waveCount = waveNum;

        // gets the wave at this index (we do waveNum - 1 because wave numbers start at 1 but array indices start at 0)
        Wave& wave = allWaves[waveNum - 1];

        // for loop adds count copies of this enemy type to the wave's spawn list
        for(int i = 0; i < count; i++) {
            if (wave.spawnCount >= maxSpawnsPerWave) break; // prevents overflow
            Spawn& s = wave.spawns[wave.spawnCount++];
            s.bitmap = getBitmapByName(type);
            s.hp = hp;
            s.speed = speed;
            s.spawnInterval = spawnInterval;
        }
    }

    fclose(file);
    return waveCount;
}

#endif