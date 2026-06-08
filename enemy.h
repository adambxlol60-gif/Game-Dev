//created by Noah Basaria and Adam Jurewicz

#pragma once
#include <allegro5/allegro.h>

const int maxSlimes = 200;

//struct for the slime enemy I don't know why I didnt make this an inline.
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
    int hitDamageTimer = 0;  
    bool camo = false;
    bool metal = false;
    bool isKing = false;   
    bool pendingSplit = false;
    int  splitCount = 0;
    int  splitHp = 0;
    float splitSpeed = 0.0f;
    ALLEGRO_BITMAP* splitBitmap = nullptr;
    int hp;
    int maxHp;
};

//declalartion for the functions used in enemy.cpp
Slime initSlime(ALLEGRO_BITMAP* bitmap, int hp, float speed);
void updateSlime(Slime& s);
void drawSlime(const Slime& s);
void loadPathFromMap(ALLEGRO_BITMAP* mapBitmap);
