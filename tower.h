#ifndef TOWER_H
#define TOWER_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include "enemy.h"
#include "bullet.h"
#include "hud.h"

//also adds tower limit and cost variable
const int maxTowers = 100;
const float towerScale = 0.2f;

const float modelXFrac = 0.37f;
const float modelYFrac = 0.26f;
const float modelWFrac = 0.26f;
const float modelHFrac = 0.48f;

//saves tower position it size and time to firing its next bullet
struct Tower {
    float x, y, w, h;
    int fireTimer;
};
//puts a small rectange in the middle of the tower to combat tower overlap and path collision issues
//the math is to make the rectangle smaller and centered within the tower's bounding box
//Also pretty much all of my function uses inline, this is because we have many files in this project
//inline is much more efficent then void function because without it I would need to have a seperate file for declaring the functions and then defining it.
//So basically thanks to inline we save a lot of time and effort
inline Tower getModelRect(Tower t) {
    Tower m;
    m.x = t.x + t.w * modelXFrac;
    m.y = t.y + t.h * modelYFrac;
    m.w = t.w * modelWFrac;
    m.h = t.h * modelHFrac;
    return m;
}
//This is for the bullet speed, tower range (field of view) and cooldow time between shots.
// Makes it super easy to adjust the towers in the future without changing much code.
const float towerRange = 200.0f;
const int   towerCooldown = 30;
//Saves information about the tower's cooldown and whether a bullet is alive or not
struct TowerState {
    int cooldown = 0;
};

//So this is a pretty complex function but it is very crucial:
//it first gets the tower's body center (cx, cy) from its model rectangle
//then it sets r2 = range squared (squared so we skip a slow sqrt in the loop)
//best = -1 means no target found yet; bestD starts at r2 so any candidate must be within range to beat it
//it loops through every slime, skips dead ones, and tracks the closest in-range slime, returning its index (or -1 if none qualify)
inline int findTarget(Tower t, const Slime slimes[], int slimeCount) {
    Tower m = getModelRect(t);
    float cx = m.x + m.w * 0.5f;
    float cy = m.y + m.h * 0.5f;
    float r2 = towerRange * towerRange;
    int best = -1;
    float bestD = r2;
    for (int i = 0; i < slimeCount; i++) {
        if (slimes[i].done) continue;
        float d = dist2(cx, cy, slimes[i].x, slimes[i].y);
        if (d <= bestD) { bestD = d; best = i; }
    }
    return best;
}
//Probably the most complex function in the game right now. It makes the bullets shoot but it also predicts where the slime will be when the bullet reaches it.
//It does this by solving the equation |P + V*t| = bulletSpeed * t, where P is the vector from the tower to the slime, V is the velocity of the slime, and t is the time it takes for the bullet to reach the slime. This gives us a quadratic equation in t, which we can solve using the quadratic formula. We then choose the positive solution that gives us the earliest intercept time.
//I was able to find this formula online which helped tremedously as before the bullet would miss the slimes a lot
inline void updateTowers(Tower towers[], TowerState states[], int towerCount, Slime slimes[], int slimeCount, Bullet bullets[], int* bulletCount) {
    for (int i = 0; i < towerCount; i++) { //this part checks for the cooldown and uses the previous function to find a target slime for the tower
        if (states[i].cooldown > 0) { states[i].cooldown--; continue; }
        int idx = findTarget(towers[i], slimes, slimeCount);
        if (idx < 0) continue;
        //finds towers center
        Tower m = getModelRect(towers[i]);
        float cx = m.x + m.w * 0.5f;
        float cy = m.y + m.h * 0.5f;

        // The prediction math right here it setups a quadratic equation but since its a computer it looks much more complex than it actually is.
        float px = slimes[idx].x - cx;
        float py = slimes[idx].y - cy;
        float vx = slimes[idx].vx;
        float vy = slimes[idx].vy;
        float a = vx*vx + vy*vy - bulletSpeed * bulletSpeed;
        float b_ = 2.0f * (px*vx + py*vy);
        float c  = px*px + py*py;
        float aimX = slimes[idx].x;
        float aimY = slimes[idx].y;
        if (fabsf(a) < 0.0001f) { //this is the second case where the slime at same speed as the bullet, the equation becomes linear
            if (fabsf(b_) > 0.0001f) { // We needed to add this equation because if the movement was equal to zero the program would crash
                float t = -c / b_;
                if (t > 0) { aimX = slimes[idx].x + vx * t; aimY = slimes[idx].y + vy * t; }
            }
        } else { //this actually solves the quadratic equation
            float disc = b_*b_ - 4*a*c;
            if (disc >= 0) {
                float sq = sqrtf(disc);
                float t1 = (-b_ - sq) / (2*a);
                float t2 = (-b_ + sq) / (2*a);
                float t = -1;
                if (t1 > 0 && t2 > 0) t = (t1 < t2) ? t1 : t2;
                else if (t1 > 0) t = t1;
                else if (t2 > 0) t = t2;
                if (t > 0) { aimX = slimes[idx].x + vx * t; aimY = slimes[idx].y + vy * t; }
            }
        }
        //this calculates the direction from the tower to predicted position and creates a bullet with that velocity
        float dx = aimX - cx;
        float dy = aimY - cy;
        float len = sqrtf(dx*dx + dy*dy);
        if (len < 0.001f) len = 1.0f;

        Bullet bul;
        bul.x = cx; bul.y = cy;
        bul.vx = (dx/len) * bulletSpeed;
        bul.vy = (dy/len) * bulletSpeed;
        bul.damage = 1;
        bul.alive = true;
        if (*bulletCount < maxBullets) bullets[(*bulletCount)++] = bul;

        states[i].cooldown = towerCooldown;
    }
}
//this onPath code for placing a tower
//it checks for the color of the pixel where the cursor is and if the color is close to the path color (red > 160, green > 120, blue < 120)
//the main point of it is to prevent the player from placing the tower on the path
//It probably would be better longterm to have a different bitmap just for the path but this works well enough :)
inline bool onPath(ALLEGRO_BITMAP* map, int mouseX, int mouseY) {
    ALLEGRO_COLOR color = al_get_pixel(map, mouseX, mouseY);
    unsigned char r, g, b;
    al_unmap_rgb(color, &r, &g, &b);
    return (r > 160 && g > 120 && b < 120);
}
//this is another function made for tower placement, this time its for making sure you cant place a tower on top of another tower
//funny thing is that the hitbox is not the drake itself it is a smaller rectangle in the middle of the tower
//the reasion for this is because the png of drake is bigger than what it looks like, so the rectangle is there to make it more accurate
inline bool towersOverlap(Tower a, Tower b) {
    Tower ma = getModelRect(a);
    Tower mb = getModelRect(b);
    return ma.x < mb.x + mb.w &&
           ma.x + ma.w > mb.x &&
           ma.y < mb.y + mb.h &&
           ma.y + ma.h > mb.y;
}
//this function just loops through every tower and checks if the new tower overlaps with any of them, if it does it returns true and if it doesnt it returns false
inline bool overlapsAnyTower(Tower newTower, Tower towers[], int towerCount) {
    for (int i = 0; i < towerCount; i++)
        if (towersOverlap(newTower, towers[i])) return true;
    return false;
}
//This is the code that checks if the tower touches the path
// it returns true if any parts of the towers rectangle touches the path
inline bool towerTouchesPath(ALLEGRO_BITMAP* map, Tower tower) {
    Tower model = getModelRect(tower);

    int mapW = al_get_bitmap_width(map);
    int mapH = al_get_bitmap_height(map);
    //this find the egdes of the search box which is the towers rectangle
    int left = (int)model.x;
    int right = (int)(model.x + model.w);
    int top = (int)model.y;
    int bottom = (int)(model.y + model.h);
    //this is a safety net for the search box it basically makes sure that the tower is not partly off the screen
    //before this if we placed a tower on the edge of the screen the program would just crash so this solves the issue lol
    if (left < 0) left = 0;
    if (top < 0) top = 0;
    if (right >= mapW) right = mapW - 1;
    if (bottom >= mapH) bottom = mapH - 1;

    bool touches = false; //boolean variaraible to check if the tower touches the path
    al_lock_bitmap(map, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

    for (int y = top; y <= bottom && !touches; y += 2)
        for (int x = left; x <= right; x += 2)
            if (onPath(map, x, y)) touches = true;

    al_unlock_bitmap(map);
    return touches;
}
//places a new tower at the mouse position if all placement rules pass
inline void handleMouseClick(const ALLEGRO_EVENT& event,
                              Tower towers[], int& towerCount,
                              ALLEGRO_BITMAP* map,
                              ALLEGRO_BITMAP* towerBmp,
                              int towerBmpW, int towerBmpH, int &gold) {
    if (event.mouse.button != 1) return;

    float towerW = towerBmpW * towerScale;
    float towerH = towerBmpH * towerScale;

    Tower newTower;
    newTower.x = event.mouse.x - towerW / 2;
    newTower.y = event.mouse.y - towerH / 2;
    newTower.w = towerW;
    newTower.h = towerH;

    Tower model = getModelRect(newTower);
    bool insideScreen =
        model.x >= 0 &&
        model.y >= 0 &&
        model.x + model.w <= screenW &&
        model.y + model.h <= screenH;

    if (insideScreen && towerCount < maxTowerLimit && gold >= towerCost && !towerTouchesPath(map, newTower) && !overlapsAnyTower(newTower, towers, towerCount)) {
    towers[towerCount++] = newTower;
    gold -= towerCost;
    }
}

#endif
