#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <vector>
#include "enemy.h"
#include "function.h"

int main(int argc, char *argv[]) {
    if (!initAllegro()) return -1;

    ALLEGRO_DISPLAY *display = createDisplay();
    if (!display) return -1;

    ALLEGRO_BITMAP *image = nullptr, *drakeTower = nullptr, *slimeBmp = nullptr;
    if (!loadBitmaps(display, image, drakeTower, slimeBmp)) {
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    setupEventQueue(event_queue, display, timer);

    loadPathFromMap(image);

    Tower towers[MAX_TOWERS];
    int towerCount = 0;
    int drakeW = al_get_bitmap_width(drakeTower);
    int drakeH = al_get_bitmap_height(drakeTower);

    std::vector<Slime> slimes;
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
            handleMouseClick(event, towers, towerCount, image, drakeTower, drakeW, drakeH);
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
                    slimes.push_back(initSlime(slimeBmp));
                    enemiesSpawned++;
                    frameCount = 0;
                }
                if (enemiesSpawned >= enemiesInWave) {
                    bool allDone = true;
                    for (const Slime& s : slimes)
                        if (!s.done) { allDone = false; break; }
                    if (allDone) {
                        betweenWaves = true;
                        frameCount = 0;
                        slimes.clear();
                    }
                }
            }

            for (Slime& s : slimes) updateSlime(s);

            al_draw_bitmap(image, 0, 0, 0);
            for (int i = 0; i < towerCount; i++) {
                al_draw_scaled_bitmap(drakeTower, 0, 0, drakeW, drakeH,
                    towers[i].x, towers[i].y, towers[i].w, towers[i].h, 0);
            }
            for (Slime& s : slimes) drawSlime(s);
            al_flip_display();
        }
    }

    cleanup(timer, event_queue, slimeBmp, drakeTower, image, display);
    return 0;
}
