//created by Noah Basaria and Adam Jurewicz

#ifndef TOWER_H
#define TOWER_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include "enemy.h"
#include "bullet.h"
#include "hud.h"

//this const maxTowers is there to prevent the player from placing more then 100 towers, there is already a 20 tower limit but this is an extra safety measure
const int maxTowers = 100;
//this scale is super important for making the game look good and cohessive, since the towers sprites are in different pixel ratios we need this scale to make them all the same size in game.
const float towerScale = 1.85f;
const float weekndScale = 1.05f;
const float elonScale = 0.85f;
const float bankScale = 0.34f;
const float icemanScale = 1.25f;
const float starboyScale = 1.05f;
const float teslaScale = 1.0f;

//these values are to make a smaller rectangle in the middle of the tower to make overlapping more forgiving and to combat path collision issues. rectangle is smaller then the tower
const float modelXFrac = 0.37f;
const float modelYFrac = 0.26f;
const float modelWFrac = 0.26f;
const float modelHFrac = 0.48f;

//tower ids. We use these to keep track of which tower is which type without it being messy.
const int towerDrake = 0;
const int towerWeeknd = 1;
const int towerElon = 2;
const int towerBank = 3;
const int towerIceman = 4;    //drake's upgraded form  Iceman its a different tower with better stats and freezing
const int towerStarboy = 5;   //weeknd's upgraded form more explosive damage, faster shooting and longer range. new sprite
const int towerTeslaMan = 6;  //elon's upgraded form, he is super strong but very expensive he is suppose to be a tower you save up for and get a massive power spike with.
const int maxBanks = 3;       //limits the amount of banks so there wouldnt be ascenario where the  banks instantly pay for themselves and the player has infinite money
const int maxDamageUpgradeLevel = 5;

//upgrade costs, these are the costs to replace the towers with a new the better towers. 
const int drakeUpgradeCost = 500;
const int weekndUpgradeCost = 800;
const int elonUpgradeCost = 2500;   //

//this is the struct for the towers, save fireTimer ()
struct Tower { 
    float x, y, w, h;
    int fireTimer;
    int type;
    int damageUpgradeLevel = 0;   //+1 damage per level. Cost ramps up. Banks ignore this.
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
const float weekndRange = 320.0f;
const float starboyRange = 350.0f;
const float elonRange   = 250.0f;
const float icemanRange = 260.0f;   //slightly longer than drake
const int   towerCooldown = 30;
//per-tower fire cooldown in frames. lower = shoots faster
const int drakeCooldown   = 30;   //normal
const int icemanCooldown  = 30;   //same as drake
const int weekndCooldown  = 25;   //a bit faster than drake
const int starboyCooldown = 20;   //a bit faster than weeknd
const int elonCooldown    = 30;   //normal
const int teslaCooldown   = 15;   //really fast, faster than starboy

//rangeOf returns the firing range for a given tower type
inline float rangeOf(int towerType) {
    if (towerType == towerWeeknd)  return weekndRange;
    if (towerType == towerElon)    return elonRange;
    if (towerType == towerBank)    return 0.0f;
    if (towerType == towerIceman)  return icemanRange;
    if (towerType == towerStarboy) return starboyRange;   //weeknd's old longer range
    if (towerType == towerTeslaMan) return elonRange;    //same as elon for now
    return towerRange;
}

const int drakeDamage = 1;
const int weekndDamage = 1;
const int elonDamage = 4;
const int icemanDamage = 1;
const int starboyDamage = 1;
const int teslaDamage = 5;
const int drakePierce = 0;
const int weekndPierce = 1;
const int elonPierce = 0;
const int icemanPierce = 0;
const int drakeIce = 0;
const int weekndIce = 0;
const int elonIce = 0;
const int icemanIce = 1;      //iceman freezes
const int drakeVision = 0;
const int weekndVision = 1;
const int elonVision = 0;
const int elonUpgVision = 1;
const int icemanVision = 0;

//explosive blast radius (pixels) per tower.
const int weekndExplosive  = 0;     //weeknd has no blast, just pierce
const int starboyExplosive = 55;    //small blast added on top of starboy's pierce
const int elonExplosive    = 80;
const int teslaExplosive   = 140;   
//explosive damage does damage to all slimes in the areaos
const int weekndExplosiveDamage  = 0;
const int starboyExplosiveDamage = 1;
const int elonExplosiveDamage    = 1;
const int teslaExplosiveDamage   = 2;

//explosiveOf returns the blast radius for a tower type (0 is no explosion)
inline int explosiveOf(int towerType) {
    if (towerType == towerWeeknd)   return weekndExplosive;
    if (towerType == towerStarboy)  return starboyExplosive;
    if (towerType == towerElon)     return elonExplosive;
    if (towerType == towerTeslaMan) return teslaExplosive;
    return 0;
}
//explosiveDamageOf returns the blast damage for a tower type
inline int explosiveDamageOf(int towerType) {
    if (towerType == towerWeeknd)   return weekndExplosiveDamage;
    if (towerType == towerStarboy)  return starboyExplosiveDamage;
    if (towerType == towerElon)     return elonExplosiveDamage;
    if (towerType == towerTeslaMan) return teslaExplosiveDamage;
    return 0;
}

//damageOf returns how much damage a bullet from this tower type deals
inline int damageOf(int towerType) {
    if (towerType == towerWeeknd)  return weekndDamage;
    if (towerType == towerElon)    return elonDamage;
    if (towerType == towerBank)    return 0;
    if (towerType == towerIceman)  return icemanDamage;
    if (towerType == towerStarboy) return starboyDamage;
    if (towerType == towerTeslaMan) return teslaDamage;
    return drakeDamage;
}

//pierceOf returns how many extra slimes a bullet from this tower type can hit before dying
inline int pierceOf(int towerType) {
    if (towerType == towerWeeknd)  return weekndPierce;
    if (towerType == towerElon)    return elonPierce;
    if (towerType == towerBank)    return 0;
    if (towerType == towerIceman)  return icemanPierce;
    if (towerType == towerStarboy) return weekndPierce;  
    if (towerType == towerTeslaMan) return elonPierce;
    return drakePierce;
}
inline int iceOf(int towerType) {
    if (towerType == towerWeeknd)  return weekndIce;
    if (towerType == towerElon)    return elonIce;
    if (towerType == towerBank)    return 0;
    if (towerType == towerIceman)  return icemanIce;
    if (towerType == towerStarboy) return weekndIce;   
    if (towerType == towerTeslaMan) return elonIce;    
    return drakeIce;
}

inline int visionOf(int towerType, int upgradeLevel) {
    if (towerType == towerWeeknd)   return weekndVision;
    if (towerType == towerStarboy)  return weekndVision;  
    if (towerType == towerTeslaMan) return elonUpgVision;  //tesla (upgraded elon) can see camo
    if (towerType == towerElon && upgradeLevel >= 1) return elonUpgVision;
    return drakeVision;
}
//cooldownOf returns how many frames a tower waits between shots (lower = faster)
inline int cooldownOf(int towerType) {
    if (towerType == towerWeeknd)   return weekndCooldown;
    if (towerType == towerElon)     return elonCooldown;
    if (towerType == towerIceman)   return icemanCooldown;
    if (towerType == towerStarboy)  return starboyCooldown;
    if (towerType == towerTeslaMan) return teslaCooldown;
    return drakeCooldown;   
}
//Saves information about the tower's cooldown and whether a bullet is alive or not
struct TowerState {
    int cooldown = 0;
};

//counts how many banks currently exist so the placement code can enforce the 3 bank cap
inline int countBanks(Tower towers[], int towerCount) {
    int n = 0;
    for (int i = 0; i < towerCount; i++) if (towers[i].type == towerBank) n++;
    return n;
}

//So this is a pretty complex function but it is very crucial:
//it first gets the tower's body center (cx, cy) from its model rectangle
//then it sets radius2 = range squared (squared so we skip a slow sqrt in the loop)
//best = -1 means no target found yet; bestD starts at radius2 so any candidate must be within range to beat it
//it loops through every slime, skips dead ones, and tracks the closest in-range slime, returning its index (or -1 if none qualify)
inline int findTarget(Tower t, const Slime slimes[], int slimeCount, float range, int vision) {
    Tower model = towerModelRectangle(t);
    float centerX = model.x + model.w * 0.5f;
    float centerY = model.y + model.h * 0.5f;
    float radius2 = range * range;
    int best = -1;
    float bestD = radius2;
    for (int i = 0; i < slimeCount; i++) {
        if (slimes[i].done) continue;
        if (slimes[i].camo && vision == 0) continue;
        float slimeDistance = distance(centerX, centerY, slimes[i].x, slimes[i].y);
        if (slimeDistance <= bestD) {
            bestD = slimeDistance; best = i; }
    }
    return best;
}
//Probably the most complex function in the game right now. It makes the bullets shoot but it also predicts where the slime will be when the bullet reaches it.
//It does this by solving the equation |P + V*t| = bulletSpeed * t, where P is the vector from the tower to the slime, V is the velocity of the slime, and t is the time it takes for the bullet to reach the slime. This gives us a quadratic equation in t, which we can solve using the quadratic formula. We then choose the positive solution that gives us the earliest intercept time.
//I was able to find this formula which helped tremedously as before the bullet would miss the slimes a lot
//Reference: https://www.mathsisfun.com/algebra/quadratic-equation.html
inline void updateTowers(Tower towers[], TowerState states[], int towerCount, Slime slimes[], int slimeCount, Bullet bullets[], int* bulletCount, ALLEGRO_BITMAP* microphoneBmp, ALLEGRO_BITMAP* drakeMicBmp, ALLEGRO_BITMAP* rocketBmp) {
    for (int i = 0; i < towerCount; i++) { //this part checks for the cooldown and uses the previous function to find a target slime for the tower
        if (towers[i].type == towerBank) continue; //banks dont shoot
        if (states[i].cooldown > 0) { states[i].cooldown--; continue; }
        float range = rangeOf(towers[i].type);
        int vision = visionOf(towers[i].type, towers[i].damageUpgradeLevel);
        int idx = findTarget(towers[i], slimes, slimeCount, range, vision);
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
        // I found the prediction equation on these 2 webstie which helped a lothttps://www.gamedeveloper.com/programming/predictive-aim-mathematics-for-ai-targeting https://indyandyjones.wordpress.com/2010/04/08/intercepting-a-target-with-projectile/
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
        }
        else { //this actually solves the quadratic equation
            float disc = b_*b_ - 4*a*c;
            if (disc >= 0) {
                float sq = sqrtf(disc);
                float t1 = (-b_ - sq) / (2*a);
                float t2 = (-b_ + sq) / (2*a);
                float t = -1;
                if (t1 > 0 && t2 > 0) {
                    if (t1 < t2) { t = t1; } else { t = t2; }
                }
                else if (t1 > 0) t = t1;
                else if (t2 > 0) t = t2;
                if (t > 0) { aimX = slimes[idx].x + vx * t; aimY = slimes[idx].y + vy * t; }
            }
        }
        //this calculates the direction from the tower to predicted position and creates a bullet with that velocity
        float dx = aimX - centerX;
        float dy = aimY - centerY;
        float len = sqrtf(dx*dx + dy*dy); //we used pyothegorean theorem to find the length of the vector
        if (len < 0.001f) len = 1.0f; // the safety check that prevents dividing by zero
            // struct for bullets, gives it properties like speed, damage, pierce, if its alive ex. Also does hit boxchecking
        Bullet bullet;
        bullet.x = centerX; bullet.y = centerY;
        bullet.vx = (dx/len) * bulletSpeed;
        bullet.vy = (dy/len) * bulletSpeed;
        bullet.damage = damageOf(towers[i].type) + towers[i].damageUpgradeLevel;
        bullet.pierce = pierceOf(towers[i].type);
        bullet.alive = true;
        bullet.ice = iceOf(towers[i].type);
        bullet.armorPiercing = (towers[i].type == towerElon || towers[i].type == towerTeslaMan || towers[i].type == towerStarboy);
        bullet.explosive = explosiveOf(towers[i].type);
        bullet.explosiveDamage = explosiveDamageOf(towers[i].type);
        //each tower fires its own bullet sprite (drake + iceman share the drake mic)
        //spriteScale differs per sprite because the source PNGs are very different sizes (microphone is 1280px, the others ~128px)
        if (towers[i].type == towerWeeknd) { 
            bullet.sprite = microphoneBmp; 
            bullet.spriteScale = 0.22f; 
        }
        else if (towers[i].type == towerStarboy) { 
            bullet.sprite = microphoneBmp;
            bullet.spriteScale = 0.22f; 
        }
        else if (towers[i].type == towerElon || towers[i].type == towerTeslaMan) {
            bullet.sprite = rocketBmp;
            bullet.spriteScale = 0.3f;
        }
        else
        { bullet.sprite = drakeMicBmp;   bullet.spriteScale = 0.18f; 
        }
        if (*bulletCount < maxBullets) {
            bullets[(*bulletCount)++] = bullet;
        }

        //each damage upgrade also speeds up firing slightly (2 frames faster per level, floored so it never gets silly)
        int cd = cooldownOf(towers[i].type) - 2 * towers[i].damageUpgradeLevel;
        if (cd < 6) cd = 6;
        states[i].cooldown = cd;
    }
}
//this onPath code for placing a tower
//it checks for the color of the pixel where the cursor is and if the color is close to the path color (red > 160, green > 120, blue < 120)
//the main point of it is to prevent the player from placing the tower on the path
//It probably would be better longterm to have a different bitmap just for the path but this works well enough :)
// these 2 websites help us learn about al lock pixel and get pixels https://liballeg.org/a5docs/trunk/graphics.html#al_get_pixel https://liballeg.org/a5docs/trunk/graphics.html#al_lock_bitmap
inline bool onPath(ALLEGRO_BITMAP* map, int mouseX, int mouseY) {
    ALLEGRO_COLOR color = al_get_pixel(map, mouseX, mouseY);
    unsigned char r, g, b;
    al_unmap_rgb(color, &r, &g, &b);
    return (r > 160 && g > 120 && b < 120);
}
//this is another function made for tower placement, this time its for making sure you cant place a tower on top of another tower
//funny thing is that the hitbox is not the drake itself it is a smaller rectangle in the middle of the tower
//the reasion for this is because the png of drake is bigger than what it looks like, so the rectangle is there to make it more accurate
//we used this article to learn about collisions better: https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection
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
