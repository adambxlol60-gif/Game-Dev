#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include "enemy.h"

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