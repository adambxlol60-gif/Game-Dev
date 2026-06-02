//created by Noah Basaria and Adam Jurewicz

#ifndef HUD_H
#define HUD_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <cstdio>

const int drakeCost = 200;
const int weekndCost = 400;
const int elonCost = 1000;
const int bankCost = 1000;
const int maxTowerLimit = 10;
const int goldPerKill = 5;
const int health = 20;

const int hudHeight = 70;
const int drakeBtnW = 120;
const int drakeBtnH = 40;
const int drakeBtnX = 20;
const int drakeBtnY = (hudHeight - drakeBtnH) / 2;

const int weekndBtnW = 120;
const int weekndBtnH = 40;
const int weekndBtnX = drakeBtnX + drakeBtnW + 10;
const int weekndBtnY = (hudHeight - weekndBtnH) / 2;
const int elonBtnW = 120;
const int elonBtnH = 40;
const int elonBtnX = weekndBtnX + weekndBtnW + 10;
const int elonBtnY = (hudHeight - elonBtnH) / 2;
const int bankBtnW = 120;
const int bankBtnH = 40;
const int bankBtnX = elonBtnX + elonBtnW + 10;
const int bankBtnY = (hudHeight - bankBtnH) / 2;
const int nextWaveBtnW = 120;
const int nextWaveBtnH = 40;
const int nextWaveBtnX = 580;
const int nextWaveBtnY = screenH - nextWaveBtnH - 40;

//Draws the hud bar with gold, tower count, and player health. Only shows the price hint when a stower button is selected
inline void drawHud(ALLEGRO_FONT* font, int gold, int towerCount, bool drakeSelected, bool weekndSelected, bool elonSelected, bool bankSelected, int playerHealth, ALLEGRO_BITMAP* heartBmp) {
    al_draw_filled_rectangle(0, 0, screenW, hudHeight, al_map_rgba(0, 0, 0, 200));
    al_draw_line(0, hudHeight, screenW, hudHeight, al_map_rgb(80, 80, 80), 1);

    //health bar
    const int maxHealth = 20; //must match your starting health
    const int barX = 740; //left edge of the bar
    const int barY = 25; // top edge of the bar
    const int barW = 160; //total width of bar
    const int barH = 20; //height of bar

    float ratio = (float)playerHealth / maxHealth;  // 0.0 to 1.0
    if (ratio < 0) ratio = 0;

    //choose color based on how high the players health is
    ALLEGRO_COLOR barColor;
    if (ratio > 0.5f) barColor = al_map_rgb(50, 200, 50); // this is for green
    else if (ratio > 0.25f) barColor = al_map_rgb(255, 200, 0); //this is for yellow
    else barColor = al_map_rgb(220, 40, 40); //this is for red

    al_draw_filled_rectangle(barX, barY, barX + barW, barY + barH, al_map_rgb(60, 20, 20)); // background of the bar when empty
    al_draw_filled_rectangle(barX, barY, barX + (int)(barW * ratio), barY + barH, barColor); // filled portion of the bar
    al_draw_rectangle(barX, barY, barX + barW, barY + barH, al_map_rgb(0, 0, 0), 2); // border of the bar

    // heart next to bar
    int heartSize = 36;
    al_draw_scaled_bitmap(heartBmp, 0, 0, al_get_bitmap_width(heartBmp), al_get_bitmap_height(heartBmp), barX - heartSize - 5, barY - 8, heartSize, heartSize, 0);
    

    char goldBuf[64], towerBuf[64];
    if (drakeSelected) {
        snprintf(goldBuf, sizeof(goldBuf), "Gold: %d (Drake = %d)", gold, drakeCost);
    } else if (weekndSelected) {
        snprintf(goldBuf, sizeof(goldBuf), "Gold: %d (Weeknd = %d)", gold, weekndCost);
    } else if (elonSelected) {
        snprintf(goldBuf, sizeof(goldBuf), "Gold: %d (Elon = %d)", gold, elonCost);
    } else if (bankSelected) {
        snprintf(goldBuf, sizeof(goldBuf), "Gold: %d (Bank = %d)", gold, bankCost);
    } else {
        snprintf(goldBuf, sizeof(goldBuf), "Gold: %d", gold);
    }
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


inline void placeNextWaveButton(ALLEGRO_FONT* font, bool betweenWaves, int currentWave, int waveCount) {
    if (!betweenWaves) return;
    bool allDone = (currentWave >= waveCount);
    ALLEGRO_COLOR fill = allDone ? al_map_rgb(80, 80, 80) : al_map_rgb(128, 128, 128);
    al_draw_filled_rectangle(nextWaveBtnX, nextWaveBtnY, nextWaveBtnX + nextWaveBtnW, nextWaveBtnY + nextWaveBtnH, fill);
    al_draw_rectangle(nextWaveBtnX, nextWaveBtnY, nextWaveBtnX + nextWaveBtnW, nextWaveBtnY + nextWaveBtnH, al_map_rgb(0, 0, 0), 2);
    const char* label = allDone ? "All Waves are Done" : "Start Wave";
    al_draw_text(font, al_map_rgb(255, 255, 255), nextWaveBtnX + nextWaveBtnW / 2, nextWaveBtnY + nextWaveBtnH / 2 - 4, ALLEGRO_ALIGN_CENTER, label);
}

inline bool nextWaveButtonPressed(int mouseX, int mouseY) {
    return mouseX >= nextWaveBtnX && mouseX <= nextWaveBtnX + nextWaveBtnW && mouseY >= nextWaveBtnY && mouseY <= nextWaveBtnY + nextWaveBtnH;
}
//drakeButtonHit returns true if the given mouse coordinates land inside the Drake button area
inline bool drakeButtonPressed(int mouseX, int mouseY) {
    return mouseX >= drakeBtnX && mouseX <= drakeBtnX + drakeBtnW && mouseY >= drakeBtnY && mouseY <= drakeBtnY + drakeBtnH;
}
inline bool weekndButtonPressed(int mouseX, int mouseY) {
    return mouseX >= weekndBtnX && mouseX <= weekndBtnX + weekndBtnW && mouseY >= weekndBtnY && mouseY <= weekndBtnY + weekndBtnH;
}
inline void placeElonButton(ALLEGRO_FONT* font, bool selected) {
    ALLEGRO_COLOR fill = selected ? al_map_rgb(60, 140, 60) : al_map_rgb(60, 60, 60);
    al_draw_filled_rectangle(elonBtnX, elonBtnY, elonBtnX + elonBtnW, elonBtnY + elonBtnH, fill);
    al_draw_rectangle(elonBtnX, elonBtnY, elonBtnX + elonBtnW, elonBtnY + elonBtnH, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font, al_map_rgb(255, 255, 255), elonBtnX + elonBtnW / 2, elonBtnY + elonBtnH / 2 - 4, ALLEGRO_ALIGN_CENTER, "Elon");
}
inline bool elonButtonPressed(int mouseX, int mouseY) {
    return mouseX >= elonBtnX && mouseX <= elonBtnX + elonBtnW && mouseY >= elonBtnY && mouseY <= elonBtnY + elonBtnH;
}
inline void placeBankButton(ALLEGRO_FONT* font, bool selected) {
    ALLEGRO_COLOR fill = selected ? al_map_rgb(60, 140, 60) : al_map_rgb(60, 60, 60);
    al_draw_filled_rectangle(bankBtnX, bankBtnY, bankBtnX + bankBtnW, bankBtnY + bankBtnH, fill);
    al_draw_rectangle(bankBtnX, bankBtnY, bankBtnX + bankBtnW, bankBtnY + bankBtnH, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font, al_map_rgb(255, 255, 255), bankBtnX + bankBtnW / 2, bankBtnY + bankBtnH / 2 - 4, ALLEGRO_ALIGN_CENTER, "Bank");
}
inline bool bankButtonPressed(int mouseX, int mouseY) {
    return mouseX >= bankBtnX && mouseX <= bankBtnX + bankBtnW && mouseY >= bankBtnY && mouseY <= bankBtnY + bankBtnH;
}

#endif
