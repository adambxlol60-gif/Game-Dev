#ifndef WAVE_LOADER_H
#define WAVE_LOADER_H

#include <stdio.h>
#include <string.h>
#include "function.h"

const int maxWaves = 50;
const int maxSpawnsPerWave = 100;

//spawns one enemy at a time
struct Spawn {
    ALLEGRO_BITMAP* bitmap;
    int hp;
    float speed;
    int spawnInterval; // time in miliseconds when the enemy spawns
};

//one full wave, uses a flat array of enemies to spawn in order
struct Wave {
    Spawn spawns[maxSpawnsPerWave];
    int spawnCount; // number of spawns in this wave

};

//maps names from text file to the correct bitmap
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

// loads all the waves from a text file int allWaves array
// returns the number of waves loaded
inline int loadWaves(const char* filename, Wave allWaves[], int maxW) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return 0;
    }

    int waveCount = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        //skip comments or any blank lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        int waveNum, count, hp, spawnInterval;
        float speed;
        char type[64];

        if (sscanf(line, "%d %s %d %d %f %d", &waveNum, type, &count, &hp, &speed, &spawnInterval) != 6) continue;
        if (waveNum < 1 || waveNum > maxW) continue;
        
        if (waveNum > waveCount) waveCount = waveNum;

        Wave& wave = allWaves[waveNum - 1];

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