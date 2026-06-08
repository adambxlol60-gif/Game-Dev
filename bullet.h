//created by Noah Basaria and Adam Jurewicz
#ifndef BULLET_H //tutorial to explain what ifndef does and why we use it https://www.scaler.com/topics/ifndef-cpp/
#define BULLET_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "enemy.h"
#include "hud.h"

//maximum amount of bullet the screen can have at once, this array says you can have max 500 alive bullets at once.
const int maxBullets = 500;

// bullet speed (kept here with the rest of the bullet data)
const float bulletSpeed = 14.0f;

//varaible for the bullet, saves stuff like velocity, damage, pierce count, ice , explosive damange, and other stuff.
//It works with the file tower.h which creates the bullet and gives them variable based on which tower shoots them.
//There is also the velocity of the bullet using the quadratic equation
const int maxHitsPerBullet = 8;
struct Bullet {
    float x, y;
    float vx, vy;
    int damage;
    int pierce;
    bool alive;
    int ice;
    bool armorPiercing = false; //if true, bullet can hit metal slimes
    int explosive = 0;         //blast radius in pixels. 0 = no explosion
    int explosiveDamage = 0;   //damage dealt to every OTHER slime inside the blast radius
    float spriteScale = 0.18f;   //how big the sprite draws; rockets override this
    ALLEGRO_BITMAP* sprite = nullptr; //special bullet model like the weeknd microphone
    //tracks which slimes this bullet already hit so pierce moves on to NEW enemies instead of hitting the same one every frame
    int hitSlimes[maxHitsPerBullet];
    int hitCount = 0;
};
//calcuate distance between two points squared
//I found this article which says that square rooting like in the pythagorem theorum is useless for comparing distances and makes the cpu and gpu work more for zero benefit
//https://mflerackers.wordpress.com/2013/07/07/optimization-tip-dont-use-sqrt-when-comparing-distances/
//https://gamedev.net/forums/topic/660273-distance-formula-in-c/5176037/
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
//Using pass by refrence here is super important, if we didnt use pass by refrence the bullets would not move because we would be editing a copy of the bullet instead of the real bullet in the array
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

        //check every slime for a hit, comparing distance squared to skip the waste sqrt calculation explained in the previous comments
        for (int j = 0; j < slimeCount; j++) {
            if (slimes[j].done) continue;
            if (bulletAlreadyHit(bullet, j)) continue;
            if (distance(bullet.x, bullet.y, slimes[j].x, slimes[j].y) >= 20.0f * 20.0f) continue;

            //if a bullet touches a metal slime without armor piercing it dies and does no damage
            if (slimes[j].metal && !bullet.armorPiercing) {
                bullet.alive = false;
                break;
            }

            //hit confirmed apply damage and award gold if it died
            slimes[j].hp -= bullet.damage;
            slimes[j].hitDamageTimer = 6; //flash red for 6 frames this is to let the player know which slimes are getting hit.
            if (slimes[j].hp <= 0) {
                slimes[j].done = true;
                *gold += slimes[j].maxHp; //1 hp = 1 gold
                if (slimes[j].splitCount > 0)
                slimes[j].pendingSplit = true;
            }
            //this is for ice bu;lets which slow down slimes
            if (bullet.ice > 0 && !slimes[j].isIced) {
                slimes[j].speed *= 0.5f;
                slimes[j].isIced = true;
            }

            //this if statement is for explosive bullets, it checks if the any slime is in the x y radius of the explosion and if they are it applies explosive damage to them
            if (bullet.explosive > 0) {
                float radius2 = (float)bullet.explosive * bullet.explosive;
                for (int k = 0; k < slimeCount; k++) {
                    if (k == j) continue; //skips slimes that got a direct hit.
                    if (slimes[k].done) continue;
                    if (distance(bullet.x, bullet.y, slimes[k].x, slimes[k].y) <= radius2) {
                        slimes[k].hp -= bullet.explosiveDamage;
                        slimes[k].hitDamageTimer = 6; //same flashing red animation 
                        if (slimes[k].hp <= 0) {
                            slimes[k].done = true;
                            *gold += slimes[k].maxHp;   //1 hp = 1 gold
                        }
                    }
                }
            }

            //this is for piercing bullets so that they dont hit the same slime every frame
            //it uses a lot of similiar sounding variables so clarifying the code is pretty important
            //bullet.hitCount is how many slimes this bullet has hit so far, it starts at 0 and goes up to maxHitsPerBullet which is 8 (not any specific reason for this number)
            
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
    //https://www.studyplan.dev/structure-of-arrays/swap-and-pop this guide explained this method in more detail
    //https://gamedev.net/forums/topic/665250-efficient-way-to-erase-an-element-from-stdvector/5207368/
    for (int i = 0; i < *bulletCount; ) {
        if (!bullets[i].alive)
            bullets[i] = bullets[--(*bulletCount)];
        else
            i++;
    }
}
//this functions draws bullets to the screen, it skips dead bullets and bullets with no sprite 
//each bullwt has a different scale so they look similiar size when shooting
inline void drawBullets(const Bullet bullets[], int bulletCount) {
    for (int i = 0; i < bulletCount; i++) {
        if (!bullets[i].alive || !bullets[i].sprite) continue;
        int sw = al_get_bitmap_width(bullets[i].sprite);
        int sh = al_get_bitmap_height(bullets[i].sprite);
        float scale = bullets[i].spriteScale;
        al_draw_scaled_bitmap(bullets[i].sprite, 0, 0, sw, sh,
            bullets[i].x - (sw * scale) / 2,
            bullets[i].y - (sh * scale) / 2,
            sw * scale, sh * scale, 0);
    }
}
#endif