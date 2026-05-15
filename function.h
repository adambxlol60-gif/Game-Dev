#ifndef FUNCTIONS_H
    #define FUNCTIONS_H

    #include <allegro5/allegro.h>

    inline bool onPath(ALLEGRO_BITMAP* map, int mouseX, int mouseY) {
        ALLEGRO_COLOR color = al_get_pixel(map, mouseX, mouseY);


        unsigned char r, g, b;
        al_unmap_rgb(color, &r, &g, &b);

        // Yellow / orange path color
        if (r > 160 && g > 120 && b < 120) {
            return true;
        }

        return false;
    }

   inline bool towerCollides(ALLEGRO_BITMAP* map, float x, float y, float w, float h) {
    int startX = static_cast<int>(x);
    int startY = static_cast<int>(y);
    int endX = static_cast<int>(x + w);
    int endY = static_cast<int>(y + h);

    for (int i = startX; i < endX; i++) {
        for (int j = startY; j < endY; j++) {
            if (onPath(map, i, j)) {
                return true;
            }
        }
    }

    return false;
}

    #endif