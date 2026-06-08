#ifndef PANEL_H //this code is for the sperate panel hud where you upgrade the towers and sell them
#define PANEL_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <cstdio>
#include "hud.h"
#include "tower.h"

//this makes the panel appear on the left top corner under the hud
const int panelX = 30;
const int panelY = hudHeight + 5;
const int panelW = 245;
const int panelH = 370;

const int portraitX = panelX + 30;
const int portraitY = panelY + 12;
const int portraitW = 180;
const int portraitH = 150;

//two upgrade buttons stacked below the stats. Banks don't show these as they dont need any upgrades
const int upgrade1BtnX = panelX + 30;
const int upgrade1BtnY = panelY + 222;
const int upgrade1BtnW = 180;
const int upgrade1BtnH = 38;

const int upgrade2BtnX = panelX + 30;
const int upgrade2BtnY = panelY + 264;
const int upgrade2BtnW = 180;
const int upgrade2BtnH = 38;

const int sellBtnX = panelX + 30;
const int sellBtnY = panelY + panelH - 60;
const int sellBtnW = 180;
const int sellBtnH = 45;

//refundFor returns the gold given back when selling a tower of the given type (70% of cost)
inline int refundFor(int type) {
    if (type == towerWeeknd) return (int)(weekndCost * 0.7f);
    if (type == towerElon)   return (int)(elonCost * 0.7f);
    if (type == towerBank)   return (int)(bankCost * 0.7f);
    if (type == towerIceman) return (int)((drakeCost + drakeUpgradeCost) * 0.7f);     //iceman refund includes the iceman price and drake so the player doesnt get scammed
    if (type == towerStarboy) return (int)((weekndCost + weekndUpgradeCost) * 0.7f);  //Starboy refund includes the starboy price and weeknd so the player doesnt get scammed
    if (type == towerTeslaMan) return (int)((elonCost + elonUpgradeCost) * 0.7f);     //Tesla Man refund includes the TeslaMan price and elon  so the player doesnt get scammed
    return (int)(drakeCost * 0.7f);
}


//clickInsidePanel is here so that if you click the panel it wont click the tower behind the panel

inline bool clickInsidePanel(int mouseX, int mouseY) {
    return mouseX >= panelX && mouseX <= panelX + panelW && mouseY >= panelY && mouseY <= panelY + panelH;
}

//bool forsellButtonPressed that sells the tower works same as all the other panels
inline bool sellButtonPressed(int mouseX, int mouseY) {
    return mouseX >= sellBtnX && mouseX <= sellBtnX + sellBtnW && mouseY >= sellBtnY && mouseY <= sellBtnY + sellBtnH;
}
inline bool upgrade1ButtonPressed(int mouseX, int mouseY) {
    return mouseX >= upgrade1BtnX && mouseX <= upgrade1BtnX + upgrade1BtnW && mouseY >= upgrade1BtnY && mouseY <= upgrade1BtnY + upgrade1BtnH;
}
inline bool upgrade2ButtonPressed(int mouseX, int mouseY) {
    return mouseX >= upgrade2BtnX && mouseX <= upgrade2BtnX + upgrade2BtnW && mouseY >= upgrade2BtnY && mouseY <= upgrade2BtnY + upgrade2BtnH;
}

//drawTowerPanel shows the portrait, stats and sell button for the currently selected tower
inline void drawTowerPanel(ALLEGRO_FONT* font, ALLEGRO_BITMAP* drakeBitmap, int drakeBitmapW, int drakeBitmapH, ALLEGRO_BITMAP* weekndBitmap, int weekndBitmapW, int weekndBitmapH, ALLEGRO_BITMAP* elonBitmap, int elonBitmapW, int elonBitmapH, ALLEGRO_BITMAP* bankBitmap, int bankBitmapW, int bankBitmapH, ALLEGRO_BITMAP* icemanBitmap, int icemanBitmapW, int icemanBitmapH, ALLEGRO_BITMAP* starboyBitmap, int starboyBitmapW, int starboyBitmapH, ALLEGRO_BITMAP* teslaBitmap, int teslaBitmapW, int teslaBitmapH, Tower towers[], int selectedTowerIndex) {
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
    if (selectedTower.type == towerWeeknd) {
        portraitSprite = weekndBitmap;
        portraitSrcW = weekndBitmapW;
        portraitSrcH = weekndBitmapH;
        characterName = "Weeknd";
    }
    else if (selectedTower.type == towerElon) {
        portraitSprite = elonBitmap;
        portraitSrcW = elonBitmapW;
        portraitSrcH = elonBitmapH;
        characterName = "Elon";
    }
    else if (selectedTower.type == towerBank) {
        portraitSprite = bankBitmap;
        portraitSrcW = bankBitmapW;
        portraitSrcH = bankBitmapH;
        characterName = "Bank";
    }
    else if (selectedTower.type == towerIceman) {
        portraitSprite = icemanBitmap;
        portraitSrcW = icemanBitmapW;
        portraitSrcH = icemanBitmapH;
        characterName = "Iceman";
    }
    else if (selectedTower.type == towerStarboy) {
        portraitSprite = starboyBitmap;
        portraitSrcW = starboyBitmapW;
        portraitSrcH = starboyBitmapH;
        characterName = "Starboy";
    }
    else if (selectedTower.type == towerTeslaMan) {
        portraitSprite = teslaBitmap;
        portraitSrcW = teslaBitmapW;
        portraitSrcH = teslaBitmapH;
        characterName = "Tesla Man";
    }
    else {
        portraitSprite = drakeBitmap;
        portraitSrcW = drakeBitmapW;
        portraitSrcH = drakeBitmapH;
        characterName = "Drake";
    }
    //crops the images
    int cropX = 0;
    int cropY = 0;
    int cropW = portraitSrcW;
    int cropH = portraitSrcH;

    //fit the cropped area inside the portrait box while keeping aspect ratio so the sprite 
    float scaleX = (float)portraitW / cropW;
    float scaleY = (float)portraitH / cropH;
    float finalScale;
    if (scaleX < scaleY) {
        finalScale = scaleX;
    }
    else {
        finalScale = scaleY;
    }
    //drake and iceman art sits small inside its canvas so we zoom their portraits up a bit
    if (selectedTower.type == towerDrake || selectedTower.type == towerIceman) finalScale *= 1.5f;
    int drawW = (int)(cropW * finalScale);
    int drawH = (int)(cropH * finalScale);
    int drawX = portraitX + (portraitW - drawW) / 2;
    int drawY = portraitY + (portraitH - drawH) / 2;
    al_draw_scaled_bitmap(portraitSprite, cropX, cropY, cropW, cropH, drawX, drawY, drawW, drawH, 0);

    //name above stats
    al_draw_text(font, al_map_rgb(255, 215, 0), panelX + panelW / 2, portraitY + portraitH + 8, ALLEGRO_ALIGN_CENTER, characterName);

    //banks dont get stats or upgrade buttons so they only have the sell button
    if (selectedTower.type != towerBank) {
        //damage and range stats aswelss damage includes the upgrade2 bonus
        //https://www.programiz.com/cpp-programming/library-function/cstdio/snprintf
        //https://en.cppreference.com/cpp/io/c/fprintf
        char statsBuf[64];
        int effectiveDamage = damageOf(selectedTower.type) + selectedTower.damageUpgradeLevel;
        snprintf(statsBuf, sizeof(statsBuf), "Damage: %d", effectiveDamage);
        al_draw_text(font, al_map_rgb(255, 255, 255), panelX + 15, portraitY + portraitH + 25, ALLEGRO_ALIGN_LEFT, statsBuf);
        snprintf(statsBuf, sizeof(statsBuf), "Range:  %d", (int)rangeOf(selectedTower.type));
        al_draw_text(font, al_map_rgb(255, 255, 255), panelX + 15, portraitY + portraitH + 40, ALLEGRO_ALIGN_LEFT, statsBuf);

        //upgrade 1 turns drake into iceman. Other towers get a "---" placeholder for now
        char upg1Label[32] = "---";
        ALLEGRO_COLOR upg1Fill = al_map_rgb(70, 70, 70);
        if (selectedTower.type == towerDrake) {
            snprintf(upg1Label, sizeof(upg1Label), "ICEMAN (%d)", drakeUpgradeCost);
            upg1Fill = al_map_rgb(60, 100, 160);
        }
    else if (selectedTower.type == towerWeeknd) {
            snprintf(upg1Label, sizeof(upg1Label), "STARBOY (%d)", weekndUpgradeCost);
            upg1Fill = al_map_rgb(140, 60, 60);
        }
    else if (selectedTower.type == towerElon) {
            snprintf(upg1Label, sizeof(upg1Label), "TESLA MAN (%d)", elonUpgradeCost);
            upg1Fill = al_map_rgb(120, 90, 40);
        }
    else if (selectedTower.type == towerIceman || selectedTower.type == towerStarboy || selectedTower.type == towerTeslaMan) {
            snprintf(upg1Label, sizeof(upg1Label), "MAXED");
            upg1Fill = al_map_rgb(50, 100, 50);
        }
        al_draw_filled_rectangle(upgrade1BtnX, upgrade1BtnY, upgrade1BtnX + upgrade1BtnW, upgrade1BtnY + upgrade1BtnH, upg1Fill);
        al_draw_rectangle(upgrade1BtnX, upgrade1BtnY, upgrade1BtnX + upgrade1BtnW, upgrade1BtnY + upgrade1BtnH, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(255, 255, 255), upgrade1BtnX + upgrade1BtnW / 2, upgrade1BtnY + upgrade1BtnH / 2 - 4, ALLEGRO_ALIGN_CENTER, upg1Label);

        //upgrade 2 gives +1 damage AND faster firing, cost starts at 300 and goes up by 100 each level
        bool upg2Maxed = selectedTower.damageUpgradeLevel >= maxDamageUpgradeLevel;
        int upg2Cost = 300 + 100 * selectedTower.damageUpgradeLevel;
        char upg2Label[32];
        if (upg2Maxed) snprintf(upg2Label, sizeof(upg2Label), "MAXED");
        else snprintf(upg2Label, sizeof(upg2Label), "+1 Damage (%d)", upg2Cost);
        ALLEGRO_COLOR upg2Fill;
        if (upg2Maxed) { upg2Fill = al_map_rgb(50, 100, 50); } else { upg2Fill = al_map_rgb(160, 100, 60); }
        al_draw_filled_rectangle(upgrade2BtnX, upgrade2BtnY, upgrade2BtnX + upgrade2BtnW, upgrade2BtnY + upgrade2BtnH, upg2Fill);
        al_draw_rectangle(upgrade2BtnX, upgrade2BtnY, upgrade2BtnX + upgrade2BtnW, upgrade2BtnY + upgrade2BtnH, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(255, 255, 255), upgrade2BtnX + upgrade2BtnW / 2, upgrade2BtnY + upgrade2BtnH / 2 - 4, ALLEGRO_ALIGN_CENTER, upg2Label);
    }

    //sell button with refund amount 70% of original cost
    al_draw_filled_rectangle(sellBtnX, sellBtnY, sellBtnX + sellBtnW, sellBtnY + sellBtnH, al_map_rgb(210, 50, 50));
    al_draw_rectangle(sellBtnX, sellBtnY, sellBtnX + sellBtnW, sellBtnY + sellBtnH, al_map_rgb(0, 0, 0), 2);
    char sellBuf[32];
    snprintf(sellBuf, sizeof(sellBuf), "Sell (+%d)", refundFor(selectedTower.type));
    al_draw_text(font, al_map_rgb(255, 255, 255), sellBtnX + sellBtnW / 2, sellBtnY + sellBtnH / 2 - 4, ALLEGRO_ALIGN_CENTER, sellBuf);
}

#endif
