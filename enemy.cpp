#include "enemy.h"
#include <allegro5/allegro_image.h>
#include <math.h>

// path arrays and point count for slime movement
//Think of them as little dots for where the slimes are supposed to go
//pointCount is how many points have been saved onto an array
//what static does is make the float and integers only avaible for this file so no other file can use them
float pathX[10000];
float pathY[10000];
int pointCount = 0;

//this part of loadPathFromMap is used to gather info on the paths
//al_ get_bitmap_width and height as the name suggest give the height and width of the bitmap, we save them onto int w and h which stands width and height
void loadPathFromMap(ALLEGRO_BITMAP* mapBitmap) {
    int w = al_get_bitmap_width(mapBitmap);
    int h = al_get_bitmap_height(mapBitmap);
    //https://www.allegro.cc/manual/5/al_lock_bitmap
    //https://www.allegro.cc/manual/5/al_get_pixel
    al_lock_bitmap(mapBitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY); //this part right is for locking the bitmap map  for reading the pixels, then we get the pixels of the map, then we tell it the computer to use any color space and the we 
    //read only helps with preformance because we dont need to write anything on the bitmap

    //int x is x axis in w is the width we got from the bitmap
    //for this art we also used something called typecasting which basically converts one data type to another, in this file we use it to conver int x and y to floats so we can save them in the path arrays which are in floats
    //https://www.w3schools.com/cpp//cpp_type_casting.asp I used this guide to learn about this
    for (int x = 0; x < w; x += 20) {
        //int totalY is the y axis
        int totalY = 0;
        //this whole part of the code works kind of like a scanner, it moves down y axis scan every pixel and if the pixels are the yellowish color of the path
        //it adds adds the y value to total y and increases the int count
        int count = 0;
        for (int y = 0; y < h; y++) {
            ALLEGRO_COLOR c = al_get_pixel(mapBitmap, x, y);
            unsigned char r, g, b, a;
            al_unmap_rgba(c, &r, &g, &b, &a);
            if (r > 220 && g > 180 && b < 100) {
                totalY += y;
                count++;
            }
        }
        //when int count is more than 0 (the pixel matches the yellow path color) we saved it on the x axis and y axis
        if (count > 0) {
            pathX[pointCount] = (float)x;
            pathY[pointCount] = (float)(totalY / count); //for the y axis it avergaes the y value so the slime moves in the middle of the path instead of top to bottom
            pointCount++; //this is how we make our point count, now we have a bunch of little points for the slime to follow
        }
    }
    //we have our point now we can unlock the bitmap so the game can use it again.
    al_unlock_bitmap(mapBitmap);
}

//this part is to initialize the slime struct with all its variables
//the variables deal with everything from slimes attributes such as hp speed characterisits to more technical things
Slime initSlime(ALLEGRO_BITMAP* bitmap, int hp, float speed) {
    Slime s;
    s.bitmap = bitmap;
    s.x = pathX[0];
    s.y = pathY[0];
    s.vx = 0;
    s.vy = 0;
    s.speed = speed;
    s.target = 1;
    s.done = false;
    s.escaped = false;
    s.isIced = false;
    s.camo = false;
    s.metal = false;
    s.pendingSplit = false;
    s.splitCount = 0;
    s.splitHp = 0;
    s.splitSpeed = 0.0f;
    s.splitBitmap = nullptr;
    s.hp = hp;
    s.maxHp = hp;
    return s;
}

//rthis void function is used to update the slime position and target
//it also includes the hit damage timer that changes the color of the slime to red for 6 frames after being hit to indicate the slime being hit
void updateSlime(Slime& s) {
    if (s.done) return; //s.done means the slime is either dead or escaped the map
    if (s.hitDamageTimer > 0) s.hitDamageTimer--;   //count down the hit damgage timer
    //this part is for moving the slime torwards the targer point.
    float targetX = pathX[s.target];  //sets the target x and y to the path arrays we made 
    float targetY = pathY[s.target];
    float dx = targetX - s.x; //d stands for delta which is the change in x and y when you subtract the target x with the slimes x and y position
    float dy = targetY - s.y; 
    float dist = sqrt(dx * dx + dy * dy); //The code uses pythagorem theorem to find a straight line distance to the target point
    //if the distance to the target is less than the speed we set the slime to the target changes to the next one, velocity x and y reset to get the new numbers
    if (dist <= s.speed) { 
        s.x = targetX;
        s.y = targetY;
        s.vx = 0;
        s.vy = 0;
        s.target++;
        if (s.target >= pointCount) { s.done = true; s.escaped = true; } //if the slime finishes all the point it is marked as done and escaped
    }
    else { //changes vx and vy to new values by using the distance and speed so the slime moves at the proper speed in both x and y direction
        //https://www.studyplan.dev/sdl2/vectors-and-movement
        //https://gamedev.net/articles/programming/math-and-physics/practical-use-of-vector-math-in-games-r2968/
        s.vx =  s.speed * (dx / dist);
        s.vy = s.speed * (dy / dist);
        s.x = s.x + s.vx; //updayes the slimes position 
        s.y = s.y + s.vy;
    }
}

// void function to draw the slime 
void drawSlime(const Slime& s) {
    if (s.done) {
        return; //if the slime escaped or got killed we dont draw it
    }
    int w = al_get_bitmap_width(s.bitmap); //gets the width and the height of the slime so we can draw it
    int h = al_get_bitmap_height(s.bitmap);
    float scale;
    if (s.isKing) { //these are scales for the slime and the king slime. king slime is using a different scale cause he is 512x512 while all the other slimes are 64x64
        scale = 0.25f;
    } 
    else {
        scale = 1.2f;
    }
    float drawX = s.x - ((w * scale) / 2); //the reason we simply dont use s.x and s.y is because for some reason the game would draw the slime at the top left corner not center
    float drawY = s.y - ((h * scale) / 2); //to fix this we take half of the height and width divide by to and subtract to make the slime centered
    if (s.hitDamageTimer > 0) {
        //this is for the red hit indicatior sets the tint to red https://www.allegro.cc/manual/5/al_draw_tinted_scaled_bitmap (this thing explain how tinting works)
        al_draw_tinted_scaled_bitmap(s.bitmap, al_map_rgb(255, 80, 80), 0, 0, w, h, drawX, drawY, w * scale, h * scale, 0);
    }
    else {
        al_draw_scaled_bitmap(s.bitmap, 0, 0, w, h, drawX, drawY, w * scale, h * scale, 0);
    }
}