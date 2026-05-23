#ifndef TOWER_H
#define TOWER_H

// ─── Includes ────────────────────────────────────────────────────────────────
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include "enemy.h"
#include "bullet.h"

// ─── Screen Constants ────────────────────────────────────────────────────────
const int SCREEN_W = 1280;
const int SCREEN_H = 960;

// ─── Tower Constants ─────────────────────────────────────────────────────────

// capacity and scaling
const int   MAX_TOWERS  = 100;
const float TOWER_SCALE = 0.2f;

// model rect fractions: defines which sub-rectangle of the sprite is the
// actual tower body used for placement and targeting
const float MODEL_X_FRAC = 0.37f;
const float MODEL_Y_FRAC = 0.26f;
const float MODEL_W_FRAC = 0.26f;
const float MODEL_H_FRAC = 0.48f;

// combat
const float TOWER_RANGE    = 200.0f;
const int   TOWER_COOLDOWN = 30;    // frames between shots (60fps -> 0.5s)

// ─── Structs ─────────────────────────────────────────────────────────────────

struct Tower {
    float x, y, w, h; // position and size in screen pixels
};

struct TowerState {
    int cooldown = 0; // frames remaining until the tower can shoot again
};

// ─── Geometry Helpers ────────────────────────────────────────────────────────

// returns the model sub-rect of a tower sprite used for collision and targeting
inline Tower getModelRect(Tower t) {
    Tower m;
    m.x = t.x + t.w * MODEL_X_FRAC;
    m.y = t.y + t.h * MODEL_Y_FRAC;
    m.w = t.w * MODEL_W_FRAC;
    m.h = t.h * MODEL_H_FRAC;
    return m;
}

// checks whether a map pixel falls on the golden path colour
inline bool onPath(ALLEGRO_BITMAP* map, int mouseX, int mouseY) {
    ALLEGRO_COLOR color = al_get_pixel(map, mouseX, mouseY);
    unsigned char r, g, b;
    al_unmap_rgb(color, &r, &g, &b);
    return (r > 160 && g > 120 && b < 120);
}

// ─── Placement Validation ────────────────────────────────────────────────────

// returns true if the model rects of two towers overlap
inline bool towersOverlap(Tower a, Tower b) {
    Tower ma = getModelRect(a);
    Tower mb = getModelRect(b);
    return ma.x < mb.x + mb.w &&
           ma.x + ma.w > mb.x &&
           ma.y < mb.y + mb.h &&
           ma.y + ma.h > mb.y;
}

// returns true if newTower overlaps any already-placed tower
inline bool overlapsAnyTower(Tower newTower, Tower towers[], int towerCount) {
    for (int i = 0; i < towerCount; i++)
        if (towersOverlap(newTower, towers[i])) return true;
    return false;
}

// returns true if any pixel inside the tower's model rect is on the path
inline bool towerTouchesPath(ALLEGRO_BITMAP* map, Tower tower) {
    Tower model = getModelRect(tower);

    // clamp model rect to map bounds
    int mapW   = al_get_bitmap_width(map);
    int mapH   = al_get_bitmap_height(map);
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

    // sample every 2 pixels for performance
    for (int y = top; y <= bottom && !touches; y += 2)
        for (int x = left; x <= right; x += 2)
            if (onPath(map, x, y)) touches = true;

    al_unlock_bitmap(map);
    return touches;
}

// ─── Targeting ───────────────────────────────────────────────────────────────

// returns the index of the closest living slime within range, or -1 if none
inline int findTarget(Tower t, const Slime slimes[], int slimeCount) {
    Tower m  = getModelRect(t);
    float cx = m.x + m.w * 0.5f;
    float cy = m.y + m.h * 0.5f;
    float r2 = TOWER_RANGE * TOWER_RANGE;

    int   best  = -1;
    float bestD = r2;

    for (int i = 0; i < slimeCount; i++) {
        if (slimes[i].done) continue;
        float d = dist2(cx, cy, slimes[i].x, slimes[i].y);
        if (d <= bestD) { bestD = d; best = i; }
    }
    return best;
}

// ─── Tower Update ────────────────────────────────────────────────────────────

// ticks each tower's cooldown, finds a target, and fires a lead-predicted bullet
inline void updateTowers(Tower towers[], TowerState states[], int towerCount,
                         Slime slimes[], int slimeCount, Bullet bullets[], int* bulletCount) {
    for (int i = 0; i < towerCount; i++) {

        // skip towers still on cooldown
        if (states[i].cooldown > 0) { states[i].cooldown--; continue; }

        int idx = findTarget(towers[i], slimes, slimeCount);
        if (idx < 0) continue;

        // find the barrel position (center of model rect)
        Tower m  = getModelRect(towers[i]);
        float cx = m.x + m.w * 0.5f;
        float cy = m.y + m.h * 0.5f;

        // Predict intercept: solve |P + V*t| = BULLET_SPEED * t
        float px = slimes[idx].x - cx;
        float py = slimes[idx].y - cy;
        float vx = slimes[idx].vx;
        float vy = slimes[idx].vy;
        float a  = vx*vx + vy*vy - BULLET_SPEED * BULLET_SPEED;
        float b_ = 2.0f * (px*vx + py*vy);
        float c  = px*px + py*py;

        // default aim: current slime position (fallback if no intercept)
        float aimX = slimes[idx].x;
        float aimY = slimes[idx].y;

        if (fabsf(a) < 0.0001f) {
            // linear case
            if (fabsf(b_) > 0.0001f) {
                float t = -c / b_;
                if (t > 0) { aimX = slimes[idx].x + vx * t; aimY = slimes[idx].y + vy * t; }
            }
        } else {
            // quadratic case: pick smallest positive root
            float disc = b_*b_ - 4*a*c;
            if (disc >= 0) {
                float sq = sqrtf(disc);
                float t1 = (-b_ - sq) / (2*a);
                float t2 = (-b_ + sq) / (2*a);
                float t  = -1;
                if      (t1 > 0 && t2 > 0) t = (t1 < t2) ? t1 : t2;
                else if (t1 > 0)            t = t1;
                else if (t2 > 0)            t = t2;
                if (t > 0) { aimX = slimes[idx].x + vx * t; aimY = slimes[idx].y + vy * t; }
            }
        }

        // normalize direction and fire bullet
        float dx  = aimX - cx;
        float dy  = aimY - cy;
        float len = sqrtf(dx*dx + dy*dy);
        if (len < 0.001f) len = 1.0f;

        Bullet b;
        b.x      = cx;
        b.y      = cy;
        b.vx     = (dx / len) * BULLET_SPEED;
        b.vy     = (dy / len) * BULLET_SPEED;
        b.damage = 1;
        b.alive  = true;
        if (*bulletCount < MAX_BULLETS) bullets[(*bulletCount)++] = b;

        states[i].cooldown = TOWER_COOLDOWN;
    }
}

// ─── Mouse Click Handler ─────────────────────────────────────────────────────

// places a new tower at the mouse position if all placement rules pass
inline void handleMouseClick(const ALLEGRO_EVENT& event,
                              Tower towers[], int& towerCount,
                              ALLEGRO_BITMAP* map,
                              ALLEGRO_BITMAP* drakeTower,
                              int drakeW, int drakeH, int& gold) {
    extern const int drakeCost;
    extern const int maxDrakeLimit;
    if (event.mouse.button != 1) return;

    // compute tower dimensions from sprite size and scale
    float towerW = drakeW * TOWER_SCALE;
    float towerH = drakeH * TOWER_SCALE;

    // center the tower on the cursor
    Tower newTower;
    newTower.x = event.mouse.x - towerW / 2;
    newTower.y = event.mouse.y - towerH / 2;
    newTower.w = towerW;
    newTower.h = towerH;

    // check that the model rect is fully inside the screen
    Tower model = getModelRect(newTower);
    bool insideScreen =
        model.x >= 0 &&
        model.y >= 0 &&
        model.x + model.w <= SCREEN_W &&
        model.y + model.h <= SCREEN_H;

    // place only if all conditions are met
    if (insideScreen &&
        towerCount < maxDrakeLimit &&
        gold >= drakeCost &&
        !towerTouchesPath(map, newTower) &&
        !overlapsAnyTower(newTower, towers, towerCount)) {
        towers[towerCount++] = newTower;
        gold -= drakeCost;
    }
}

#endif
