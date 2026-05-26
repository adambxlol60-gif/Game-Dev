//created by Noah Basaria and Adam Jurewicz

#ifndef HUD_H
#define HUD_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <cstdio>

const int drakeCost = 200;
const int weekndCost = 400;
const int maxTowerLimit = 10;
const int goldPerKill = 5;

const int hudHeight = 70;
const int drakeBtnW = 120;
const int drakeBtnH = 40;
const int drakeBtnX = 20;
const int drakeBtnY = (hudHeight - drakeBtnH) / 2;

const int weekndBtnW = 120;
const int weekndBtnH = 40;
const int weekndBtnX = drakeBtnX + drakeBtnW + 10;
const int weekndBtnY = (hudHeight - weekndBtnH) / 2;

//Draws the hud at the top of the screen, it shows the player how much gold they have and how many towers they have placed out of the tower limit
inline void drawHud(ALLEGRO_FONT* font, int gold, int towerCount, int towerCost) {
    al_draw_filled_rectangle(0, 0, screenW, hudHeight, al_map_rgba(0, 0, 0, 200));
    al_draw_line(0, hudHeight, screenW, hudHeight, al_map_rgb(80, 80, 80), 1);

    char goldBuf[64], towerBuf[64];
    snprintf(goldBuf, sizeof(goldBuf), "Gold: %d (Tower = %d)", gold, towerCost);
    snprintf(towerBuf, sizeof(towerBuf), "Towers: %d / %d", towerCount, maxTowerLimit);

    al_draw_text(font, al_map_rgb(255, 215, 0), screenW / 2, hudHeight / 2 - 4, ALLEGRO_ALIGN_CENTER, goldBuf);
    al_draw_text(font, al_map_rgb(100, 200, 255), screenW - 20, hudHeight / 2 - 4, ALLEGRO_ALIGN_RIGHT, towerBuf);
}

//drawDrakeButton draws the Drake selection button on the hud with a black outline and a green fill when selected
inline void placeDrakeButton(ALLEGRO_FONT* font, bool selected) {
    ALLEGRO_COLOR fill = selected ? al_map_rgb(60, 140, 60) : al_map_rgb(60, 60, 60);
    al_draw_filled_rectangle(drakeBtnX, drakeBtnY, drakeBtnX + drakeBtnW, drakeBtnY + drakeBtnH, fill);
    al_draw_rectangle(drakeBtnX, drakeBtnY, drakeBtnX + drakeBtnW, drakeBtnY + drakeBtnH, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font, al_map_rgb(255, 255, 255), drakeBtnX + drakeBtnW / 2, drakeBtnY + drakeBtnH / 2 - 4, ALLEGRO_ALIGN_CENTER, "Drake");
}
inline void placeWeekndButton(ALLEGRO_FONT* font, bool selected) {
    ALLEGRO_COLOR fill = selected ? al_map_rgb(60, 140, 60) : al_map_rgb(60, 60, 60);
    al_draw_filled_rectangle(weekndBtnX, weekndBtnY, weekndBtnX + weekndBtnW, weekndBtnY + weekndBtnH, fill);
    al_draw_rectangle(weekndBtnX, weekndBtnY, weekndBtnX + weekndBtnW, weekndBtnY + weekndBtnH, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font, al_map_rgb(255, 255, 255), weekndBtnX + weekndBtnW / 2, weekndBtnY + weekndBtnH / 2 - 4, ALLEGRO_ALIGN_CENTER, "Weeknd");
}

//drakeButtonHit returns true if the given mouse coordinates land inside the Drake button area
inline bool drakeButtonPressed(int mouseX, int mouseY) {
    return mouseX >= drakeBtnX && mouseX <= drakeBtnX + drakeBtnW && mouseY >= drakeBtnY && mouseY <= drakeBtnY + drakeBtnH;
}
inline bool weekndButtonPressed(int mouseX, int mouseY) {
    return mouseX >= weekndBtnX && mouseX <= weekndBtnX + weekndBtnW && mouseY >= weekndBtnY && mouseY <= weekndBtnY + weekndBtnH;
}

#endif
