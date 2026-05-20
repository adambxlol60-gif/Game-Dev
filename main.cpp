 #include <allegro5/allegro.h>
    #include <allegro5/allegro_image.h>
    #include <allegro5/allegro_native_dialog.h>
    #include <vector>
    #include <cmath>
    #include "enemy.h"
    #include "function.h"

    const int SCREEN_W = 1280;
    const int SCREEN_H = 960;

    struct Tower {
        float x;
        float y;
        float w;
        float h;
    };

    int main(int argc, char *argv[]) {
        const float TOWER_SCALE = 0.2f;
        if (!al_init()) return -1;

        al_init_native_dialog_addon();

        if (!al_install_mouse()) {
            al_show_native_message_box(nullptr, "Error", "Error",
                "Failed to install mouse!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            return -1;
        }

        if (!al_init_image_addon()) {
            al_show_native_message_box(nullptr, "Error", "Error",
                "Failed to initialize image addon!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            return -1;
        }

        ALLEGRO_DISPLAY *display = al_create_display(SCREEN_W, SCREEN_H);
        if (!display) {
            al_show_native_message_box(nullptr, "Error", "Error",
                "Failed to initialize display!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            return -1;
        }

        al_set_window_title(display, "Tower Defense");
        al_show_mouse_cursor(display);
        al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);

        ALLEGRO_BITMAP *image = al_load_bitmap("Images/BetaMap.png");
        if (!image) {
            al_show_native_message_box(display, "Error", "Error",
                "Failed to load BetaMap.png!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            al_destroy_display(display);
            return -1;
        }

        ALLEGRO_BITMAP *drakeTower = al_load_bitmap("Images/DrakeTower.png");
        if (!drakeTower) {
            al_show_native_message_box(display, "Error", "Error",
                "Failed to load DrakeTower.png!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            al_destroy_bitmap(image);
            al_destroy_display(display);
            return -1;
        }

        ALLEGRO_BITMAP *slimeBmp = al_load_bitmap("Images/Slime.png");
        if (!slimeBmp) {
            al_show_native_message_box(display, "Error", "Error",
                "Failed to load Slime.png!", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
            al_destroy_bitmap(drakeTower);
            al_destroy_bitmap(image);
            al_destroy_display(display);
            return -1;
        }

        ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
        ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    
        al_register_event_source(event_queue, al_get_display_event_source(display));
        al_register_event_source(event_queue, al_get_mouse_event_source());
        al_register_event_source(event_queue, al_get_timer_event_source(timer));

    loadPathFromMap(image);
    Slime slime = initSlime(slimeBmp);
    std::vector<Tower> towers;
    bool running = true;

        al_start_timer(timer);

        while (running) {
            ALLEGRO_EVENT event;
            al_wait_for_event(event_queue, &event);

            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                running = false;
            }

            if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        if (event.mouse.button == 1) {
            float towerW = al_get_bitmap_width(drakeTower) * TOWER_SCALE;
            float towerH = al_get_bitmap_height(drakeTower) * TOWER_SCALE;

            Tower newTower;
            newTower.x = event.mouse.x - towerW / 2;
            newTower.y = event.mouse.y - towerH / 2;

            if (!onPath(image, event.mouse.x, event.mouse.y)) {
                towers.push_back(newTower);
            }
        }
}

            if (event.type == ALLEGRO_EVENT_TIMER) {
                updateSlime(slime);

                al_draw_bitmap(image, 0, 0, 0);
                for (Tower tower : towers) {
                    al_draw_scaled_bitmap(drakeTower, 0, 0, al_get_bitmap_width(drakeTower), al_get_bitmap_height(drakeTower), tower.x, tower.y, al_get_bitmap_width(drakeTower) * TOWER_SCALE, al_get_bitmap_height(drakeTower) * TOWER_SCALE, 0);
                }
                drawSlime(slime);
                al_flip_display();
            }
        }

        al_destroy_timer(timer);
        al_destroy_event_queue(event_queue);
        al_destroy_bitmap(slimeBmp);
        al_destroy_bitmap(drakeTower);
        al_destroy_bitmap(image);
        al_destroy_display(display);

        return 0;
    }
