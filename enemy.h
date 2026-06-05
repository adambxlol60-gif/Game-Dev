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
    bool camo = false;
    bool metal = false;
    bool isKing = false;   //the boss draws bigger but uses a smaller scale since its PNG is 512px
    bool pendingSplit        = false;
    int  splitCount         = 0;
    int  splitHp            = 0;
    float splitSpeed        = 0.0f;
    ALLEGRO_BITMAP* splitBitmap = nullptr;
    int hp;
    int maxHp;
};

// slime path points
Slime initSlime(ALLEGRO_BITMAP* bitmap, int hp, float speed);
void updateSlime(Slime& s);
void drawSlime(const Slime& s);
void loadPathFromMap(ALLEGRO_BITMAP* mapBitmap);
