#ifndef BULLET_H
#define BULLET_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "enemy.h"

const int maxBullets = 500;
// bullet speed (kept here with the rest of the bullet data)
const float bulletSpeed = 14.0f;

//varaible for the bullet, saves stuff like velocity, damage, and whether it's alive or not
struct Bullet {
    float x, y;
    float vx, vy;
    int damage;
    bool alive;
};

inline float dist2(float ax, float ay, float bx, float by) {
    float dx = ax - bx, dy = ay - by;
    return dx*dx + dy*dy;
}

//this function updates the bullets position and then checks for a collision with the slimes.
//It also deletes all the off screen bullets
inline void updateBullets(Bullet bullets[], int* bulletCount, Slime slimes[], int slimeCount) {
    for (int i = 0; i < *bulletCount; i++) { //this simply loops through every bullet and updates its position based on the velocity
        Bullet* b = &bullets[i]; //if the bullet is not alive we skip it
        if (!b->alive) continue;
        b->x += b->vx; //moves the bullet
        b->y += b->vy;
        if (b->x < 0 || b->y < 0 || b->x > screenW || b->y > screenH) { //if the bullet goes off screen we destroy it
            b->alive = false;
            continue;
        } //this part is responsible for checking if the bullet hits the slime. a hit is registered if the distance squared between the bullet and slime is less than 20 pixels squared
        for (int j = 0; j < slimeCount; j++) {
            if (slimes[j].done) continue;
            if (dist2(b->x, b->y, slimes[j].x, slimes[j].y) < 20.0f * 20.0f) {
                slimes[j].hp -= b->damage;
                if (slimes[j].hp <= 0) slimes[j].done = true;
                b->alive = false;
                break;
            }
        }
    }
    //Since our bullet array is limited to 500, this loop solves the issue of having more then 500 bullets
    //What it does is it checks if there are any dead bullets (dead bullets are bullets that have hit a slime or gone of screen)
    //then an active bullet gets put into the dead bullet spot instead of taking more space
    //this is very efficient because we wont have to worry of running out of bullet space
    //the only limit with this code is that once we add larger waves more characters ex. we might have more then 500 bullets on the screen
    //We will need to increase the maxBullets variable but for now with the way the game is setup we wont have to worry
    for (int i = 0; i < *bulletCount; ) {
        if (!bullets[i].alive)
            bullets[i] = bullets[--(*bulletCount)];
        else
            i++;
    }
}
//this draws the bullet as a small circle, we will add a beter model later
inline void drawBullets(const Bullet bullets[], int bulletCount) {
    for (int i = 0; i < bulletCount; i++)
        if (bullets[i].alive)
            al_draw_filled_circle(bullets[i].x, bullets[i].y, 4, al_map_rgb(255, 220, 0));
}

#endif
