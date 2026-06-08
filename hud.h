#ifndef HUD_H
#define HUD_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <cstdio>

//Here we declare the integers for the size of the ui
const int drakeCost = 200;
const int weekndCost = 500;
const int elonCost = 1000;
const int bankCost = 1000;
const int maxTowerLimit = 15;
const int hudHeight = 70;
const int drakeButtonW = 120;
const int drakeButtonH = 40;
const int drakeButtonX = 20;
const int drakeButtonY = (hudHeight - drakeButtonH) / 2;
const int weekndButtonW = 120;
const int weekndButtonH = 40;
const int weekndButtonX = drakeButtonX + drakeButtonW + 10;
const int weekndButtonY = (hudHeight - weekndButtonH) / 2;
const int elonButtonW = 120;
const int elonButtonH = 40;
const int elonButtonX = weekndButtonX + weekndButtonW + 10;
const int elonButtonY = (hudHeight - elonButtonH) / 2;
const int bankButtonW = 120;
const int bankButtonH = 40;
const int bankButtonX = elonButtonX + elonButtonW + 10;
const int bankButtonY = (hudHeight - bankButtonH) / 2;
const int nextWaveButtonW = 120;
const int nextWaveButtonH = 40;
const int nextWaveButtonX = 580;
const int nextWaveButtonY = screenH - nextWaveButtonH - 40;

//Draws the hud bar with gold, tower count, and player health. Only shows the price hint when a stower button is selected
inline void drawHud(ALLEGRO_FONT* font, int gold, int towerCount, bool drakeSelected, bool weekndSelected, bool elonSelected, bool bankSelected, int playerHealth, ALLEGRO_BITMAP* heartBitmap, int currentWave, int waveCount) {
    al_draw_filled_rectangle(0, 0, screenW, hudHeight, al_map_rgba(0, 0, 0, 200));
    al_draw_line(0, hudHeight, screenW, hudHeight, al_map_rgb(80, 80, 80), 1);

    //health bar
    const int maxHealth = 20; //max health
    const int barX = 790; //left edge of the bar
    const int barY = 25; // top edge of the bar
    const int barW = 160; //total width of bar
    const int barH = 20; //height of bar

    float ratio = (float)playerHealth / maxHealth;  //this is the ratio used for the color of the bar, we didn't like using numbers to represent the health so we used a bar
    if (ratio < 0) ratio = 0;

    //using the previously made ratio this changes the color of the bar to indicate hp
    ALLEGRO_COLOR barColor;
    if (ratio > 0.5f) barColor = al_map_rgb(50, 200, 50); // this is for green
    else if (ratio > 0.25f) barColor = al_map_rgb(255, 200, 0); //this is for yellow
    else barColor = al_map_rgb(220, 40, 40); //this is for red

    al_draw_filled_rectangle(barX, barY, barX + barW, barY + barH, al_map_rgb(60, 20, 20)); // background of the bar when empty
    al_draw_filled_rectangle(barX, barY, barX + (int)(barW * ratio), barY + barH, barColor); // filled portion of the bar
    al_draw_rectangle(barX, barY, barX + barW, barY + barH, al_map_rgb(0, 0, 0), 2); // border of the bar

    // heart next to bar
    int heartSize = 36;
    al_draw_scaled_bitmap(heartBitmap, 0, 0, al_get_bitmap_width(heartBitmap), al_get_bitmap_height(heartBitmap), barX - heartSize - 5, barY - 8, heartSize, heartSize, 0);
    
    //this is to show the cost of the drake and all the other towers next to the gold bar
    //Also for the hid we used snprintf instead of printf because it builds a string with variables inserted into it and saves it into a buffer so al_draw_text can use it.
    //This is https://www.programiz.com/cpp-programming/library-function/cstdio/snprintf to learn about it
    //https://en.cppreference.com/cpp/io/c/fprintf
    char goldBuf[64], towerBuf[64];
    if (drakeSelected) {
        snprintf(goldBuf, sizeof(goldBuf), "Gold: %d (Drake = %d)", gold, drakeCost);
    }
    else if (weekndSelected) {
        snprintf(goldBuf, sizeof(goldBuf), "Gold: %d (Weeknd = %d)", gold, weekndCost);
    }
    else if (elonSelected) {
        snprintf(goldBuf, sizeof(goldBuf), "Gold: %d (Elon = %d)", gold, elonCost);
    }
    else if (bankSelected) {
        snprintf(goldBuf, sizeof(goldBuf), "Gold: %d (Bank = %d)", gold, bankCost);
    }
    else {
        snprintf(goldBuf, sizeof(goldBuf), "Gold: %d", gold);
    }
    snprintf(towerBuf, sizeof(towerBuf), "Towers: %d / %d", towerCount, maxTowerLimit);
    //this part build the gold counter, tower counter and wave counter
    al_draw_text(font, al_map_rgb(255, 215, 0), 640, hudHeight / 2 - 4, ALLEGRO_ALIGN_CENTER, goldBuf);
    al_draw_text(font, al_map_rgb(100, 200, 255), 1250, hudHeight / 2 - 4, ALLEGRO_ALIGN_RIGHT, towerBuf);
    char waveBuf[64];
    snprintf(waveBuf, sizeof(waveBuf), "Wave: %d / %d", currentWave + 1, waveCount);
    al_draw_text(font, al_map_rgb(255, 255, 255), 1040, hudHeight / 2 - 4, ALLEGRO_ALIGN_CENTER, waveBuf);
}

//drawDrakeButton draws the Drake selection button on the hud with a black outline and a green fill when selected
inline void placeDrakeButton(ALLEGRO_FONT* font, bool selected) {
    ALLEGRO_COLOR fill;
    if (selected) { fill = al_map_rgb(60, 140, 60); } else { fill = al_map_rgb(60, 60, 60); }
    al_draw_filled_rectangle(drakeButtonX, drakeButtonY, drakeButtonX + drakeButtonW, drakeButtonY + drakeButtonH, fill);
    al_draw_rectangle(drakeButtonX, drakeButtonY, drakeButtonX + drakeButtonW, drakeButtonY + drakeButtonH, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font, al_map_rgb(255, 255, 255), drakeButtonX + drakeButtonW / 2, drakeButtonY + drakeButtonH / 2 - 4, ALLEGRO_ALIGN_CENTER, "Drake");
} //same as drake but for weeknd
inline void placeWeekndButton(ALLEGRO_FONT* font, bool selected) {
    ALLEGRO_COLOR fill;
    if (selected) { fill = al_map_rgb(60, 140, 60); } else { fill = al_map_rgb(60, 60, 60); }
    al_draw_filled_rectangle(weekndButtonX, weekndButtonY, weekndButtonX + weekndButtonW, weekndButtonY + weekndButtonH, fill);
    al_draw_rectangle(weekndButtonX, weekndButtonY, weekndButtonX + weekndButtonW, weekndButtonY + weekndButtonH, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font, al_map_rgb(255, 255, 255), weekndButtonX + weekndButtonW / 2, weekndButtonY + weekndButtonH / 2 - 4, ALLEGRO_ALIGN_CENTER, "Weeknd");
}

//this is the wave button, it only appears when wave is done 
inline void placeNextWaveButton(ALLEGRO_FONT* font, bool betweenWaves, int currentWave, int waveCount) {
    if (!betweenWaves) return;
    bool allDone = (currentWave >= waveCount);
    ALLEGRO_COLOR fill;
    if (allDone) { fill = al_map_rgb(80, 80, 80); } else { fill = al_map_rgb(128, 128, 128); }
    al_draw_filled_rectangle(nextWaveButtonX, nextWaveButtonY, nextWaveButtonX + nextWaveButtonW, nextWaveButtonY + nextWaveButtonH, fill);
    al_draw_rectangle(nextWaveButtonX, nextWaveButtonY, nextWaveButtonX + nextWaveButtonW, nextWaveButtonY + nextWaveButtonH, al_map_rgb(0, 0, 0), 2);
    const char* label;
    if (allDone) { label = "All Waves are Done"; } else { label = "Start Wave"; }
    al_draw_text(font, al_map_rgb(255, 255, 255), nextWaveButtonX + nextWaveButtonW / 2, nextWaveButtonY + nextWaveButtonH / 2 - 4, ALLEGRO_ALIGN_CENTER, label);
}
//beginner tip tucked into the bottom-right corner. auto hides once the player has cleared a few waves so that it wont waste space
const int tipBarHideWave = 3;  //hidden from wave 4 onward (currentWave is 0-indexed)

inline void drawTipBar(ALLEGRO_FONT* font, int currentWave) {
    if (currentWave >= tipBarHideWave) return;

    al_draw_text(font, al_map_rgb(255, 255, 255), screenW - 10, screenH - 24,
        ALLEGRO_ALIGN_RIGHT, "Tip: Left-click a tower to open its upgrade panel");
}

//bool for pressing the wave button
inline bool nextWaveButtonPressed(int mouseX, int mouseY) {
    return mouseX >= nextWaveButtonX && mouseX <= nextWaveButtonX + nextWaveButtonW && mouseY >= nextWaveButtonY && mouseY <= nextWaveButtonY + nextWaveButtonH;
}
//drakeButtonHit returns true if the given mouse coordinates land inside the Drake button area
inline bool drakeButtonPressed(int mouseX, int mouseY) {
    return mouseX >= drakeButtonX && mouseX <= drakeButtonX + drakeButtonW && mouseY >= drakeButtonY && mouseY <= drakeButtonY + drakeButtonH;
}//the same thing but for weeknd
inline bool weekndButtonPressed(int mouseX, int mouseY) {
    return mouseX >= weekndButtonX && mouseX <= weekndButtonX + weekndButtonW && mouseY >= weekndButtonY && mouseY <= weekndButtonY + weekndButtonH;
} //same button but for elon
inline void placeElonButton(ALLEGRO_FONT* font, bool selected) {
    ALLEGRO_COLOR fill;
    if (selected) { fill = al_map_rgb(60, 140, 60); } else { fill = al_map_rgb(60, 60, 60); }
    al_draw_filled_rectangle(elonButtonX, elonButtonY, elonButtonX + elonButtonW, elonButtonY + elonButtonH, fill);
    al_draw_rectangle(elonButtonX, elonButtonY, elonButtonX + elonButtonW, elonButtonY + elonButtonH, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font, al_map_rgb(255, 255, 255), elonButtonX + elonButtonW / 2, elonButtonY + elonButtonH / 2 - 4, ALLEGRO_ALIGN_CENTER, "Elon"); //the math right here is done for centering the image
}//same as in weeknd or drake
inline bool elonButtonPressed(int mouseX, int mouseY) {
    return mouseX >= elonButtonX && mouseX <= elonButtonX + elonButtonW && mouseY >= elonButtonY && mouseY <= elonButtonY + elonButtonH;
} //place button for the bank
inline void placeBankButton(ALLEGRO_FONT* font, bool selected) {
    ALLEGRO_COLOR fill;
    if (selected) { fill = al_map_rgb(60, 140, 60); } else { fill = al_map_rgb(60, 60, 60); }
    al_draw_filled_rectangle(bankButtonX, bankButtonY, bankButtonX + bankButtonW, bankButtonY + bankButtonH, fill);
    al_draw_rectangle(bankButtonX, bankButtonY, bankButtonX + bankButtonW, bankButtonY + bankButtonH, al_map_rgb(0, 0, 0), 2);
    al_draw_text(font, al_map_rgb(255, 255, 255), bankButtonX + bankButtonW / 2, bankButtonY + bankButtonH / 2 - 4, ALLEGRO_ALIGN_CENTER, "Bank");
}//same bool as in drake weeknd and elon
inline bool bankButtonPressed(int mouseX, int mouseY) {
    return mouseX >= bankButtonX && mouseX <= bankButtonX + bankButtonW && mouseY >= bankButtonY && mouseY <= bankButtonY + bankButtonH;
}

#endif
