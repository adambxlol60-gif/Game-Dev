//created by Noah Basaria and Adam Jurewicz

#include <allegro5/allegro.h>
#include "function.h"

//int main function to run the game
int main(int argc, char *argv[]) {
    if (!initAllegro()) return -1;

    // creates a display and loads the bitmaps for the map, tower, and slime
    // if error, error message pops up
    ALLEGRO_DISPLAY *display = createDisplay();
    if (!display) return -1;

    //bitmaps for the map, tower, and slime
    ALLEGRO_BITMAP *image = nullptr, *towerBmp = nullptr, *slimeBmp = nullptr;
    if (!Images(display, image, towerBmp, slimeBmp)) {
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
    int towerBmpW = al_get_bitmap_width(towerBmp);
    int towerBmpH = al_get_bitmap_height(towerBmp);

    // arrays for slimes and bullets, and variables for wave manaagement
    Slime slimes[maxSlimes];
    int slimeCount = 0;
    Bullet bullets[maxBullets];
    int bulletCount = 0;
    bool running = true;
    //tracks where the mouse is so we can draw the tower preview at the cursor
    int mouseX = 0, mouseY = 0;
    int currentWave = 0;
    int enemiesInWave = 0;
    int enemiesSpawned = 0;
    int frameCount = 0;
    const int spawnInterval = 60;
    const int waveDelay = 300;
    bool betweenWaves = true;

    al_start_timer(timer);

    // while loop to run the game
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            handleMouseClick(event, towers, towerCount, image, towerBmp, towerBmpW, towerBmpH, gold);
        }

        //updates the saved mouse position every time the cursor moves
        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouseX = event.mouse.x;
            mouseY = event.mouse.y;
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            frameCount++;

            if (betweenWaves) {
                if (frameCount >= waveDelay) {
                    currentWave++;
                    enemiesInWave = 5 + (currentWave - 1) * 2;
                    enemiesSpawned = 0;
                    betweenWaves = false;
                    frameCount = 0;
                }
            } else {
                if (enemiesSpawned < enemiesInWave && frameCount >= spawnInterval) {
                    if (slimeCount < maxSlimes) slimes[slimeCount++] = initSlime(slimeBmp);
                    enemiesSpawned++;
                    frameCount = 0;
                }
                if (enemiesSpawned >= enemiesInWave) {
                    bool allDone = true;
                    for (int i = 0; i < slimeCount; i++)
                        if (!slimes[i].done) { allDone = false; break; }
                    if (allDone) {
                        betweenWaves = true;
                        frameCount = 0;
                        slimeCount = 0;
                    }
                }
            }

            //for loop to update the enemies, towers, and bullets, then draw them on the screen
            for (int i = 0; i < slimeCount; i++) updateSlime(slimes[i]);
            updateTowers(towers, towerStates, towerCount, slimes, slimeCount, bullets, &bulletCount);
            updateBullets(bullets, &bulletCount, slimes, slimeCount, &gold);

            // draws everthing
            al_draw_bitmap(image, 0, 0, 0);
            for (int i = 0; i < towerCount; i++) {
                al_draw_scaled_bitmap(towerBmp, 0, 0, towerBmpW, towerBmpH,
                    towers[i].x, towers[i].y, towers[i].w, towers[i].h, 0);
            }
            for (int i = 0; i < slimeCount; i++) drawSlime(slimes[i]);
            drawBullets(bullets, bulletCount);
            //draws the ghost tower and range circle under the hud so the hud always stays on top
            towerPlacement(towerBmp, towerBmpW, towerBmpH, mouseX, mouseY, image, towers, towerCount, gold);
            drawHud(font, gold, towerCount);
            al_flip_display();
        }
    }

    // cleans up the program resources at the end of the program
    al_destroy_font(font);
    deleteAllegro(timer, event_queue, slimeBmp, towerBmp, image, display);
    return 0;
}
