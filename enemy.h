//created by Noah Basaria and Adam Jurewicz

#pragma once
#include <allegro5/allegro.h>

const int maxSlimes = 200;

//struct for the slime enemy
struct Slime {
    ALLEGRO_BITMAP* bitmap;
    float x;
    float y;
    float vx;
    float vy;
    float speed;
    int target;
    bool done;
    bool escaped;
    bool isIced = false;
    int hitDamageTimer = 0;   //frames left to draw the slime red after being hit (counts down like a tower cooldown)
    int hp;
    int maxHp;
};

// slime path points
Slime initSlime(ALLEGRO_BITMAP* bitmap, int hp, float speed); // initializes a slime with the bitmap which is given);
void updateSlime(Slime& s); // moves the slime along the path
void drawSlime(const Slime& s); // draws the slime at the correct position
void loadPathFromMap(ALLEGRO_BITMAP* mapBitmap); // loads the path points from a bitmap where the path is draw in a specific color

