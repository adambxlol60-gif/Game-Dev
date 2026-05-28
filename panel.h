//created by Noah Basaria and Adam Jurewicz

#ifndef PANEL_H
#define PANEL_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <cstdio>
#include "hud.h"
#include "tower.h"

//panel layout - sits on the left side under the hud
const int panelX = 30;
const int panelY = hudHeight + 5;
const int panelW = 245;
const int panelH = 365;

const int portraitX = panelX + 30;
const int portraitY = panelY + 15;
const int portraitW = 180;
const int portraitH = 200;

const int sellBtnX = panelX + 30;
const int sellBtnY = panelY + panelH - 60;
const int sellBtnW = 180;
const int sellBtnH = 45;

//refundFor returns the gold given back when selling a tower of the given type (70% of cost)
inline int refundFor(int type) {
    if (type == TOWER_WEEKND) return (int)(weekndCost * 0.7f);
    return (int)(drakeCost * 0.7f);
}

//clickInsidePanel returns true if the mouse is inside the side panel area
inline bool clickInsidePanel(int mouseX, int mouseY) {
    return mouseX >= panelX && mouseX <= panelX + panelW && mouseY >= panelY && mouseY <= panelY + panelH;
}

//sellButtonPressed returns true if the mouse is on the sell button inside the panel
inline bool sellButtonPressed(int mouseX, int mouseY) {
    return mouseX >= sellBtnX && mouseX <= sellBtnX + sellBtnW && mouseY >= sellBtnY && mouseY <= sellBtnY + sellBtnH;
}

//drawTowerPanel shows the portrait, stats and sell button for the currently selected tower
inline void drawTowerPanel(ALLEGRO_FONT* font, ALLEGRO_BITMAP* drakeBmp, int drakeBmpW, int drakeBmpH, ALLEGRO_BITMAP* weekndBmp, int weekndBmpW, int weekndBmpH, Tower towers[], int selectedTowerIndex) {
    if (selectedTowerIndex < 0) return;
    Tower& selectedTower = towers[selectedTowerIndex];

    //background and border
    al_draw_filled_rectangle(panelX, panelY, panelX + panelW, panelY + panelH, al_map_rgba(25, 45, 25, 230));
    al_draw_rectangle(panelX, panelY, panelX + panelW, panelY + panelH, al_map_rgb(0, 0, 0), 2);

    //pick the portrait sprite to draw based on tower type
    ALLEGRO_BITMAP* portraitSprite;
    int portraitSrcW;
    int portraitSrcH;
    const char* characterName;
    if (selectedTower.type == TOWER_WEEKND) {
        portraitSprite = weekndBmp;
        portraitSrcW = weekndBmpW;
        portraitSrcH = weekndBmpH;
        characterName = "Weeknd";
    } else {
        portraitSprite = drakeBmp;
        portraitSrcW = drakeBmpW;
        portraitSrcH = drakeBmpH;
        characterName = "Drake";
    }
    //Drake's PNG has lots of empty padding so we crop it to just the character. Weeknd's PNG already fills its image
    int cropX, cropY, cropW, cropH;
    if (selectedTower.type == TOWER_DRAKE) {
        cropX = (int)(portraitSrcW * modelXFrac);
        cropY = (int)(portraitSrcH * modelYFrac);
        cropW = (int)(portraitSrcW * modelWFrac);
        cropH = (int)(portraitSrcH * modelHFrac);
    } else {
        cropX = 0;
        cropY = 0;
        cropW = portraitSrcW;
        cropH = portraitSrcH;
    }

    //fit the cropped area inside the portrait box while keeping aspect ratio so neither sprite stretches
    float scaleX = (float)portraitW / cropW;
    float scaleY = (float)portraitH / cropH;
    float fitScale = scaleX < scaleY ? scaleX : scaleY;
    int drawW = (int)(cropW * fitScale);
    int drawH = (int)(cropH * fitScale);
    int drawX = portraitX + (portraitW - drawW) / 2;
    int drawY = portraitY + (portraitH - drawH) / 2;
    al_draw_scaled_bitmap(portraitSprite, cropX, cropY, cropW, cropH, drawX, drawY, drawW, drawH, 0);

    //name above stats
    al_draw_text(font, al_map_rgb(255, 215, 0), panelX + panelW / 2, portraitY + portraitH + 8, ALLEGRO_ALIGN_CENTER, characterName);

    //damage and range stats
    char statsBuf[64];
    snprintf(statsBuf, sizeof(statsBuf), "Damage: %d", damageOf(selectedTower.type));
    al_draw_text(font, al_map_rgb(255, 255, 255), panelX + 15, portraitY + portraitH + 25, ALLEGRO_ALIGN_LEFT, statsBuf);
    snprintf(statsBuf, sizeof(statsBuf), "Range:  %d", (int)rangeOf(selectedTower.type));
    al_draw_text(font, al_map_rgb(255, 255, 255), panelX + 15, portraitY + portraitH + 40, ALLEGRO_ALIGN_LEFT, statsBuf);

    //character description - write your own text in the empty string below
    const char* description = "";
    al_draw_text(font, al_map_rgb(200, 200, 200), panelX + 15, portraitY + portraitH + 58, ALLEGRO_ALIGN_LEFT, description);

    //sell button with refund amount baked into the label
    al_draw_filled_rectangle(sellBtnX, sellBtnY, sellBtnX + sellBtnW, sellBtnY + sellBtnH, al_map_rgb(210, 50, 50));
    al_draw_rectangle(sellBtnX, sellBtnY, sellBtnX + sellBtnW, sellBtnY + sellBtnH, al_map_rgb(0, 0, 0), 2);
    char sellBuf[32];
    snprintf(sellBuf, sizeof(sellBuf), "Sell (+%d)", refundFor(selectedTower.type));
    al_draw_text(font, al_map_rgb(255, 255, 255), sellBtnX + sellBtnW / 2, sellBtnY + sellBtnH / 2 - 4, ALLEGRO_ALIGN_CENTER, sellBuf);
}

#endif
