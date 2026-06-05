//created by Noah Basaria and Adam Jurewicz

#include "enemy.h"
#include <allegro5/allegro_image.h>
#include <math.h>

// path arrays and point count for slime movement
static float pathX[10000];
static float pathY[10000];
static int pointCount = 0;

// loads the path points from a bitmap where the path is drawn in a specific color
void loadPathFromMap(ALLEGRO_BITMAP* mapBitmap) {
    int w = al_get_bitmap_width(mapBitmap);
    int h = al_get_bitmap_height(mapBitmap);
    al_lock_bitmap(mapBitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

    for (int x = 0; x < w; x += 20) {

        int totalY = 0;

        int count = 0;
        for (int y = 0; y < h; y++) {
            ALLEGRO_COLOR c = al_get_pixel(mapBitmap, x, y);
            unsigned char r, g, b, a;
            al_unmap_rgba(c, &r, &g, &b, &a);
            if (r > 220 && g > 180 && b < 100) {
                totalY += y;
                count++;
            }
        }
        if (count > 0) {
            pathX[pointCount] = (float)x;
            pathY[pointCount] = (float)(totalY / count);
            pointCount++;
        }
    }

    al_unlock_bitmap(mapBitmap);
}

// slime structere and function definitions
Slime initSlime(ALLEGRO_BITMAP* bitmap, int hp, float speed) {
    Slime s;
    s.bitmap = bitmap;
    s.x = pathX[0];
    s.y = pathY[0];
    s.vx = 0;
    s.vy = 0;
    s.speed = speed;
    s.target = 1;
    s.done = false;
    s.escaped = false;
    s.isIced = false;
    s.camo = false;
    s.metal = false;
    s.pendingSplit = false;
    s.splitCount = 0;
    s.splitHp = 0;
    s.splitSpeed = 0.0f;
    s.splitBitmap = nullptr;
    s.hp = hp;
    s.maxHp = hp;
    return s;
}

//void updateSlime function to move the slime along the path points
void updateSlime(Slime& s) {
    if (s.done) return;
    if (s.hitDamageTimer > 0) s.hitDamageTimer--;   //count down the red flash, same pattern as tower cooldown

    float targetX = pathX[s.target];
    float targetY = pathY[s.target];
    float dx = targetX - s.x;
    float dy = targetY - s.y;
    float dist = sqrt(dx * dx + dy * dy);

    if (dist <= s.speed) {
        s.x = targetX;
        s.y = targetY;
        s.vx = 0;
        s.vy = 0;
        s.target++;
        if (s.target >= pointCount) { s.done = true; s.escaped = true; }
    } else {
        s.vx = (dx / dist) * s.speed;
        s.vy = (dy / dist) * s.speed;
        s.x += s.vx;
        s.y += s.vy;
    }
}

// void drawSlime function made to draw the slime sprite at the slime's position
void drawSlime(const Slime& s) {
    if (s.done) return;
    int w = al_get_bitmap_width(s.bitmap);
    int h = al_get_bitmap_height(s.bitmap);
    //king PNG is 512px (others ~64px), so it needs a much smaller scale to look like a boss instead of filling the screen
    const float scale = s.isKing ? 0.32f : 1.2f;
    float drawX = s.x - (w * scale) / 2;
    float drawY = s.y - (h * scale) / 2;
    if (s.hitDamageTimer > 0) {
        //recently hit - tint it red
        al_draw_tinted_scaled_bitmap(s.bitmap, al_map_rgb(255, 80, 80), 0, 0, w, h, drawX, drawY, w * scale, h * scale, 0);
    } else {
        al_draw_scaled_bitmap(s.bitmap, 0, 0, w, h, drawX, drawY, w * scale, h * scale, 0);
    }

    //dimensions for the health bar
    float barWidth = 20.0f; // width of bar
    float barHeight = 4.0f; // height of bar
    float barX = s.x - barWidth / 2;
    float barY = s.y - 16.0f; // position the bar is above the slime
}