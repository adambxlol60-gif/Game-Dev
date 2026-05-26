//created by Noah Basaria and Adam Jurewicz

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
            "Couldnt install mouse", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }
    if (!al_init_image_addon()) {
        al_show_native_message_box(nullptr, "Error", "Error",
            "Error with image addons", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }
    if (!al_init_primitives_addon()) {
    al_show_native_message_box(nullptr, "Error", "Error",
        "Error with primtives addons", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
    return false;
    }
    return true;
}
//creates the display the game runs in
inline ALLEGRO_DISPLAY* createDisplay() {
    ALLEGRO_DISPLAY* display = al_create_display(screenW, screenH);
    if (!display) {
        al_show_native_message_box(nullptr, "Error", "Error",
            "Dispaly couldnt load", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        return nullptr;
    }
    al_set_window_title(display, "Tower Defense");
    al_show_mouse_cursor(display);
    al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);
    return display;
}
//list of every image file the game needs at startup
//to add a new image just put one new line in here, nothing else to change
inline const char* imageNames[] = {"Images/BetaMap.png", "Images/DrakeTower.png", "Images/Slime.png", "Images/Weeknd.png", "Images/microphone.png", "Images/blueSlime.png","Images/blueCamoSlime.png","Images/greenCamoSlime.png","Images/metalSlime.png", "Images/purpleSlime.png",  "Images/purpleCamoSlime.png", "Images/rainbowSlime.png",  "Images/rainbowCamoSlime.png",  "Images/redSlime.png", "Images/redCamoSlime.png", "Images/yellowSlime.png", "Images/yellowCamoSlime.png",};
inline const int imageCount = (int)(sizeof(imageNames) / sizeof(imageNames[0]));

inline ALLEGRO_BITMAP* bitmaps[imageCount] = {0};

//Checks each image if it runs, if it runs it saves it to a bitmap array if it doesnt error
//have to keep drakeBmp, slimeBmp and map as separate variables because they are used in many different places and it would be a pain to change all of them
inline bool Images(ALLEGRO_DISPLAY* display, ALLEGRO_BITMAP*& map, ALLEGRO_BITMAP*& drakeBmp, ALLEGRO_BITMAP*& slimeBmp, ALLEGRO_BITMAP*& weekndBmp, ALLEGRO_BITMAP*& microphoneBmp) {
    for (int i = 0; i < imageCount; i++) {
        bitmaps[i] = al_load_bitmap(imageNames[i]);
        if (!bitmaps[i]) {
            al_show_native_message_box(display, "Error", "Error", imageNames[i], nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            for (int j = 0; j < i; j++) al_destroy_bitmap(bitmaps[j]);
            return false;
        }
    }
    map           = bitmaps[0];
    drakeBmp      = bitmaps[1];
    slimeBmp      = bitmaps[2];
    weekndBmp     = bitmaps[3];
    microphoneBmp = bitmaps[4];
    return true;
}
//setups a event queue for the display, mouse, and timer.
// the display one is used to check if the player clicks the x button to close the game
//the mouse one is used to check for mouse clicks to place towers
//the timer one is used to update the game state every frame
inline void eventQueue(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_DISPLAY* display, ALLEGRO_TIMER* timer) {
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
}
//function to clean up the allegro resources, it destroys the timer, event queue, bitmaps, and display when game is closed
inline void deleteAllegro(ALLEGRO_TIMER* timer, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_BITMAP* slimeBmp, ALLEGRO_BITMAP* drakeBmp, ALLEGRO_BITMAP* weekndBmp, ALLEGRO_BITMAP* microphoneBmp, ALLEGRO_BITMAP* map, ALLEGRO_DISPLAY* display) {
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_bitmap(slimeBmp);
    al_destroy_bitmap(drakeBmp);
    al_destroy_bitmap(weekndBmp);
    al_destroy_bitmap(microphoneBmp);
    al_destroy_bitmap(map);
    al_destroy_display(display);
}


#endif
