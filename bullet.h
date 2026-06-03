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

//varaible for the bullet, saves stuff like velocity, damage, pierce count, whether it's alive, and optional sprite (nullptr = yellow circle)
const int maxHitsPerBullet = 8;
struct Bullet {
    float x, y;
    float vx, vy;
    int damage;
    int pierce;
    bool alive;
    int ice;
    int explosive = 0;         //blast radius in pixels. 0 = no explosion
    int explosiveDamage = 0;   //damage dealt to every OTHER slime inside the blast radius
    float spriteScale = 0.18f;   //how big the sprite draws; rockets override this
    ALLEGRO_BITMAP* sprite = nullptr;
    //tracks which slimes this bullet already hit so pierce moves on to NEW enemies instead of hitting the same one every frame
    int hitSlimes[maxHitsPerBullet];
    int hitCount = 0;
};
//calcuate distance between two points squared
inline float distance(float ax, float ay, float bx, float by) {
    float deltaX = ax - bx, deltaY = ay - by;
    return deltaX*deltaX + deltaY*deltaY;
}

//returns true if this bullet has already damaged slime index j on a previous frame
inline bool bulletAlreadyHit(const Bullet& bullet, int j) {
    for (int k = 0; k < bullet.hitCount; k++) {
        if (bullet.hitSlimes[k] == j) return true;
    }
    return false;
}


//this function updates the bullets position and then checks for a collision with the slimes.
//It also deletes all the off screen bullets
inline void updateBullets(Bullet bullets[], int* bulletCount, Slime slimes[], int slimeCount, int* gold) {
    for (int i = 0; i < *bulletCount; i++) {
        Bullet& bullet = bullets[i]; //reference so we edit the real bullet, not a copy
        if (!bullet.alive) continue;

        //move the bullet by its velocity
        bullet.x += bullet.vx;
        bullet.y += bullet.vy;

        //off screen = dead
        if (bullet.x < 0 || bullet.y < 0 || bullet.x > screenW || bullet.y > screenH) {
            bullet.alive = false;
            continue;
        }

        //check every slime for a hit, comparing distance squared to skip a slow sqrt
        for (int j = 0; j < slimeCount; j++) {
            if (slimes[j].done) continue;
            if (bulletAlreadyHit(bullet, j)) continue;
            if (distance(bullet.x, bullet.y, slimes[j].x, slimes[j].y) >= 20.0f * 20.0f) continue;

            //hit confirmed - apply damage and award gold if it died
            slimes[j].hp -= bullet.damage;
            if (slimes[j].hp <= 0) {
                slimes[j].done = true;  
                *gold += goldPerKill;
            }
            //ice bullet slows the slime, but only once per slime ever (no stacking from other bullets)
            if (bullet.ice > 0 && !slimes[j].isIced) {
                slimes[j].speed *= 0.5f;
                slimes[j].isIced = true;
            }

            //explosive bullet - every OTHER slime inside the blast radius takes explosiveDamage
            if (bullet.explosive > 0) {
                float radius2 = (float)bullet.explosive * bullet.explosive;
                for (int k = 0; k < slimeCount; k++) {
                    if (k == j) continue;            //skip the direct hit, already damaged above
                    if (slimes[k].done) continue;
                    if (distance(bullet.x, bullet.y, slimes[k].x, slimes[k].y) <= radius2) {
                        slimes[k].hp -= bullet.explosiveDamage;
                        if (slimes[k].hp <= 0) {
                            slimes[k].done = true;
                            *gold += goldPerKill;
                        }
                    }
                }
            }

            //remember this slime so pierce moves on to a NEW enemy next frame
            if (bullet.hitCount < maxHitsPerBullet) {
                bullet.hitSlimes[bullet.hitCount++] = j;
            }

            //use a pierce charge, or die on this hit
            if (bullet.pierce > 0) bullet.pierce -= 1;
            else bullet.alive = false;
            break; 
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
//this draws bullets as either a yellow circle or the microphone sprite if its a weeknd 
inline void drawBullets(const Bullet bullets[], int bulletCount) {
    for (int i = 0; i < bulletCount; i++) {
        if (!bullets[i].alive) continue;
        if (bullets[i].sprite) {
            int sw = al_get_bitmap_width(bullets[i].sprite);
            int sh = al_get_bitmap_height(bullets[i].sprite);
            float scale = bullets[i].spriteScale;
            al_draw_scaled_bitmap(bullets[i].sprite, 0, 0, sw, sh,
                bullets[i].x - (sw * scale) / 2,
                bullets[i].y - (sh * scale) / 2,
                sw * scale, sh * scale, 0);
        } else {
            al_draw_filled_circle(bullets[i].x, bullets[i].y, 4, al_map_rgb(255, 220, 0));
        }
    }
}
#endif