//created by Noah Basaria and Adam Jurewicz

#ifndef TOWER_PLACEMENT_H
#define TOWER_PLACEMENT_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "tower.h"
#include "hud.h"

//checks if each variable is checked befroe placing the tower, if all checks are passed the tower is placed and gold is subtracted
inline void handleMouseClick(const ALLEGRO_EVENT& event, Tower towers[], int& towerCount, ALLEGRO_BITMAP* map, ALLEGRO_BITMAP* drakeBitmap, int drakeBitmapW, int drakeBitmapH, int weekndBitmapW, int weekndBitmapH, int elonBitmapW, int elonBitmapH, int bankBitmapW, int bankBitmapH, int &gold, bool& drakeSelected, bool& weekndSelected, bool& elonSelected, bool& bankSelected, int towerCost) {
    if (event.mouse.button != 1) return;
    if (!drakeSelected && !weekndSelected && !elonSelected && !bankSelected) return;

    //bank cap of 3 to prevent infinite money glitch and also to make the game more fun and challenging
    if (bankSelected && countBanks(towers, towerCount) >= maxBanks) return;

    //picks the footprint dimensions from whichever tower is selected
    float towerW;
    float towerH;
    if (drakeSelected) {
        towerW = drakeBitmapW * towerScale;
        towerH = drakeBitmapH * towerScale;
    }
    else if (weekndSelected) {
        towerW = weekndBitmapW * weekndScale;
        towerH = weekndBitmapH * weekndScale;
    }
    else if (elonSelected) {
        towerW = elonBitmapW * elonScale;
        towerH = elonBitmapH * elonScale;
    }
    else {
        towerW = bankBitmapW * bankScale;
        towerH = bankBitmapH * bankScale;
    }
    //code for placing the tower, it creates a new tower based on the mouse position and the selected tower type, then it checks if the tower can be placed and if it can it adds it to the towers array and subtracts gold
    Tower newTower;
    newTower.x = event.mouse.x - towerW / 2;
    newTower.y = event.mouse.y - towerH / 2;
    newTower.w = towerW;
    newTower.h = towerH;
    if (drakeSelected) newTower.type = towerDrake;
    else if (weekndSelected) newTower.type = towerWeeknd;
    else if (elonSelected) newTower.type = towerElon;
    else newTower.type = towerBank;

    Tower model = towerModelRectangle(newTower);
    bool insideScreen = model.x >= 0 && model.y >= 0 && model.x + model.w <= screenW &&model.y + model.h <= screenH;
    //checks to see if we reached the tower limit or if we have enough gold to place the tower, also check if the tower overlaps the path or any other towers, if all checks are passed we place the tower and subtract gold
    if (insideScreen && towerCount < maxTowerLimit && gold >= towerCost && !towerTouchesPath(map, newTower) && overlapsAnyTower(newTower, towers, towerCount) == false) {
    towers[towerCount++] = newTower;
    gold -= towerCost;
    }
}

//code for hover ghost tower, it picks the drake or weeknd sprite based on which button is selected and shows a green or red tint plus the tower range circle
inline void towerPlacement(ALLEGRO_BITMAP* drakeBitmap, int drakeBitmapW, int drakeBitmapH, ALLEGRO_BITMAP* weekndBitmap, int weekndBitmapW, int weekndBitmapH, ALLEGRO_BITMAP* elonBitmap, int elonBitmapW, int elonBitmapH, ALLEGRO_BITMAP* bankBitmap, int bankBitmapW, int bankBitmapH, int mouseX, int mouseY, ALLEGRO_BITMAP* map, Tower towers[], int towerCount, int gold, bool drakeSelected, bool weekndSelected, bool elonSelected, bool bankSelected) {
    if (!drakeSelected && !weekndSelected && !elonSelected && !bankSelected) return;

    ALLEGRO_BITMAP* spriteBitmap;
    int spriteW;
    int spriteH;
    float range;
    if (drakeSelected) {
        spriteBitmap = drakeBitmap;
        spriteW = drakeBitmapW;
        spriteH = drakeBitmapH;
        range = towerRange;
    }
    else if (weekndSelected) {
        spriteBitmap = weekndBitmap;
        spriteW = weekndBitmapW;
        spriteH = weekndBitmapH;
        range = weekndRange;
    }
    else if (elonSelected) {
        spriteBitmap = elonBitmap;
        spriteW = elonBitmapW;
        spriteH = elonBitmapH;
        range = elonRange;
    }
    else {
        spriteBitmap = bankBitmap;
        spriteW = bankBitmapW;
        spriteH = bankBitmapH;
        range = 0.0f;
    }

    //this is theghost footprint which uses the matching sprite's actual dimensions
    float towerW;
    float towerH;
    if (drakeSelected) {
        towerW = drakeBitmapW * towerScale;
        towerH = drakeBitmapH * towerScale;
    }
    else if (weekndSelected) {
        towerW = weekndBitmapW * weekndScale;
        towerH = weekndBitmapH * weekndScale;
    }
    else if (elonSelected) {
        towerW = elonBitmapW * elonScale;
        towerH = elonBitmapH * elonScale;
    }
    else {
        towerW = bankBitmapW * bankScale;
        towerH = bankBitmapH * bankScale;
    }
    //this is for the tower ghost so the player can see where the tower will be placed
    Tower preview;
    preview.x = mouseX - towerW / 2;
    preview.y = mouseY - towerH / 2;
    preview.w = towerW;
    preview.h = towerH;
    //the model rectangle is used for all placement checks and the circle range it uses rectangle to be more forgiving and less frustrating for the player
    Tower model = towerModelRectangle(preview);
    bool insideScreen = model.x >= 0 && model.y >= 0 && model.x + model.w <= screenW && model.y + model.h <= screenH;

    int cost;
    if (drakeSelected) cost = drakeCost;
    else if (weekndSelected) cost = weekndCost;
    else if (elonSelected) cost = elonCost;
    else cost = bankCost;
    //this canPlace variable is used to determine the color of the ghost tower and whether the player can place the tower or not, it checks all the same conditions as the handleMouseClick function
    bool canPlace = insideScreen && towerCount < maxTowerLimit && gold >= cost && !towerTouchesPath(map, preview) && !overlapsAnyTower(preview, towers, towerCount);
    //extra restriction such as banks are capped at maxBanks regardless of total tower limit
    if (bankSelected && countBanks(towers, towerCount) >= maxBanks) canPlace = false;
    float centerX = model.x + model.w * 0.5f;
    float centerY = model.y + model.h * 0.5f;
    al_draw_circle(centerX, centerY, range, al_map_rgba(120, 120, 120, 180), 2);

    //green if placement is allowed, red if not
    ALLEGRO_COLOR tint;
    if (canPlace) {
        tint = al_map_rgba(0, 180, 0, 150);
    } 
    else {
        tint = al_map_rgba(180, 0, 0, 150);
    }
    al_draw_tinted_scaled_bitmap(spriteBitmap, tint, 0, 0, spriteW, spriteH, preview.x, preview.y, preview.w, preview.h, 0);
}

//highlightTower redraws the selected tower with a grey transparent tint over the actual sprite shape
inline void highlightTower(Tower towers[], int selectedTowerIndex, ALLEGRO_BITMAP* drakeBitmap, int drakeBitmapW, int drakeBitmapH, ALLEGRO_BITMAP* weekndBitmap, int weekndBitmapW, int weekndBitmapH, ALLEGRO_BITMAP* elonBitmap, int elonBitmapW, int elonBitmapH, ALLEGRO_BITMAP* bankBitmap, int bankBitmapW, int bankBitmapH, ALLEGRO_BITMAP* icemanBitmap, int icemanBitmapW, int icemanBitmapH, ALLEGRO_BITMAP* starboyBitmap, int starboyBitmapW, int starboyBitmapH, ALLEGRO_BITMAP* teslaBitmap, int teslaBitmapW, int teslaBitmapH) {
    if (selectedTowerIndex < 0) return;
    Tower& selectedTower = towers[selectedTowerIndex];
    //
    ALLEGRO_BITMAP* towerSprite;
    int spriteWidth;
    int spriteHeight;
    if (selectedTower.type == towerWeeknd) {
        towerSprite  = weekndBitmap;
        spriteWidth  = weekndBitmapW;
        spriteHeight = weekndBitmapH;
    }
    else if (selectedTower.type == towerElon) {
        towerSprite  = elonBitmap;
        spriteWidth  = elonBitmapW;
        spriteHeight = elonBitmapH;
    }
    else if (selectedTower.type == towerBank) {
        towerSprite  = bankBitmap;
        spriteWidth  = bankBitmapW;
        spriteHeight = bankBitmapH;
    }
    else if (selectedTower.type == towerIceman) {
        towerSprite  = icemanBitmap;
        spriteWidth  = icemanBitmapW;
        spriteHeight = icemanBitmapH;
    }
    else if (selectedTower.type == towerStarboy) {
        towerSprite  = starboyBitmap;
        spriteWidth  = starboyBitmapW;
        spriteHeight = starboyBitmapH;
    }
    else if (selectedTower.type == towerTeslaMan) {
        towerSprite  = teslaBitmap;
        spriteWidth  = teslaBitmapW;
        spriteHeight = teslaBitmapH;
    }
    else {
        towerSprite  = drakeBitmap;
        spriteWidth  = drakeBitmapW;
        spriteHeight = drakeBitmapH;
    }

    ALLEGRO_COLOR grey = al_map_rgba(160, 160, 160, 200);
    al_draw_tinted_scaled_bitmap(towerSprite, grey, 0, 0, spriteWidth, spriteHeight, selectedTower.x, selectedTower.y, selectedTower.w, selectedTower.h, 0);
}

#endif
