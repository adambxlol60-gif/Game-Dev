//created by Noah Basaria and Adam Jurewicz

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
const float weekndScale = 0.11f;

const float modelXFrac = 0.37f;
const float modelYFrac = 0.26f;
const float modelWFrac = 0.26f;
const float modelHFrac = 0.48f;

//tower type ids - kept as plain ints so we can branch on them with if/else
const int towerDrake = 0;
const int towerWeeknd = 1;

//saves tower position, size, fire timer, and which type it is
struct Tower {
    float x, y, w, h;
    int fireTimer;
    int type;
};
//puts a small rectange in the middle of the tower to combat tower overlap and path collision issues
//the math is to make the rectangle smaller and centered within the tower's bounding box
//Also pretty much all of my function uses inline, this is because we have many files in this project
//inline is much more efficent then void function because without it I would need to have a seperate file for declaring the functions and then defining it.
//So basically thanks to inline we save a lot of time and effort
inline Tower towerModelRectangle(Tower t) {
    Tower model;
    model.x = t.x + t.w * modelXFrac;
    model.y = t.y + t.h * modelYFrac;
    model.w = t.w * modelWFrac;
    model.h = t.h * modelHFrac;
    return model;
}
//This is for the bullet speed, tower range (field of view) and cooldow time between shots.
// Makes it super easy to adjust the towers in the future without changing much code.
const float towerRange = 200.0f;
const float weekndRange = 350.0f;
const int   towerCooldown = 30;

//rangeOf returns the firing range for a given tower type
inline float rangeOf(int towerType) {
    if (towerType == towerWeeknd) return weekndRange;
    return towerRange;
}

const int drakeDamage = 1;
const int weekndDamage = 2;
const int drakePierce = 0;
const int weekndPierce = 1;
const int drakeIce = 1;
const int weekndIce = 0;

//damageOf returns how much damage a bullet from this tower type deals
inline int damageOf(int towerType) {
    if (towerType == towerWeeknd) return weekndDamage;
    return drakeDamage;
}

//pierceOf returns how many extra slimes a bullet from this tower type can hit before dying
inline int pierceOf(int towerType) {
    if (towerType == towerWeeknd) return weekndPierce;
    return drakePierce;
}
inline int iceOf(int towerType) {
    if (towerType == towerWeeknd) return weekndIce;
    return drakeIce;
}
//Saves information about the tower's cooldown and whether a bullet is alive or not
struct TowerState {
    int cooldown = 0;
};

//So this is a pretty complex function but it is very crucial:
//it first gets the tower's body center (cx, cy) from its model rectangle
//then it sets r2 = range squared (squared so we skip a slow sqrt in the loop)
//best = -1 means no target found yet; bestD starts at r2 so any candidate must be within range to beat it
//it loops through every slime, skips dead ones, and tracks the closest in-range slime, returning its index (or -1 if none qualify)
inline int findTarget(Tower t, const Slime slimes[], int slimeCount, float range) {
    Tower model = towerModelRectangle(t);
    float centerX = model.x + model.w * 0.5f;
    float centerY = model.y + model.h * 0.5f;
    float r2 = range * range;
    int best = -1;
    float bestD = r2;
    for (int i = 0; i < slimeCount; i++) {
        if (slimes[i].done) continue;
        float distance = dist2(centerX, centerY, slimes[i].x, slimes[i].y); //used dist2 instead of sqrt to make the code run faster 
        if (distance <= bestD) { 
            bestD = distance; best = i; }
    }
    return best;
}
//Probably the most complex function in the game right now. It makes the bullets shoot but it also predicts where the slime will be when the bullet reaches it.
//It does this by solving the equation |P + V*t| = bulletSpeed * t, where P is the vector from the tower to the slime, V is the velocity of the slime, and t is the time it takes for the bullet to reach the slime. This gives us a quadratic equation in t, which we can solve using the quadratic formula. We then choose the positive solution that gives us the earliest intercept time.
//I was able to find this formula which helped tremedously as before the bullet would miss the slimes a lot
inline void updateTowers(Tower towers[], TowerState states[], int towerCount, Slime slimes[], int slimeCount, Bullet bullets[], int* bulletCount, ALLEGRO_BITMAP* microphoneBmp, ALLEGRO_BITMAP* drakeMicBmp) {
    for (int i = 0; i < towerCount; i++) { //this part checks for the cooldown and uses the previous function to find a target slime for the tower
        if (states[i].cooldown > 0) { states[i].cooldown--; continue; }
        float range = rangeOf(towers[i].type);
        int idx = findTarget(towers[i], slimes, slimeCount, range);
        if (idx < 0) continue;
        //finds towers center
        Tower model = towerModelRectangle(towers[i]);
        float centerX = model.x + model.w * 0.5f;
        float centerY = model.y + model.h * 0.5f;

        // The prediction math right here it setups a quadratic equation but since its a computer it looks much more complex than it actually is.
        float px = slimes[idx].x - centerX;
        float py = slimes[idx].y - centerY;
        float vx = slimes[idx].vx;
        float vy = slimes[idx].vy;
        float a = vx*vx + vy*vy - bulletSpeed * bulletSpeed;
        float b_ = 2.0f * (px*vx + py*vy); //b is discrimant 
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
        float dx = aimX - centerX;
        float dy = aimY - centerY;
        float len = sqrtf(dx*dx + dy*dy); //pyothegorean theorem to find the length of the vector, thanks Pythagoras
        if (len < 0.001f) len = 1.0f; //safety check that prevents dividing by zero
            // struct for bullets, gives it properties like speed, damage, pierce, if its alive ex. Also does hit boxchecking
        Bullet bullet;
        bullet.x = centerX; bullet.y = centerY;
        bullet.vx = (dx/len) * bulletSpeed;
        bullet.vy = (dy/len) * bulletSpeed;
        bullet.damage = damageOf(towers[i].type);
        bullet.pierce = pierceOf(towers[i].type);
        bullet.alive = true;
        bullet.ice = iceOf(towers[i].type);
        //each tower fires its own microphone sprite
        if (towers[i].type == towerWeeknd) bullet.sprite = microphoneBmp;
        else                               bullet.sprite = drakeMicBmp;
        if (*bulletCount < maxBullets) bullets[(*bulletCount)++] = bullet;

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
    Tower ma = towerModelRectangle(a);
    Tower mb = towerModelRectangle(b);
    return ma.x < mb.x + mb.w && ma.x + ma.w > mb.x && ma.y < mb.y + mb.h && ma.y + ma.h > mb.y;
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
    Tower model = towerModelRectangle(tower);

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
   
#endif
