#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "enemy.h"

//screen size 4:3 ratio
const int screenW = 1280;
const int screenH = 960;

// split headers — included after screenW/screenH so they can use them
#include "bullet.h"
#include "hud.h"
#include "tower.h"

//simple allegro initialization function, they also check if the mouse, image addon are initialized
inline bool initAllegro() {
    if (!al_init()) return false;
    al_init_font_addon();
    al_init_primitives_addon();
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
//creates the display the game runs in
inline ALLEGRO_DISPLAY* createDisplay() {
    ALLEGRO_DISPLAY* display = al_create_display(screenW, screenH);
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
//loads the images, if it fails to load any of the 3 images it shows an error message
//long term we will probably need to make this a function but for now since we only have 3 images its not too bad
inline bool loadBitmaps(ALLEGRO_DISPLAY* display, ALLEGRO_BITMAP*& map, ALLEGRO_BITMAP*& towerBmp, ALLEGRO_BITMAP*& slimeBmp) {
    map = al_load_bitmap("Images/BetaMap.png");
    if (!map) {
        al_show_native_message_box(display, "Error", "Error",
            "Failed to load BetaMap.png!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }
    towerBmp = al_load_bitmap("Images/DrakeTower.png");
    if (!towerBmp) {
        al_show_native_message_box(display, "Error", "Error",
            "Failed to load DrakeTower.png!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_bitmap(map);
        return false;
    }
    slimeBmp = al_load_bitmap("Images/Slime.png");
    if (!slimeBmp) {
        al_show_native_message_box(display, "Error", "Error",
            "Failed to load Slime.png!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_bitmap(towerBmp);
        al_destroy_bitmap(map);
        return false;
    }
    return true;
}
//setups a event queue for the display, mouse, and timer.
// the display one is used to check if the player clicks the x button to close the game
//the mouse one is used to check for mouse clicks to place towers
//the timer one is used to update the game state every frame
inline void setupEventQueue(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_DISPLAY* display, ALLEGRO_TIMER* timer) {
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
}
//function to clean up the allegro resources, it destroys the timer, event queue, bitmaps, and display when game is closed
inline void cleanup(ALLEGRO_TIMER* timer,
                    ALLEGRO_EVENT_QUEUE* queue,
                    ALLEGRO_BITMAP* slimeBmp,
                    ALLEGRO_BITMAP* towerBmp,
                    ALLEGRO_BITMAP* map,
                    ALLEGRO_DISPLAY* display) {
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_bitmap(slimeBmp);
    al_destroy_bitmap(towerBmp);
    al_destroy_bitmap(map);
    al_destroy_display(display);
}

#endif
