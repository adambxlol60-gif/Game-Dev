#ifndef HUD_H
#define HUD_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <cstdio>

const int towerCost = 100;
const int maxTowerLimit = 10;
const int goldPerKill = 5;

//Draws the hud at the top of the screen, it shows the player how much gold they have and how many towers they have placed out of the tower limit
inline void drawHud(ALLEGRO_FONT* font, int gold, int towerCount) {
    al_draw_filled_rectangle(0, 0, screenW, 44, al_map_rgba(0, 0, 0, 200));
    al_draw_line(0, 44, screenW, 44, al_map_rgb(80, 80, 80), 1);

    char goldBuf[64], towerBuf[64];
    snprintf(goldBuf, sizeof(goldBuf), "Gold: %d (Tower = %d)", gold, towerCost);
    snprintf(towerBuf, sizeof(towerBuf), "Towers: %d / %d", towerCount, maxTowerLimit);

    al_draw_text(font, al_map_rgb(255, 215, 0), 20, 14, ALLEGRO_ALIGN_LEFT, goldBuf);
    al_draw_text(font, al_map_rgb(100, 200, 255), screenW - 20, 14, ALLEGRO_ALIGN_RIGHT, towerBuf);
}

#endif
