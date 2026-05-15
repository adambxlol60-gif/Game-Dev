#pragma once
#include <allegro5/allegro.h>

struct Slime {
    ALLEGRO_BITMAP* bitmap;
    float x;
    float y;
    float speed;
    int target;
    bool done;
};

// Function prototypes
Slime initSlime(ALLEGRO_BITMAP* bitmap);
void updateSlime(Slime& s);
void drawSlime(const Slime& s);
void loadPathFromMap(ALLEGRO_BITMAP* mapBitmap);