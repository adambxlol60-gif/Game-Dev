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
    ALLEGRO_BITMAP *image = nullptr, *drakeBmp = nullptr, *slimeBmp = nullptr, *weekndBmp = nullptr, *microphoneBmp = nullptr, *heartBmp = nullptr, *drakeMicBmp = nullptr, *elonBmp = nullptr, *rocketBmp = nullptr, *bankBmp = nullptr, *icemanBmp = nullptr, *starboyBmp = nullptr, *teslaBmp = nullptr;
    if (!Images(display, image, drakeBmp, slimeBmp, weekndBmp, microphoneBmp, heartBmp, drakeMicBmp, elonBmp, rocketBmp, bankBmp, icemanBmp, starboyBmp, teslaBmp)) {
        al_destroy_display(display);
        return -1;
    }

    // creates timer
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    eventQueue(event_queue, display, timer);
    //keyboard for the m test hotkey (jump to wave 50)
    al_install_keyboard();
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    loadPathFromMap(image); // loads the slime path from the map bitmap

    Tower towers[maxTowers];
    TowerState towerStates[maxTowers];
    ALLEGRO_FONT* font = al_create_builtin_font();

    // game state variables
    int towerCount = 0;
    int gold = 500; // starting amount of gold
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
    int starboyBmpW = al_get_bitmap_width(starboyBmp);
    int starboyBmpH = al_get_bitmap_height(starboyBmp);
    int teslaBmpW = al_get_bitmap_width(teslaBmp);
    int teslaBmpH = al_get_bitmap_height(teslaBmp);

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

    int gameMenu = 1;   // 1 = menu, 0 = game, 2 = win, 3 = game over
    //play button: image drawn scaled + centered; click area scaled to match
    const float playScale = 0.6f;
    const int playDrawW = (int)(screenW * playScale);
    const int playDrawH = (int)(screenH * playScale);
    const int playDrawX = (screenW - playDrawW) / 2;
    const int playDrawY = (screenH - playDrawH) / 2;
    const int playLeft   = (int)(640 + (345 - 640) * playScale);
    const int playRight  = (int)(640 + (935 - 640) * playScale);
    const int playTop    = (int)(480 + (384 - 480) * playScale);
    const int playBottom = (int)(480 + (557 - 480) * playScale);

    //retry (top) + menu (bottom) buttons on the win/gameover screens. draw regions big; click areas split so they dont overlap
    const int retryLeft = 265, retryRight = 1015;
    const int retryDrawTop = 240, retryDrawBottom = 800;
    const int menuLeft  = 265, menuRight = 1015;
    const int menuDrawTop = 440, menuDrawBottom = 1000;
    const int btnClickSplit = 620;
    const int retryClickTop = 240, retryClickBottom = btnClickSplit;
    const int menuClickTop  = btnClickSplit, menuClickBottom = 1000;

    //resetGame puts every gameplay variable back to its starting value so Retry/Menu start fresh
    auto resetGame = [&]() {
        towerCount = 0;
        slimeCount = 0;
        bulletCount = 0;
        gold = 500;
        playerHealth = 20;
        currentWave = 0;
        spawnIndex = 0;
        nextSpawnIn = 0;
        betweenWaves = true;
        selectedTowerIndex = -1;
        drakeSelected = false;
        weekndSelected = false;
        elonSelected = false;
        bankSelected = false;
    };

    al_start_timer(timer);

    // while loop to run the game
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        //test hotkey: m jumps straight to wave 50 and starts it
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_M) {
                gameMenu = 0;
                currentWave = 49;
                slimeCount = 0;
                betweenWaves = false;
                spawnIndex = 0;
                nextSpawnIn = 0;
            }
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (gameMenu == 1) {
                if (event.mouse.button == 1 && event.mouse.x >= playLeft && event.mouse.x <= playRight && event.mouse.y >= playTop && event.mouse.y <= playBottom) {
                    gameMenu = 0;
                }
            }
            else if (gameMenu == 2 || gameMenu == 3) {
                if (event.mouse.button == 1 && event.mouse.x >= retryLeft && event.mouse.x <= retryRight && event.mouse.y >= retryClickTop && event.mouse.y <= retryClickBottom) {
                    resetGame();
                    gameMenu = 0;
                }
                else if (event.mouse.button == 1 && event.mouse.x >= menuLeft && event.mouse.x <= menuRight && event.mouse.y >= menuClickTop && event.mouse.y <= menuClickBottom) {
                    resetGame();
                    gameMenu = 1;
                }
            }
            else if (event.mouse.button == 1 && betweenWaves && nextWaveButtonPressed(event.mouse.x, event.mouse.y)) {
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
                //upgrade 1 - drake into iceman, or weeknd into starboy. Recenter with the new footprint so it isnt squished
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
                } else if (tower.type == towerWeeknd && gold >= weekndUpgradeCost) {
                    gold -= weekndUpgradeCost;
                    float cx = tower.x + tower.w / 2;
                    float cy = tower.y + tower.h / 2;
                    tower.type = towerStarboy;
                    tower.w = starboyBmpW * starboyScale;
                    tower.h = starboyBmpH * starboyScale;
                    tower.x = cx - tower.w / 2;
                    tower.y = cy - tower.h / 2;
                } else if (tower.type == towerElon && gold >= elonUpgradeCost) {
                    gold -= elonUpgradeCost;
                    float cx = tower.x + tower.w / 2;
                    float cy = tower.y + tower.h / 2;
                    tower.type = towerTeslaMan;
                    tower.w = teslaBmpW * teslaScale;
                    tower.h = teslaBmpH * teslaScale;
                    tower.x = cx - tower.w / 2;
                    tower.y = cy - tower.h / 2;
                }
            } else if (event.mouse.button == 1 && selectedTowerIndex >= 0 && upgrade2ButtonPressed(event.mouse.x, event.mouse.y)) {
                //upgrade 2 - +1 damage AND slightly faster firing per click. Cost ramps: 300, 400, 500...
                Tower& tower = towers[selectedTowerIndex];
                int cost = 300 + 100 * tower.damageUpgradeLevel;
                if (gold >= cost && tower.damageUpgradeLevel < maxDamageUpgradeLevel) {
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
            if (gameMenu == 1) {
                al_draw_scaled_bitmap(bitmaps[25], 0, 0, al_get_bitmap_width(bitmaps[25]), al_get_bitmap_height(bitmaps[25]), 0, 0, screenW, screenH, 0);
                al_draw_scaled_bitmap(bitmaps[26], 0, 0, al_get_bitmap_width(bitmaps[26]), al_get_bitmap_height(bitmaps[26]), playDrawX, playDrawY, playDrawW, playDrawH, 0);
                al_flip_display();
            }
            else if (gameMenu == 3 || gameMenu == 2) {
                ALLEGRO_BITMAP* backdrop = (gameMenu == 3) ? bitmaps[27] : bitmaps[28];
                al_draw_scaled_bitmap(backdrop, 0, 0, al_get_bitmap_width(backdrop), al_get_bitmap_height(backdrop), 0, 0, screenW, screenH, 0);
                al_draw_scaled_bitmap(bitmaps[29], 0, 0, al_get_bitmap_width(bitmaps[29]), al_get_bitmap_height(bitmaps[29]), retryLeft, retryDrawTop, retryRight - retryLeft, retryDrawBottom - retryDrawTop, 0);
                al_draw_scaled_bitmap(bitmaps[30], 0, 0, al_get_bitmap_width(bitmaps[30]), al_get_bitmap_height(bitmaps[30]), menuLeft, menuDrawTop, menuRight - menuLeft, menuDrawBottom - menuDrawTop, 0);
                al_flip_display();
            }
            else {
            if (!betweenWaves) {
                if (spawnIndex < allWaves[currentWave].spawnCount) {
                    if (nextSpawnIn <= 0) {
                        Spawn& s = allWaves[currentWave].spawns[spawnIndex];
                        if (slimeCount < maxSlimes) {
                            Slime newSlime = initSlime(s.bitmap, s.hp, s.speed);
                            if (isCamoSpawn(s)) newSlime.camo = true;
                            if (isMetalSpawn(s)) newSlime.metal = true;
                            int sc; int shp; float sspd; ALLEGRO_BITMAP* sbmp;
                            splitDataFor(s, sc, shp, sspd, sbmp);
                            if (sc > 0) {
                                newSlime.splitCount  = sc;
                                newSlime.splitHp     = shp;
                                newSlime.splitSpeed  = sspd;
                                newSlime.splitBitmap = sbmp;
                            }
                            slimes[slimeCount++] = newSlime;
                        }
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
                        gold += 100 + (currentWave * 5);
                        //every bank pays out 250 gold at the end of the wave
                        int bankMoney = countBanks(towers, towerCount);
                        gold += 100 * bankMoney;
                        currentWave++;
                        betweenWaves = true;
                        slimeCount = 0;
                        if (currentWave >= waveCount) gameMenu = 2;   //all waves cleared - win screen
                    }
                }
            }

            //for loop to update the enemies, towers, and bullets, then draw them on the screen
            for (int i = 0; i < slimeCount; i++) {
                updateSlime(slimes[i]);
                if (slimes[i].escaped) {
                    if (slimes[i].isKing) gameMenu = 3;   //king reaching the end is an instant loss
                    playerHealth--;
                    slimes[i].escaped = false;
                    if (playerHealth <=0) {
                        gameMenu = 3;   // game over screen
                    }
                }
            }
            updateTowers(towers, towerStates, towerCount, slimes, slimeCount, bullets, &bulletCount, microphoneBmp, drakeMicBmp, rocketBmp);
            updateBullets(bullets, &bulletCount, slimes, slimeCount, &gold);
            for (int i = 0; i < slimeCount; i++) {
                if (!slimes[i].pendingSplit) continue;
                slimes[i].pendingSplit = false;
                for (int c = 0; c < slimes[i].splitCount && slimeCount < maxSlimes; c++) {
                    Slime child = initSlime(slimes[i].splitBitmap, slimes[i].splitHp, slimes[i].splitSpeed);
                    child.x = slimes[i].x;
                    child.y = slimes[i].y;
                    child.target = slimes[i].target;
                    if (child.bitmap == bitmaps[9]) {
                        child.splitCount = 2;
                        child.splitHp = child.hp/2;
                        child.splitSpeed = child.speed*1.3f;
                        child.splitBitmap = bitmaps[2];
                    }
                    slimes[slimeCount++] = child;
                }
            }
            // draws everthing
            al_draw_bitmap(image, 0, 0, 0);
            for (int i = 0; i < towerCount; i++) {
                ALLEGRO_BITMAP* sprite;
                int sw, sh;
                if (towers[i].type == towerWeeknd) {
                    sprite = weekndBmp; 
                    sw = weekndBmpW; 
                    sh = weekndBmpH;
                } else if (towers[i].type == towerElon) {
                    sprite = elonBmp; 
                    sw = elonBmpW;   
                    sh = elonBmpH;
                } else if (towers[i].type == towerBank) {
                    sprite = bankBmp;   
                    sw = bankBmpW;   sh = bankBmpH;
                } else if (towers[i].type == towerIceman) {
                    sprite = icemanBmp; 
                    sw = icemanBmpW; 
                    sh = icemanBmpH;
                } else if (towers[i].type == towerStarboy) {
                    sprite = starboyBmp; 
                    sw = starboyBmpW; 
                    sh = starboyBmpH;
                } else if (towers[i].type == towerTeslaMan) {
                    sprite = teslaBmp;
                     sw = teslaBmpW; 
                     sh = teslaBmpH;
                } else {
                    sprite = drakeBmp;  
                    sw = drakeBmpW;  
                    sh = drakeBmpH;
                }
                al_draw_scaled_bitmap(sprite, 0, 0, sw, sh, towers[i].x, towers[i].y, towers[i].w, towers[i].h, 0);
            }
            highlightTower(towers, selectedTowerIndex, drakeBmp, drakeBmpW, drakeBmpH, weekndBmp, weekndBmpW, weekndBmpH, elonBmp, elonBmpW, elonBmpH, bankBmp, bankBmpW, bankBmpH, icemanBmp, icemanBmpW, icemanBmpH, starboyBmp, starboyBmpW, starboyBmpH, teslaBmp, teslaBmpW, teslaBmpH);
            for (int i = 0; i < slimeCount; i++) drawSlime(slimes[i]);
            drawBullets(bullets, bulletCount);
            //draws the ghost tower and range circle under the hud so the hud always stays on top
            towerPlacement(drakeBmp, drakeBmpW, drakeBmpH, weekndBmp, weekndBmpW, weekndBmpH, elonBmp, elonBmpW, elonBmpH, bankBmp, bankBmpW, bankBmpH, mouseX, mouseY, image, towers, towerCount, gold, drakeSelected, weekndSelected, elonSelected, bankSelected);
            drawHud(font, gold, towerCount, drakeSelected, weekndSelected, elonSelected, bankSelected, playerHealth, heartBmp, currentWave, waveCount);
            placeDrakeButton(font, drakeSelected);
            placeWeekndButton(font, weekndSelected);
            placeElonButton(font, elonSelected);
            placeBankButton(font, bankSelected);
            drawTowerPanel(font, drakeBmp, drakeBmpW, drakeBmpH, weekndBmp, weekndBmpW, weekndBmpH, elonBmp, elonBmpW, elonBmpH, bankBmp, bankBmpW, bankBmpH, icemanBmp, icemanBmpW, icemanBmpH, starboyBmp, starboyBmpW, starboyBmpH, teslaBmp, teslaBmpW, teslaBmpH, towers, selectedTowerIndex);
            placeNextWaveButton(font, betweenWaves, currentWave, waveCount);
            al_flip_display();
            }
        }
    }

    // cleans up the program resources at the end of the program
    al_destroy_font(font);
    deleteAllegro(timer, event_queue, slimeBmp, drakeBmp, weekndBmp, microphoneBmp, image, display);
    return 0;
}
