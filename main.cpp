#include <allegro5/allegro.h>
#include "function.h"
#include "waveload.h"

//int main function to run the game
int main(int argc, char *argv[]) {
    if (!initAllegro()) return -1;

    // creates a display and loads the bitmaps for the map, tower, and slime
    // if error, error message pops up
    ALLEGRO_DISPLAY *display = createDisplay();
    if (!display) return -1;

    //bitmaps for the map, tower, slime, weeknd and microphone projectile
    ALLEGRO_BITMAP *image = nullptr, *drakeBmp = nullptr, *slimeBmp = nullptr, *weekndBmp = nullptr, *microphoneBmp = nullptr, *heartBmp = nullptr, *drakeMicBmp = nullptr, *elonBmp = nullptr, *rocketBmp = nullptr, *bankBmp = nullptr, *icemanBmp = nullptr;
    if (!Images(display, image, drakeBmp, slimeBmp, weekndBmp, microphoneBmp, heartBmp, drakeMicBmp, elonBmp, rocketBmp, bankBmp, icemanBmp)) {
        al_destroy_display(display);
        return -1;
    }

    // creates timer
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    eventQueue(event_queue, display, timer);

    loadPathFromMap(image); // loads the slime path from the map bitmap

    Tower towers[maxTowers];
    TowerState towerStates[maxTowers];
    ALLEGRO_FONT* font = al_create_builtin_font();

    // game state variables
    int towerCount = 0;
    int gold = 1000; // starting amount of gold
    int playerHealth = 20; // player's health
    bool drakeSelected = false;
    bool weekndSelected = false;
    bool elonSelected = false;
    bool bankSelected = false;
    int selectedTowerIndex = -1; // -1 means no tower is currently selected
    int drakeBmpW = al_get_bitmap_width(drakeBmp);
    int drakeBmpH = al_get_bitmap_height(drakeBmp);
    int weekndBmpW = al_get_bitmap_width(weekndBmp);
    int weekndBmpH = al_get_bitmap_height(weekndBmp);
    int elonBmpW = al_get_bitmap_width(elonBmp);
    int elonBmpH = al_get_bitmap_height(elonBmp);
    int bankBmpW = al_get_bitmap_width(bankBmp);
    int bankBmpH = al_get_bitmap_height(bankBmp);
    int icemanBmpW = al_get_bitmap_width(icemanBmp);
    int icemanBmpH = al_get_bitmap_height(icemanBmp);

    // arrays for slimes and bullets, and variables for wave manaagement
    // static: these are too large for Allegro's secondary thread stack on macOS
    static Slime slimes[maxSlimes];
    int slimeCount = 0;
    static Bullet bullets[maxBullets];
    int bulletCount = 0;
    bool running = true;
    int mouseX = 0, mouseY = 0;
    //tracks where the mouse is so we can draw the tower preview at the cursor
    static Wave allWaves[maxWaves];
    int waveCount = loadWaves("waves.txt", allWaves, maxWaves);

    int currentWave = 0;
    int spawnIndex = 0;
    int nextSpawnIn = 0;
    bool betweenWaves = true;
    

    al_start_timer(timer);

    // while loop to run the game
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (event.mouse.button == 1 && betweenWaves && nextWaveButtonPressed(event.mouse.x, event.mouse.y)) {
                if (currentWave < waveCount) {
                    betweenWaves = false;
                    spawnIndex = 0;
                    nextSpawnIn = 0;
                }
            }
            
            
            
            else if (event.mouse.button == 1 && drakeButtonPressed(event.mouse.x, event.mouse.y)) {
                drakeSelected = !drakeSelected;
                if (drakeSelected) { weekndSelected = false; elonSelected = false; bankSelected = false; }
                selectedTowerIndex = -1;
            } else if (event.mouse.button == 1 && weekndButtonPressed(event.mouse.x, event.mouse.y)) {
                weekndSelected = !weekndSelected;
                if (weekndSelected) { drakeSelected = false; elonSelected = false; bankSelected = false; }
                selectedTowerIndex = -1;
            } else if (event.mouse.button == 1 && elonButtonPressed(event.mouse.x, event.mouse.y)) {
                elonSelected = !elonSelected;
                if (elonSelected) { drakeSelected = false; weekndSelected = false; bankSelected = false; }
                selectedTowerIndex = -1;
            } else if (event.mouse.button == 1 && bankButtonPressed(event.mouse.x, event.mouse.y)) {
                bankSelected = !bankSelected;
                if (bankSelected) { drakeSelected = false; weekndSelected = false; elonSelected = false; }
                selectedTowerIndex = -1;
            } else if (event.mouse.button == 1 && selectedTowerIndex >= 0 && sellButtonPressed(event.mouse.x, event.mouse.y)) {
                gold += refundFor(towers[selectedTowerIndex].type);
                towers[selectedTowerIndex] = towers[--towerCount];
                towerStates[selectedTowerIndex] = towerStates[towerCount];
                selectedTowerIndex = -1;
            } else if (event.mouse.button == 1 && selectedTowerIndex >= 0 && upgrade1ButtonPressed(event.mouse.x, event.mouse.y)) {
                //upgrade 1 - drake into iceman
                Tower& tower = towers[selectedTowerIndex];
                if (tower.type == towerDrake && gold >= drakeUpgradeCost) {
                    gold -= drakeUpgradeCost;
                    float cx = tower.x + tower.w / 2;
                    float cy = tower.y + tower.h / 2;
                    tower.type = towerIceman;
                    tower.w = icemanBmpW * icemanScale;
                    tower.h = icemanBmpH * icemanScale;
                    tower.x = cx - tower.w / 2;
                    tower.y = cy - tower.h / 2;
                }
            } else if (event.mouse.button == 1 && selectedTowerIndex >= 0 && upgrade2ButtonPressed(event.mouse.x, event.mouse.y)) {
                //upgrade 2 - +1 damage per click. Cost ramps: 400, 500, 600...
                Tower& tower = towers[selectedTowerIndex];
                int cost = 400 + 100 * tower.damageUpgradeLevel;
                if (gold >= cost) {
                    gold -= cost;
                    tower.damageUpgradeLevel += 1;
                }
            } else if (event.mouse.button == 1 && selectedTowerIndex >= 0 && clickInsidePanel(event.mouse.x, event.mouse.y)) {
                // click landed on the panel area but not on the sell button - do nothing
            } else if (event.mouse.button == 1) {
                int clicked = selectTower(event.mouse.x, event.mouse.y, towers, towerCount);
                if (clicked >= 0) {
                    if (clicked == selectedTowerIndex) {
                        selectedTowerIndex = -1;
                    } else {
                        selectedTowerIndex = clicked;
                        drakeSelected = false;
                        weekndSelected = false;
                        elonSelected = false;
                        bankSelected = false;
                    }
                } else {
                    selectedTowerIndex = -1;
                    int cost = drakeSelected ? drakeCost : (weekndSelected ? weekndCost : (elonSelected ? elonCost : bankCost));
                    handleMouseClick(event, towers, towerCount, image, drakeBmp, drakeBmpW, drakeBmpH, weekndBmpW, weekndBmpH, elonBmpW, elonBmpH, bankBmpW, bankBmpH, gold, drakeSelected, weekndSelected, elonSelected, bankSelected, cost);
                }
            }
        }

        //updates the saved mouse position every time the cursor moves
        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouseX = event.mouse.x;
            mouseY = event.mouse.y;
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            if (!betweenWaves) {
                if (spawnIndex < allWaves[currentWave].spawnCount) {
                    if (nextSpawnIn <= 0) {
                        Spawn& s = allWaves[currentWave].spawns[spawnIndex];
                        if (slimeCount < maxSlimes)
                            slimes[slimeCount++] = initSlime(s.bitmap, s.hp, s.speed);
                        nextSpawnIn = s.spawnInterval;
                        spawnIndex++;
                    } else {
                        nextSpawnIn--;
                    }
                } else {
                    bool allDone = true;
                    for (int i = 0; i < slimeCount; i++)
                        if (!slimes[i].done) { allDone = false; break; }
                    if (allDone) {
                        gold += 100 + (currentWave * 25);
                        //every bank pays out 250 gold at the end of the wave
                        int bankMoney = countBanks(towers, towerCount);
                        gold += 250 * bankMoney;
                        currentWave++;
                        betweenWaves = true;
                        slimeCount = 0;
                    }
                }
            }

            //for loop to update the enemies, towers, and bullets, then draw them on the screen
            for (int i = 0; i < slimeCount; i++) {
                updateSlime(slimes[i]);
                if (slimes[i].escaped) {
                    playerHealth--;
                    slimes[i].escaped = false;
                    if (playerHealth <=0) {
                        // game over
                        running = false;
                    }
                }
            }
            updateTowers(towers, towerStates, towerCount, slimes, slimeCount, bullets, &bulletCount, microphoneBmp, drakeMicBmp, rocketBmp);
            updateBullets(bullets, &bulletCount, slimes, slimeCount, &gold);

            // draws everthing
            al_draw_bitmap(image, 0, 0, 0);
            for (int i = 0; i < towerCount; i++) {
                ALLEGRO_BITMAP* sprite;
                int sw, sh;
                if (towers[i].type == towerWeeknd) {
                    sprite = weekndBmp; sw = weekndBmpW; sh = weekndBmpH;
                } else if (towers[i].type == towerElon) {
                    sprite = elonBmp;   sw = elonBmpW;   sh = elonBmpH;
                } else if (towers[i].type == towerBank) {
                    sprite = bankBmp;   sw = bankBmpW;   sh = bankBmpH;
                } else if (towers[i].type == towerIceman) {
                    sprite = icemanBmp; sw = icemanBmpW; sh = icemanBmpH;
                } else {
                    sprite = drakeBmp;  sw = drakeBmpW;  sh = drakeBmpH;
                }
                al_draw_scaled_bitmap(sprite, 0, 0, sw, sh, towers[i].x, towers[i].y, towers[i].w, towers[i].h, 0);
            }
            highlightTower(towers, selectedTowerIndex, drakeBmp, drakeBmpW, drakeBmpH, weekndBmp, weekndBmpW, weekndBmpH, elonBmp, elonBmpW, elonBmpH, bankBmp, bankBmpW, bankBmpH, icemanBmp, icemanBmpW, icemanBmpH);
            for (int i = 0; i < slimeCount; i++) drawSlime(slimes[i]);
            drawBullets(bullets, bulletCount);
            //draws the ghost tower and range circle under the hud so the hud always stays on top
            towerPlacement(drakeBmp, drakeBmpW, drakeBmpH, weekndBmp, weekndBmpW, weekndBmpH, elonBmp, elonBmpW, elonBmpH, bankBmp, bankBmpW, bankBmpH, mouseX, mouseY, image, towers, towerCount, gold, drakeSelected, weekndSelected, elonSelected, bankSelected);
            drawHud(font, gold, towerCount, drakeSelected, weekndSelected, elonSelected, bankSelected, playerHealth, heartBmp, currentWave, waveCount);
            placeDrakeButton(font, drakeSelected);
            placeWeekndButton(font, weekndSelected);
            placeElonButton(font, elonSelected);
            placeBankButton(font, bankSelected);
            drawTowerPanel(font, drakeBmp, drakeBmpW, drakeBmpH, weekndBmp, weekndBmpW, weekndBmpH, elonBmp, elonBmpW, elonBmpH, bankBmp, bankBmpW, bankBmpH, icemanBmp, icemanBmpW, icemanBmpH, towers, selectedTowerIndex);
            placeNextWaveButton(font, betweenWaves, currentWave, waveCount);
            al_flip_display();
        }
    }

    // cleans up the program resources at the end of the program
    al_destroy_font(font);
    deleteAllegro(timer, event_queue, slimeBmp, drakeBmp, weekndBmp, microphoneBmp, image, display);
    return 0;
}
