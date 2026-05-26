//created by Noah Basaria and Adam Jurewicz


#ifndef BULLET_H
#define BULLET_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "enemy.h"
#include "hud.h"
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
inline void updateBullets(Bullet bullets[], int* bulletCount, Slime slimes[], int slimeCount, int* gold) {
    for (int i = 0; i < *bulletCount; i++) { //this simply loops through every bullet and updates its position based on the velocity
        Bullet& bullet = bullets[i]; //reference to the bullet so we can use dot syntax and still edit the real bullet, not a copy
        if (!bullet.alive) continue;
        bullet.x += bullet.vx; //moves the bullet
        bullet.y += bullet.vy;
        if (bullet.x < 0 || bullet.y < 0 || bullet.x > screenW || bullet.y > screenH) { //if the bullet goes off screen we destroy it
            bullet.alive = false;
            continue;
        } //this part is responsible for checking if the bullet hits the slime. a hit is registered if the distance squared between the bullet and slime is less than 20 pixels squared
        for (int j = 0; j < slimeCount; j++) {
            if (slimes[j].done) continue;
            if (dist2(bullet.x, bullet.y, slimes[j].x, slimes[j].y) < 20.0f * 20.0f) {
                slimes[j].hp -= bullet.damage;
                if (slimes[j].hp <= 0) {
                    slimes[j].done = true;
                    *gold += goldPerKill;
                } //adds gold to the player when they kill a slime had to add a * so that we could modify the gold variable that was declared in main
                bullet.alive = false;
                break;
            }
        }
    }
    //So this code here is a way to keep our bullets array clean from dead bullets
    //at first after running a long time my bullets would stop appearing, the reason was because my array was full
    //at first I did a simple moving the array down one every time a bullet died
    //this worked fine untill I added more drakes to the game, our game started lagging and running poorly 
    //this is because moving the array down is extremely inefficent when yoou have hundreds of bullets
    //this version is much more efficent because it doesnt move the whole array down but just put new bullet in the dead bullets place
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
