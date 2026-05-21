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
    ALLEGRO_FONT *font = al_create_builtin_font();

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    setupEventQueue(event_queue, display, timer);

    loadPathFromMap(image);

    Tower towers[MAX_TOWERS];
    int towerCount = 0;
    int money = 1000;
    //drake cost
    const int DrakeCost = 100;
    //max quant of drakes
    const int MaxDrake = 10;
    int drakeW = al_get_bitmap_width(drakeTower);
    int drakeH = al_get_bitmap_height(drakeTower);

    std::vector<Slime> slimes;
    bool running = true;
    int frameCount = 0;
    const int SPAWN_INTERVAL = 60 * 3;

    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (money >= DrakeCost && towerCount < MaxDrake) {
                int prevCount = towerCount;
                handleMouseClick(event, towers, towerCount, image, drakeTower, drakeW, drakeH);
                if (towerCount > prevCount) money -= DrakeCost;
            }
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            frameCount++;
            if (frameCount >= SPAWN_INTERVAL) {
                slimes.push_back(initSlime(slimeBmp));
                frameCount = 0;
            }

            for (Slime& s : slimes) updateSlime(s);

            al_draw_bitmap(image, 0, 0, 0);
            for (int i = 0; i < towerCount; i++) {
                al_draw_scaled_bitmap(drakeTower, 0, 0, drakeW, drakeH,
                    towers[i].x, towers[i].y, towers[i].w, towers[i].h, 0);
            }
            for (Slime& s : slimes) drawSlime(s);
           al_draw_filled_rectangle(0, 0, SCREEN_W, 50, al_map_rgba(0, 0, 0, 200));
al_draw_textf(font, al_map_rgb(255, 215, 0), 20, 17, 0, "Money: $%d", money);
al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_W / 2 - 80, 17, 0, "Drake Tower: $%d", DrakeCost);
al_draw_textf(font, al_map_rgb(200, 200, 200), SCREEN_W - 200, 17, 0, "Towers: %d/%d", towerCount, MaxDrake);
            al_flip_display();
        }
    }

    al_destroy_font(font);
    cleanup(timer, event_queue, slimeBmp, drakeTower, image, display);
    return 0;
}
