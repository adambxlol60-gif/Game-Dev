#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include "enemy.h"
#include <vector>
#include <algorithm>
#include <math.h>
#include <allegro5/allegro_primitives.h>

const int SCREEN_W = 1280;
const int SCREEN_H = 960;
const int MAX_TOWERS = 100;
const float TOWER_SCALE = 0.2f;

const float MODEL_X_FRAC = 0.37f;
const float MODEL_Y_FRAC = 0.26f;
const float MODEL_W_FRAC = 0.26f;
const float MODEL_H_FRAC = 0.48f;

struct Tower {
    float x, y, w, h;
};

inline Tower getModelRect(Tower t) {
    Tower m;
    m.x = t.x + t.w * MODEL_X_FRAC;
    m.y = t.y + t.h * MODEL_Y_FRAC;
    m.w = t.w * MODEL_W_FRAC;
    m.h = t.h * MODEL_H_FRAC;
    return m;
}

const float TOWER_RANGE = 200.0f;
const int   TOWER_COOLDOWN = 30;   // frames between shots (60fps -> 0.5s)
const float BULLET_SPEED = 14.0f;

struct TowerState {
    int cooldown = 0;
};

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

inline int findTarget(Tower t, const std::vector<Slime>& slimes) {
    Tower m = getModelRect(t);
    float cx = m.x + m.w * 0.5f;
    float cy = m.y + m.h * 0.5f;
    float r2 = TOWER_RANGE * TOWER_RANGE;
    int best = -1;
    float bestD = r2;
    for (size_t i = 0; i < slimes.size(); i++) {
        if (slimes[i].done) continue;
        float d = dist2(cx, cy, slimes[i].x, slimes[i].y);
        if (d <= bestD) { bestD = d; best = (int)i; }
    }
    return best;
}

inline void updateTowers(Tower towers[], TowerState states[], int towerCount,
                         std::vector<Slime>& slimes, std::vector<Bullet>& bullets) {
    for (int i = 0; i < towerCount; i++) {
        if (states[i].cooldown > 0) { states[i].cooldown--; continue; }
        int idx = findTarget(towers[i], slimes);
        if (idx < 0) continue;

        Tower m = getModelRect(towers[i]);
        float cx = m.x + m.w * 0.5f;
        float cy = m.y + m.h * 0.5f;

        // Predict intercept: solve |P + V*t| = BULLET_SPEED * t
        float px = slimes[idx].x - cx;
        float py = slimes[idx].y - cy;
        float vx = slimes[idx].vx;
        float vy = slimes[idx].vy;
        float a = vx*vx + vy*vy - BULLET_SPEED * BULLET_SPEED;
        float b_ = 2.0f * (px*vx + py*vy);
        float c  = px*px + py*py;
        float aimX = slimes[idx].x;
        float aimY = slimes[idx].y;
        if (fabsf(a) < 0.0001f) {
            if (fabsf(b_) > 0.0001f) {
                float t = -c / b_;
                if (t > 0) { aimX = slimes[idx].x + vx * t; aimY = slimes[idx].y + vy * t; }
            }
        } else {
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

        float dx = aimX - cx;
        float dy = aimY - cy;
        float len = sqrtf(dx*dx + dy*dy);
        if (len < 0.001f) len = 1.0f;

        Bullet b;
        b.x = cx; b.y = cy;
        b.vx = (dx/len) * BULLET_SPEED;
        b.vy = (dy/len) * BULLET_SPEED;
        b.damage = 1;
        b.alive = true;
        bullets.push_back(b);

        states[i].cooldown = TOWER_COOLDOWN;
    }
}

inline void updateBullets(std::vector<Bullet>& bullets, std::vector<Slime>& slimes) {
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

inline bool onPath(ALLEGRO_BITMAP* map, int mouseX, int mouseY) {
    ALLEGRO_COLOR color = al_get_pixel(map, mouseX, mouseY);
    unsigned char r, g, b;
    al_unmap_rgb(color, &r, &g, &b);
    return (r > 160 && g > 120 && b < 120);
}

inline bool towersOverlap(Tower a, Tower b) {
    Tower ma = getModelRect(a);
    Tower mb = getModelRect(b);
    return ma.x < mb.x + mb.w &&
           ma.x + ma.w > mb.x &&
           ma.y < mb.y + mb.h &&
           ma.y + ma.h > mb.y;
}

inline bool overlapsAnyTower(Tower newTower, Tower towers[], int towerCount) {
    for (int i = 0; i < towerCount; i++)
        if (towersOverlap(newTower, towers[i])) return true;
    return false;
}

inline bool towerTouchesPath(ALLEGRO_BITMAP* map, Tower tower) {
    Tower model = getModelRect(tower);

    int mapW = al_get_bitmap_width(map);
    int mapH = al_get_bitmap_height(map);

    int left   = (int)model.x;
    int right  = (int)(model.x + model.w);
    int top    = (int)model.y;
    int bottom = (int)(model.y + model.h);

    if (left   < 0)     left   = 0;
    if (top    < 0)     top    = 0;
    if (right  >= mapW) right  = mapW - 1;
    if (bottom >= mapH) bottom = mapH - 1;

    bool touches = false;
    al_lock_bitmap(map, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

    for (int y = top; y <= bottom && !touches; y += 2)
        for (int x = left; x <= right; x += 2)
            if (onPath(map, x, y)) touches = true;

    al_unlock_bitmap(map);
    return touches;
}

inline bool initAllegro() {
    if (!al_init()) return false;
    al_init_native_dialog_addon();
    if (!al_install_mouse()) {
        al_show_native_message_box(nullptr, "Error", "Error",
            "Failed to install mouse!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }
    if (!al_init_image_addon()) {
        al_show_native_message_box(nullptr, "Error", "Error",
            "Failed to initialize image addon!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }
    if (!al_init_primitives_addon()) {
    al_show_native_message_box(nullptr, "Error", "Error",
        "Failed to initialize primitives addon!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
    return false;
    }
    return true;
}

inline ALLEGRO_DISPLAY* createDisplay() {
    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display) {
        al_show_native_message_box(nullptr, "Error", "Error",
            "Failed to initialize display!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        return nullptr;
    }
    al_set_window_title(display, "Tower Defense");
    al_show_mouse_cursor(display);
    al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);
    return display;
}

inline bool loadBitmaps(ALLEGRO_DISPLAY* display,
                        ALLEGRO_BITMAP*& map,
                        ALLEGRO_BITMAP*& drakeTower,
                        ALLEGRO_BITMAP*& slimeBmp) {
    map = al_load_bitmap("Images/BetaMap.png");
    if (!map) {
        al_show_native_message_box(display, "Error", "Error",
            "Failed to load BetaMap.png!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }
    drakeTower = al_load_bitmap("Images/DrakeTower.png");
    if (!drakeTower) {
        al_show_native_message_box(display, "Error", "Error",
            "Failed to load DrakeTower.png!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_bitmap(map);
        return false;
    }
    slimeBmp = al_load_bitmap("Images/Slime.png");
    if (!slimeBmp) {
        al_show_native_message_box(display, "Error", "Error",
            "Failed to load Slime.png!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_bitmap(drakeTower);
        al_destroy_bitmap(map);
        return false;
    }
    return true;
}

inline void setupEventQueue(ALLEGRO_EVENT_QUEUE* queue,
                            ALLEGRO_DISPLAY* display,
                            ALLEGRO_TIMER* timer) {
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
}

inline void handleMouseClick(const ALLEGRO_EVENT& event,
                              Tower towers[], int& towerCount,
                              ALLEGRO_BITMAP* map,
                              ALLEGRO_BITMAP* drakeTower,
                              int drakeW, int drakeH) {
    if (event.mouse.button != 1) return;

    float towerW = drakeW * TOWER_SCALE;
    float towerH = drakeH * TOWER_SCALE;

    Tower newTower;
    newTower.x = event.mouse.x - towerW / 2;
    newTower.y = event.mouse.y - towerH / 2;
    newTower.w = towerW;
    newTower.h = towerH;

    Tower model = getModelRect(newTower);
    bool insideScreen =
        model.x >= 0 &&
        model.y >= 0 &&
        model.x + model.w <= SCREEN_W &&
        model.y + model.h <= SCREEN_H;

    if (insideScreen &&
        towerCount < MAX_TOWERS &&
        !towerTouchesPath(map, newTower) &&
        !overlapsAnyTower(newTower, towers, towerCount)) {
        towers[towerCount++] = newTower;
    }
}

inline void renderFrame(ALLEGRO_BITMAP* map,
                        ALLEGRO_BITMAP* drakeTower,
                        Tower towers[], int towerCount,
                        int drakeW, int drakeH,
                        const Slime& slime) {
    al_draw_bitmap(map, 0, 0, 0);

    for (int i = 0; i < towerCount; i++) {
        al_draw_scaled_bitmap(drakeTower, 0, 0, drakeW, drakeH,
            towers[i].x, towers[i].y, towers[i].w, towers[i].h, 0);
    }

    drawSlime(slime);
    al_flip_display();
}

inline void cleanup(ALLEGRO_TIMER* timer,
                    ALLEGRO_EVENT_QUEUE* queue,
                    ALLEGRO_BITMAP* slimeBmp,
                    ALLEGRO_BITMAP* drakeTower,
                    ALLEGRO_BITMAP* map,
                    ALLEGRO_DISPLAY* display) {
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_bitmap(slimeBmp);
    al_destroy_bitmap(drakeTower);
    al_destroy_bitmap(map);
    al_destroy_display(display);
}

#endif