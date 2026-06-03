#ifndef PANEL_H
#define PANEL_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <cstdio>
#include "hud.h"
#include "tower.h"

//panel layout sits on the left side under the hud
const int panelX = 30;
const int panelY = hudHeight + 5;
const int panelW = 245;
const int panelH = 370;

const int portraitX = panelX + 30;
const int portraitY = panelY + 12;
const int portraitW = 180;
const int portraitH = 150;

//two upgrade buttons stacked below the stats. Banks don't show these.
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
    if (type == towerIceman) return (int)((drakeCost + drakeUpgradeCost) * 0.7f);     //iceman refund includes the upgrade gold spent
    if (type == towerStarboy) return (int)((weekndCost + weekndUpgradeCost) * 0.7f);  //starboy refund includes the upgrade gold spent
    if (type == towerTeslaMan) return (int)((elonCost + elonUpgradeCost) * 0.7f);     //tesla refund includes the upgrade gold spent
    return (int)(drakeCost * 0.7f);
}


//clickInsidePanel is here so that if you click the panel it wont click the tower behind the panel

inline bool clickInsidePanel(int mouseX, int mouseY) {
    return mouseX >= panelX && mouseX <= panelX + panelW && mouseY >= panelY && mouseY <= panelY + panelH;
}

//sellButtonPressed sells the tower
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
inline void drawTowerPanel(ALLEGRO_FONT* font, ALLEGRO_BITMAP* drakeBmp, int drakeBmpW, int drakeBmpH, ALLEGRO_BITMAP* weekndBmp, int weekndBmpW, int weekndBmpH, ALLEGRO_BITMAP* elonBmp, int elonBmpW, int elonBmpH, ALLEGRO_BITMAP* bankBmp, int bankBmpW, int bankBmpH, ALLEGRO_BITMAP* icemanBmp, int icemanBmpW, int icemanBmpH, ALLEGRO_BITMAP* starboyBmp, int starboyBmpW, int starboyBmpH, ALLEGRO_BITMAP* teslaBmp, int teslaBmpW, int teslaBmpH, Tower towers[], int selectedTowerIndex) {
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
        portraitSprite = weekndBmp;
        portraitSrcW = weekndBmpW;
        portraitSrcH = weekndBmpH;
        characterName = "Weeknd";
    } else if (selectedTower.type == towerElon) {
        portraitSprite = elonBmp;
        portraitSrcW = elonBmpW;
        portraitSrcH = elonBmpH;
        characterName = "Elon";
    } else if (selectedTower.type == towerBank) {
        portraitSprite = bankBmp;
        portraitSrcW = bankBmpW;
        portraitSrcH = bankBmpH;
        characterName = "Bank";
    } else if (selectedTower.type == towerIceman) {
        portraitSprite = icemanBmp;
        portraitSrcW = icemanBmpW;
        portraitSrcH = icemanBmpH;
        characterName = "Iceman";
    } else if (selectedTower.type == towerStarboy) {
        portraitSprite = starboyBmp;
        portraitSrcW = starboyBmpW;
        portraitSrcH = starboyBmpH;
        characterName = "Starboy";
    } else if (selectedTower.type == towerTeslaMan) {
        portraitSprite = teslaBmp;
        portraitSrcW = teslaBmpW;
        portraitSrcH = teslaBmpH;
        characterName = "Tesla Man";
    } else {
        portraitSprite = drakeBmp;
        portraitSrcW = drakeBmpW;
        portraitSrcH = drakeBmpH;
        characterName = "Drake";
    }
    //all the new pixel-art PNGs fill their canvas, so just use the whole image
    int cropX = 0;
    int cropY = 0;
    int cropW = portraitSrcW;
    int cropH = portraitSrcH;

    //fit the cropped area inside the portrait box while keeping aspect ratio so neither sprite stretches or gets cut off
    float scaleX = (float)portraitW / cropW;
    float scaleY = (float)portraitH / cropH;
    float finalScale = scaleX < scaleY ? scaleX : scaleY;
    //the new pixel-art PNGs fill their canvas, so the plain fit-to-box scale already fills the portrait nicely
    int drawW = (int)(cropW * finalScale);
    int drawH = (int)(cropH * finalScale);
    int drawX = portraitX + (portraitW - drawW) / 2;
    int drawY = portraitY + (portraitH - drawH) / 2;
    al_draw_scaled_bitmap(portraitSprite, cropX, cropY, cropW, cropH, drawX, drawY, drawW, drawH, 0);

    //name above stats
    al_draw_text(font, al_map_rgb(255, 215, 0), panelX + panelW / 2, portraitY + portraitH + 8, ALLEGRO_ALIGN_CENTER, characterName);

    //banks dont get stats or upgrade buttons - they only have the sell button
    if (selectedTower.type != towerBank) {
        //damage and range stats - damage includes the upgrade2 bonus
        char statsBuf[64];
        int effectiveDamage = damageOf(selectedTower.type) + selectedTower.damageUpgradeLevel;
        snprintf(statsBuf, sizeof(statsBuf), "Damage: %d", effectiveDamage);
        al_draw_text(font, al_map_rgb(255, 255, 255), panelX + 15, portraitY + portraitH + 25, ALLEGRO_ALIGN_LEFT, statsBuf);
        snprintf(statsBuf, sizeof(statsBuf), "Range:  %d", (int)rangeOf(selectedTower.type));
        al_draw_text(font, al_map_rgb(255, 255, 255), panelX + 15, portraitY + portraitH + 40, ALLEGRO_ALIGN_LEFT, statsBuf);

        //upgrade 1 - drake into iceman. Other towers get a "---" placeholder for now
        char upg1Label[32] = "---";
        ALLEGRO_COLOR upg1Fill = al_map_rgb(70, 70, 70);
        if (selectedTower.type == towerDrake) {
            snprintf(upg1Label, sizeof(upg1Label), "ICEMAN (%d)", drakeUpgradeCost);
            upg1Fill = al_map_rgb(60, 100, 160);
        } else if (selectedTower.type == towerWeeknd) {
            snprintf(upg1Label, sizeof(upg1Label), "STARBOY (%d)", weekndUpgradeCost);
            upg1Fill = al_map_rgb(140, 60, 60);
        } else if (selectedTower.type == towerElon) {
            snprintf(upg1Label, sizeof(upg1Label), "TESLA MAN (%d)", elonUpgradeCost);
            upg1Fill = al_map_rgb(120, 90, 40);
        } else if (selectedTower.type == towerIceman || selectedTower.type == towerStarboy || selectedTower.type == towerTeslaMan) {
            snprintf(upg1Label, sizeof(upg1Label), "MAXED");
            upg1Fill = al_map_rgb(50, 100, 50);
        }
        al_draw_filled_rectangle(upgrade1BtnX, upgrade1BtnY, upgrade1BtnX + upgrade1BtnW, upgrade1BtnY + upgrade1BtnH, upg1Fill);
        al_draw_rectangle(upgrade1BtnX, upgrade1BtnY, upgrade1BtnX + upgrade1BtnW, upgrade1BtnY + upgrade1BtnH, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(255, 255, 255), upgrade1BtnX + upgrade1BtnW / 2, upgrade1BtnY + upgrade1BtnH / 2 - 4, ALLEGRO_ALIGN_CENTER, upg1Label);

        //upgrade 2 - +1 damage per click, cost starts at 400 and goes up by 100 each level
        int upg2Cost = 400 + 100 * selectedTower.damageUpgradeLevel;
        char upg2Label[32];
        snprintf(upg2Label, sizeof(upg2Label), "+1 Damage (%d)", upg2Cost);
        al_draw_filled_rectangle(upgrade2BtnX, upgrade2BtnY, upgrade2BtnX + upgrade2BtnW, upgrade2BtnY + upgrade2BtnH, al_map_rgb(160, 100, 60));
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
