#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include "tower.h"
#include "hud.h"

inline bool initAllegro() {
    if (!al_init()) return false;
    al_init_font_addon();
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
