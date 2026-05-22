#ifndef BULLET_H
#define BULLET_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "enemy.h"
#include <vector>
#include <algorithm>

const float BULLET_SPEED = 14.0f;

struct Bullet {
    float x, y;
    float vx, vy;
    int damage;
    bool alive;
};

inline float dist2(float ax, float ay, float bx, float by) {
    float dx = ax - bx, dy = ay - by;
    return dx*dx + dy*dy;
}

inline void updateBullets(std::vector<Bullet>& bullets, std::vector<Slime>& slimes) {
    extern const int SCREEN_W;
    extern const int SCREEN_H;
    for (auto& b : bullets) {
        if (!b.alive) continue;
        b.x += b.vx;
        b.y += b.vy;
        if (b.x < 0 || b.y < 0 || b.x > SCREEN_W || b.y > SCREEN_H) {
            b.alive = false;
            continue;
        }
        for (auto& s : slimes) {
            if (s.done) continue;
            if (dist2(b.x, b.y, s.x, s.y) < 20.0f * 20.0f) {
                s.hp -= b.damage;
                if (s.hp <= 0) s.done = true;
                b.alive = false;
                break;
            }
        }
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b){ return !b.alive; }), bullets.end());
}

inline void drawBullets(const std::vector<Bullet>& bullets) {
    for (const auto& b : bullets)
        if (b.alive)
            al_draw_filled_circle(b.x, b.y, 4, al_map_rgb(255, 220, 0));
}

#endif
