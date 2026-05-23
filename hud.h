#ifndef HUD_H
#define HUD_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "tower.h"
#include <cstdio>

//variables for the main hud display
const int drakeCost     = 100; // amount of gold to buy a drake
const int maxDrakeLimit = 10;  // maximum number of drake towers

// draws the hud at the top of the screen
inline void drawHud(ALLEGRO_FONT* font, int gold, int towerCount) {
    al_draw_filled_rectangle(0, 0, SCREEN_W, 44, al_map_rgba(0, 0, 0, 200));
    al_draw_line(0, 44, SCREEN_W, 44, al_map_rgb(80, 80, 80), 1);

    char goldBuf[64], drakeBuf[64];
    snprintf(goldBuf,  sizeof(goldBuf),  "Gold: %d (Drake = %d)", gold, drakeCost);
    snprintf(drakeBuf, sizeof(drakeBuf), "Drakes: %d / %d", towerCount, maxDrakeLimit);

    al_draw_text(font, al_map_rgb(255, 215, 0),       20,          14, ALLEGRO_ALIGN_LEFT,  goldBuf);
    al_draw_text(font, al_map_rgb(100, 200, 255), SCREEN_W - 20,   14, ALLEGRO_ALIGN_RIGHT, drakeBuf);
}

#endif // preprocessor directive
