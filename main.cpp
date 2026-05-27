//created by Noah Basaria and Adam Jurewicz

#include <allegro5/allegro.h>
#include "function.h"
#include "waveload.h"

//int main function to run the game
int main(int argc, char *argv[]) {
    if (!initAllegro()) return -1;

    // creates a display and loads the bitmaps for the map, tower, and slime
    // if error, error message pops up
    ALLEGRO_DISPLAY *display = createDisplay();
    if (!display) return -1;

    //bitmaps for the map, tower, slime, weeknd and microphone projectile
    ALLEGRO_BITMAP *image = nullptr, *drakeBmp = nullptr, *slimeBmp = nullptr, *weekndBmp = nullptr, *microphoneBmp = nullptr, *heartBmp = nullptr;
    if (!Images(display, image, drakeBmp, slimeBmp, weekndBmp, microphoneBmp, heartBmp)) {
        al_destroy_display(display);
        return -1;
    }

    // creates timer
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    eventQueue(event_queue, display, timer);

    loadPathFromMap(image); // loads the slime path from the map bitmap

    Tower towers[maxTowers];
    TowerState towerStates[maxTowers];
    ALLEGRO_FONT* font = al_create_builtin_font();

    // game state variables
    int towerCount = 0;
    int gold = 500; // starting amount of gold
    int playerHealth = 20; // player's health
    bool drakeSelected = false;
    bool weekndSelected = false;
    int selectedTowerIndex = -1; // -1 means no tower is currently selected
    int drakeBmpW = al_get_bitmap_width(drakeBmp);
    int drakeBmpH = al_get_bitmap_height(drakeBmp);
    int weekndBmpW = al_get_bitmap_width(weekndBmp);
    int weekndBmpH = al_get_bitmap_height(weekndBmp);

    // arrays for slimes and bullets, and variables for wave manaagement
    Slime slimes[maxSlimes];
    int slimeCount = 0;
    Bullet bullets[maxBullets];
    int bulletCount = 0;
    bool running = true;
    int mouseX = 0, mouseY = 0;
    //tracks where the mouse is so we can draw the tower preview at the cursor
    Wave allWaves[maxWaves] = {};
    int waveCount = loadWaves("waves.txt", allWaves, maxWaves);

    int currentWave = 0;
    int spawnIndex = 0;
    int frameCount = 0;
    int nextSpawnIn = 0;
    bool betweenWaves = true;
    const int waveDelay = 300; // delay in frames between waves
    

    al_start_timer(timer);

    // while loop to run the game
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (event.mouse.button == 1 && drakeButtonPressed(event.mouse.x, event.mouse.y)) {
                drakeSelected = !drakeSelected;
                if (drakeSelected) weekndSelected = false;
                selectedTowerIndex = -1;
            } else if (event.mouse.button == 1 && weekndButtonPressed(event.mouse.x, event.mouse.y)) {
                weekndSelected = !weekndSelected;
                if (weekndSelected) drakeSelected = false;
                selectedTowerIndex = -1;
            } else if (event.mouse.button == 1) {
                int clicked = selectTower(event.mouse.x, event.mouse.y, towers, towerCount);
                if (clicked >= 0) {
                    if (clicked == selectedTowerIndex) {
                        selectedTowerIndex = -1;
                    } else {
                        selectedTowerIndex = clicked;
                        drakeSelected = false;
                        weekndSelected = false;
                    }
                } else {
                    selectedTowerIndex = -1;
                    handleMouseClick(event, towers, towerCount, image, drakeBmp, drakeBmpW, drakeBmpH, weekndBmpW, weekndBmpH, gold, drakeSelected, weekndSelected, drakeSelected ? drakeCost : weekndCost);
                }
            }
        }

        //updates the saved mouse position every time the cursor moves
        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouseX = event.mouse.x;
            mouseY = event.mouse.y;
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            frameCount++;

            if (betweenWaves) {
                if (frameCount >= waveDelay && currentWave < waveCount) {
                    spawnIndex = 0;
                    nextSpawnIn = 0;
                    betweenWaves = false;
                    frameCount = 0;
                }
            } else {
                if (spawnIndex < allWaves[currentWave].spawnCount) {
                    if (nextSpawnIn <= 0) {
                        Spawn& s = allWaves[currentWave].spawns[spawnIndex];
                        if (slimeCount < maxSlimes)
                            slimes[slimeCount++] = initSlime(s.bitmap, s.hp, s.speed);
                        nextSpawnIn = s.spawnInterval;
                        spawnIndex++;
                    } else {
                        nextSpawnIn--;
                    }
                } else {
                    bool allDone = true;
                    for (int i = 0; i < slimeCount; i++)
                        if (!slimes[i].done) { allDone = false; break; }
                    if (allDone) {
                        currentWave++;
                        betweenWaves = true;
                        frameCount = 0;
                        slimeCount = 0;
                    }
                }
            }

            //for loop to update the enemies, towers, and bullets, then draw them on the screen
            for (int i = 0; i < slimeCount; i++) {
                updateSlime(slimes[i]);
                if (slimes[i].escaped) {
                    playerHealth--;
                    slimes[i].escaped = false;
                    if (playerHealth <=0) {
                        // game over b/c health is 0
                        running = false;
                    }
                }
            }
            updateTowers(towers, towerStates, towerCount, slimes, slimeCount, bullets, &bulletCount, microphoneBmp);
            updateBullets(bullets, &bulletCount, slimes, slimeCount, &gold);

            // draws everthing
            al_draw_bitmap(image, 0, 0, 0);
            for (int i = 0; i < towerCount; i++) {
                ALLEGRO_BITMAP* sprite;
                int sw, sh;
                if (towers[i].type == TOWER_WEEKND) {
                    sprite = weekndBmp; sw = weekndBmpW; sh = weekndBmpH;
                } else {
                    sprite = drakeBmp;  sw = drakeBmpW;  sh = drakeBmpH;
                }
                al_draw_scaled_bitmap(sprite, 0, 0, sw, sh, towers[i].x, towers[i].y, towers[i].w, towers[i].h, 0);
            }
            highlightTower(towers, selectedTowerIndex, drakeBmp, drakeBmpW, drakeBmpH, weekndBmp, weekndBmpW, weekndBmpH);
            for (int i = 0; i < slimeCount; i++) drawSlime(slimes[i]);
            drawBullets(bullets, bulletCount);
            //draws the ghost tower and range circle under the hud so the hud always stays on top
            towerPlacement(drakeBmp, drakeBmpW, drakeBmpH, weekndBmp, weekndBmpW, weekndBmpH, mouseX, mouseY, image, towers, towerCount, gold, drakeSelected, weekndSelected);
            drawHud(font, gold, towerCount, drakeSelected, weekndSelected, playerHealth, heartBmp);
            placeDrakeButton(font, drakeSelected);
            placeWeekndButton(font, weekndSelected);
            al_flip_display();
        }
    }

    // cleans up the program resources at the end of the program
    al_destroy_font(font);
    deleteAllegro(timer, event_queue, slimeBmp, drakeBmp, weekndBmp, microphoneBmp, image, display);
    return 0;
}
