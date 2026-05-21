#include "function.h"

int main(int argc, char* argv[]) {
    if (!initAllegro()) return -1;

    ALLEGRO_DISPLAY* display = createDisplay();
    if (!display) return -1;

    ALLEGRO_BITMAP *map, *drakeTower, *slimeBmp;
    if (!loadBitmaps(display, map, drakeTower, slimeBmp)) {
        al_destroy_display(display);
        return -1;
    }

    const int drakeW = al_get_bitmap_width(drakeTower);
    const int drakeH = al_get_bitmap_height(drakeTower);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    setupEventQueue(queue, display, timer);

    loadPathFromMap(map);
    Slime slime = initSlime(slimeBmp);
    Tower towers[MAX_TOWERS];
    int towerCount = 0;
    bool running = true;
    bool redraw = false;

    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            running = false;

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
            handleMouseClick(event, towers, towerCount, map, drakeTower, drakeW, drakeH);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            updateSlime(slime);
            redraw = true;
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            renderFrame(map, drakeTower, towers, towerCount, drakeW, drakeH, slime);
        }
    }

    cleanup(timer, queue, slimeBmp, drakeTower, map, display);
    return 0;
}