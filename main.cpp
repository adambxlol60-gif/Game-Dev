#include <allegro5/allegro.h>
#include "function.h"

//int main function to run the game
int main(int argc, char *argv[]) {
    if (!initAllegro()) return -1;

    ALLEGRO_DISPLAY *display = createDisplay();
    if (!display) return -1;

    ALLEGRO_BITMAP *image = nullptr, *drakeTower = nullptr, *slimeBmp = nullptr;
    if (!loadBitmaps(display, image, drakeTower, slimeBmp)) {
        al_destroy_display(display);
        return -1;
    }

    // creates timer
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    setupEventQueue(event_queue, display, timer);

    loadPathFromMap(image); // loads the slime path from the map bitmap

    Tower towers[MAX_TOWERS];
    TowerState towerStates[MAX_TOWERS];
    ALLEGRO_FONT* font = al_create_builtin_font();
   
    // game state variables
    int towerCount = 0;
    int gold = 500; // starting amount of gold
    int drakeW = al_get_bitmap_width(drakeTower);
    int drakeH = al_get_bitmap_height(drakeTower);

    // arrays for slimes and bullets, and variables for wave manaagement
    Slime slimes[MAX_SLIMES];
    int slimeCount = 0;
    Bullet bullets[MAX_BULLETS];
    int bulletCount = 0;
    bool running = true;
    int currentWave = 0;
    int enemiesInWave = 0;
    int enemiesSpawned = 0;
    int frameCount = 0;
    const int SPAWN_INTERVAL = 60;
    const int WAVE_DELAY = 300;
    bool betweenWaves = true;

    al_start_timer(timer);
    
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            handleMouseClick(event, towers, towerCount, image, drakeTower, drakeW, drakeH, gold);
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            frameCount++;

            if (betweenWaves) {
                if (frameCount >= WAVE_DELAY) {
                    currentWave++;
                    enemiesInWave = 5 + (currentWave - 1) * 2;
                    enemiesSpawned = 0;
                    betweenWaves = false;
                    frameCount = 0;
                }
            } else {
                if (enemiesSpawned < enemiesInWave && frameCount >= SPAWN_INTERVAL) {
                    if (slimeCount < MAX_SLIMES) slimes[slimeCount++] = initSlime(slimeBmp);
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

            for (int i = 0; i < slimeCount; i++) updateSlime(slimes[i]);
            updateTowers(towers, towerStates, towerCount, slimes, slimeCount, bullets, &bulletCount);
            updateBullets(bullets, &bulletCount, slimes, slimeCount);

            al_draw_bitmap(image, 0, 0, 0);
            for (int i = 0; i < towerCount; i++) {
                al_draw_scaled_bitmap(drakeTower, 0, 0, drakeW, drakeH,
                    towers[i].x, towers[i].y, towers[i].w, towers[i].h, 0);
            }
            for (int i = 0; i < slimeCount; i++) drawSlime(slimes[i]);
            drawBullets(bullets, bulletCount);
            drawHud(font, gold, towerCount);
            al_flip_display();
        }
    }

    al_destroy_font(font);
    cleanup(timer, event_queue, slimeBmp, drakeTower, image, display);
    return 0;
}
