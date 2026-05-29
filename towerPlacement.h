#ifndef TOWER_PLACEMENT_H
#define TOWER_PLACEMENT_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "tower.h"
#include "hud.h"

//checks if each variable is checked befroe placing the tower, if all checks are passed the tower is placed and gold is subtracted
inline void handleMouseClick(const ALLEGRO_EVENT& event, Tower towers[], int& towerCount, ALLEGRO_BITMAP* map, ALLEGRO_BITMAP* drakeBmp, int drakeBmpW, int drakeBmpH, int weekndBmpW, int weekndBmpH, int &gold, bool& drakeSelected, bool& weekndSelected, int towerCost) {
    if (event.mouse.button != 1) return;
    if (!drakeSelected && !weekndSelected) return;

    //here you can see the code for the ghost tower and range circle
    float towerW;
    float towerH;
    if (drakeSelected) {
        towerW = drakeBmpW * towerScale;
        towerH = drakeBmpH * towerScale;
    } else {
        towerW = weekndBmpW * weekndScale;
        towerH = weekndBmpH * weekndScale;
    }
    //code for placing the tower, it creates a new tower based on the mouse position and the selected tower type, then it checks if the tower can be placed and if it can it adds it to the towers array and subtracts gold
    Tower newTower;
    newTower.x = event.mouse.x - towerW / 2;
    newTower.y = event.mouse.y - towerH / 2;
    newTower.w = towerW;
    newTower.h = towerH;
    if (drakeSelected) {
        newTower.type = TOWER_DRAKE;
    } else {
        newTower.type = TOWER_WEEKND;
    }

    Tower model = towerModelRectangle(newTower);
    bool insideScreen = model.x >= 0 && model.y >= 0 && model.x + model.w <= screenW &&model.y + model.h <= screenH;
    //check to see if we reached the tower limit or if we have enough gold to place the tower, also check if the tower overlaps the path or any other towers, if all checks are passed we place the tower and subtract gold
    if (insideScreen && towerCount < maxTowerLimit && gold >= towerCost && !towerTouchesPath(map, newTower) && overlapsAnyTower(newTower, towers, towerCount) == false) {
    towers[towerCount++] = newTower;
    gold -= towerCost;
    }
}

//code for hover ghost tower, it picks the drake or weeknd sprite based on which button is selected and shows a green or red tint plus the tower range circle
inline void towerPlacement(ALLEGRO_BITMAP* drakeBmp, int drakeBmpW, int drakeBmpH, ALLEGRO_BITMAP* weekndBmp, int weekndBmpW, int weekndBmpH, int mouseX, int mouseY, ALLEGRO_BITMAP* map, Tower towers[], int towerCount, int gold, bool drakeSelected, bool weekndSelected) {
    if (!drakeSelected && !weekndSelected) return;

    ALLEGRO_BITMAP* spriteBmp;
    int spriteW;
    int spriteH;
    float range;
    if (drakeSelected) {
        spriteBmp = drakeBmp;
        spriteW = drakeBmpW;
        spriteH = drakeBmpH;
        range = towerRange;
    } else {
        spriteBmp = weekndBmp;
        spriteW = weekndBmpW;
        spriteH = weekndBmpH;
        range = weekndRange;
    }

    //ghost footprint uses the matching sprite's actual dimensions
    float towerW;
    float towerH;
    if (drakeSelected) {
        towerW = drakeBmpW * towerScale;
        towerH = drakeBmpH * towerScale;
    } else {
        towerW = weekndBmpW * weekndScale;
        towerH = weekndBmpH * weekndScale;
    }
    Tower preview;
    preview.x = mouseX - towerW / 2;
    preview.y = mouseY - towerH / 2;
    preview.w = towerW;
    preview.h = towerH;
    //the model rectangle is used for all placement checks and the circle range it uses rectangle to be more forgiving and less frustrating for the player
    Tower model = towerModelRectangle(preview);
    bool insideScreen = model.x >= 0 && model.y >= 0 && model.x + model.w <= screenW && model.y + model.h <= screenH;

    int cost;
    if (drakeSelected) {
        cost = drakeCost;
    } else {
        cost = weekndCost;
    }
    //this canPlace variable is used to determine the color of the ghost tower and whether the player can place the tower or not, it checks all the same conditions as the handleMouseClick function
    bool canPlace = insideScreen && towerCount < maxTowerLimit && gold >= cost && !towerTouchesPath(map, preview) && !overlapsAnyTower(preview, towers, towerCount);
    float centerX = model.x + model.w * 0.5f;
    float centerY = model.y + model.h * 0.5f;
    al_draw_circle(centerX, centerY, range, al_map_rgba(120, 120, 120, 180), 2);

    //green if placement is allowed, red if not
    ALLEGRO_COLOR tint;
    if (canPlace) {
        tint = al_map_rgba(0, 180, 0, 150);
    } else {
        tint = al_map_rgba(180, 0, 0, 150);
    }
    al_draw_tinted_scaled_bitmap(spriteBmp, tint, 0, 0, spriteW, spriteH, preview.x, preview.y, preview.w, preview.h, 0);
}

//highlightTower redraws the selected tower with a grey transparent tint over the actual sprite shape
inline void highlightTower(Tower towers[], int selectedTowerIndex, ALLEGRO_BITMAP* drakeBmp, int drakeBmpW, int drakeBmpH, ALLEGRO_BITMAP* weekndBmp, int weekndBmpW, int weekndBmpH) {
    if (selectedTowerIndex < 0) return;
    Tower& selectedTower = towers[selectedTowerIndex];

    ALLEGRO_BITMAP* towerSprite;
    int spriteWidth;
    int spriteHeight;
    if (selectedTower.type == TOWER_WEEKND) {
        towerSprite  = weekndBmp;
        spriteWidth  = weekndBmpW;
        spriteHeight = weekndBmpH;
    } else {
        towerSprite  = drakeBmp;
        spriteWidth  = drakeBmpW;
        spriteHeight = drakeBmpH;
    }

    ALLEGRO_COLOR grey = al_map_rgba(160, 160, 160, 200);
    al_draw_tinted_scaled_bitmap(towerSprite, grey, 0, 0, spriteWidth, spriteHeight, selectedTower.x, selectedTower.y, selectedTower.w, selectedTower.h, 0);
}

#endif
