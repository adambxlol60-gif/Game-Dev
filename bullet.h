#ifndef BULLET_H
#define BULLET_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "enemy.h"

// bullet properties
const float BULLET_SPEED = 14.0f;
const int   MAX_BULLETS  = 500;

struct Bullet {
    float x, y; // position
    float vx, vy; // velocity
    int damage; // damage to enemy  
    bool alive; // wether the bullet is active
};

//float distance to square of the
inline float dist2(float ax, float ay, float bx, float by) {
    float dx = ax - bx, dy = ay - by;
    return dx*dx + dy*dy;
}

// updates the bullet positions and checks for collisions with slimes
// it also removes bullets that are out of bouunds or have already hit the slime
inline void updateBullets(Bullet bullets[], int* bulletCount, Slime slimes[], int slimeCount) {
    extern const int SCREEN_W;
    extern const int SCREEN_H;
    for (int i = 0; i < *bulletCount; i++) {
        Bullet* b = &bullets[i];
        if (!b->alive) continue;
        b->x += b->vx;
        b->y += b->vy;
        if (b->x < 0 || b->y < 0 || b->x > SCREEN_W || b->y > SCREEN_H) {
            b->alive = false;
            continue;
        }
        for (int j = 0; j < slimeCount; j++) {
            if (slimes[j].done) continue;
            if (dist2(b->x, b->y, slimes[j].x, slimes[j].y) < 20.0f * 20.0f) {
                slimes[j].hp -= b->damage;
                if (slimes[j].hp <= 0) slimes[j].done = true;
                b->alive = false;
                break;
            }
        }
    }
    // compact: swap dead bullets with last
    for (int i = 0; i < *bulletCount; ) {
        if (!bullets[i].alive)
            bullets[i] = bullets[--(*bulletCount)];
        else
            i++;
    }
}

// draws the bullets as filled circles for placeholder projectiles
inline void drawBullets(const Bullet bullets[], int bulletCount) {
    for (int i = 0; i < bulletCount; i++)
        if (bullets[i].alive)
            al_draw_filled_circle(bullets[i].x, bullets[i].y, 4, al_map_rgb(255, 220, 0));
}

#endif
