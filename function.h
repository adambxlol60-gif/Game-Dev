//created by Noah Basaria and Adam Jurewicz

//we learned to use allegro with this reference: https://liballeg.org/a5docs/trunk/display.html and creelman tutorials



#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include "enemy.h"

//the screen size 4:3 ratio - must be defined before the headers below since they use it
const int screenW = 1280;
const int screenH = 960;

#include "bullet.h"
#include "hud.h"
#include "tower.h"
#include "towerPlacement.h"
#include "panel.h"



//this is the simple allegro initialization function, they also check if the mouse, image addon are initialized
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
//we used creelman tutorials for ALLEGRO_DISPLAY and al_create_display
//inline tells the compiler to copy the function code directly where it is
//we used it here because there are smalled functions called frequently
//Reference: inline functions in C++ GeeksforGeeks: https://www.geeksforgeeks.org/cpp/inline-functions-cpp/

inline ALLEGRO_DISPLAY* createDisplay() {
    ALLEGRO_DISPLAY* display = al_create_display(screenW, screenH);
    if (!display) {
        al_show_native_message_box(nullptr, "Error", "Error",
            "Display couldnt load", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        return nullptr;
    }
    al_set_window_title(display, "Tower Defense");
    al_show_mouse_cursor(display);
    al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);
    return display;
}
//this is a list of every image file the game needs at startup
inline const char* imageNames[] = {"Images/BetaMap.png", "Images/DrakeTower.png", "Images/Slime.png", "Images/Weeknd.png", "Images/microphone.png", "Images/blueSlime.png","Images/blueCamoSlime.png","Images/greenCamoSlime.png","Images/metalSlime.png", "Images/purpleSlime.png",  "Images/purpleCamoSlime.png", "Images/rainbowSlime.png",  "Images/rainbowCamoSlime.png",  "Images/redSlime.png", "Images/redCamoSlime.png", "Images/yellowSlime.png", "Images/yellowCamoSlime.png", "Images/heart.png", "Images/drakeMic.png", "Images/ElonMusk.png", "Images/DrakeIceman.png", "Images/SpaceXRocket.png", "Images/Bank.png", "Images/Starboy.png", "Images/ElomMuskUpgrade.png", "Images/titleScreen.png", "Images/playButton.png", "Images/gameOverScreen.png", "Images/WinScreen.png", "Images/retryButton.png", "Images/menuButton.png", "Images/slimeKing.png"};

//I used sizeof to figure out how many images are in the array
//Reference: C++ Get the Size of an Array - W3schools: https://www.w3schools.com/cpp/cpp_arrays_size.asp
//https://tannerdolby.com/writing/calculating-the-size-of-an-array-in-c++-using-the-sizeof-operator/
inline const int imageCount = (int)(sizeof(imageNames) / sizeof(imageNames[0]));


//We used an array to hold a pointer to each loaded image, starts as all zeros
//We made it so that each slot in the arraw doesn't actually store the image itself, but stores a pointer to where the images is in our memory
//setting it to {0} makes it so that all the pointers starts as null so that nothing is pointed at useless memory before the images are loaded
//Reference: C++ Array of Pointers - TutorialsPoint: https://www.tutorialspoint.com/cplusplus/cpp_array_of_pointers.htm
//https://www.geeksforgeeks.org/cpp/creating-array-of-pointers-in-cpp/

inline ALLEGRO_BITMAP* bitmaps[imageCount] = {0};


//we used ALLEGRO_BITMAP*& so the function can change what the pointer points to, outside the function
//normally if you pass a pointer into a function and change it, the change won't stick after the functions program ends
//adding the & fixes that, so when Images() runs, allthe parameters will get propery updated
//reference:: https://www3.ntu.edu.sg/home/ehchua/programming/cpp/cp4_PointerReference.html
//https://www.geeksforgeeks.org/cpp/passing-reference-to-a-pointer-in-c/
//https://learn.microsoft.com/en-us/cpp/cpp/references-to-pointers?view=msvc-170
inline bool Images(ALLEGRO_DISPLAY* display, ALLEGRO_BITMAP*& map, ALLEGRO_BITMAP*& drakeBitmap, ALLEGRO_BITMAP*& slimeBitmap, ALLEGRO_BITMAP*& weekndBitmap, ALLEGRO_BITMAP*& microphoneBitmap, ALLEGRO_BITMAP*& heartBitmap, ALLEGRO_BITMAP*& drakeMicBitmap, ALLEGRO_BITMAP*& elonBitmap, ALLEGRO_BITMAP*& rocketBitmap, ALLEGRO_BITMAP*& bankBitmap, ALLEGRO_BITMAP*& icemanBitmap, ALLEGRO_BITMAP*& starboyBitmap, ALLEGRO_BITMAP*& teslaBitmap) {
    for (int i = 0; i < imageCount; i++) {
        bitmaps[i] = al_load_bitmap(imageNames[i]);
        if (!bitmaps[i]) {
            al_show_native_message_box(display, "Error", "Error", imageNames[i], nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            for (int j = 0; j < i; j++) al_destroy_bitmap(bitmaps[j]);
            return false;
        }
    }
    map = bitmaps[0];
    drakeBitmap = bitmaps[1];
    slimeBitmap = bitmaps[2];
    weekndBitmap = bitmaps[3];
    microphoneBitmap = bitmaps[4];
    heartBitmap = bitmaps[17];
    drakeMicBitmap = bitmaps[18];
    elonBitmap = bitmaps[19];
    rocketBitmap = bitmaps[21];
    bankBitmap = bitmaps[22];
    icemanBitmap = bitmaps[20];
    starboyBitmap = bitmaps[23];
    teslaBitmap = bitmaps[24];
    return true;
}
//it setups a event queue for the display, mouse, and timer.
//the display one is used to check if the player clicks the x button to close the game
//the mouse one is used to check for mouse clicks to place towers
//the timer one is used to update the game state every frame
inline void eventQueue(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_DISPLAY* display, ALLEGRO_TIMER* timer) {
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
}
//this is a function to clean up the allegro resources, it destroys the timer, event queue, bitmaps, and display when game is closed
inline void deleteAllegro(ALLEGRO_TIMER* timer, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_BITMAP* slimeBitmap, ALLEGRO_BITMAP* drakeBitmap, ALLEGRO_BITMAP* weekndBitmap, ALLEGRO_BITMAP* microphoneBitmap, ALLEGRO_BITMAP* map, ALLEGRO_DISPLAY* display) {
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_bitmap(slimeBitmap);
    al_destroy_bitmap(drakeBitmap);
    al_destroy_bitmap(weekndBitmap);
    al_destroy_bitmap(microphoneBitmap);
    al_destroy_bitmap(map);
    al_destroy_display(display);
}
//this is a function to scan the mouse to see if it is next to the tower then 
inline int selectTower(int mouseX, int mouseY, Tower towers[], int towerCount) {
    for (int i = 0; i < towerCount; i++) {
        Tower model = towerModelRectangle(towers[i]);
        if (mouseX >= model.x && mouseX <= model.x + model.w && mouseY >= model.y && mouseY <= model.y + model.h) {
            return i;
        }
    }
    return -1;
}


#endif
