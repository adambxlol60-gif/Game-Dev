//Created by: Noah Basaria and Adam Jurewicz
//Program Name: Canadian Tower Defence
//Description: A super interesting tower defence game with Canadian characters and amazing upgrades. One of the best tower defence games you will ever play.
//Date: Jun 7, 2026


#include <allegro5/allegro.h>
#include "function.h"
#include "waveload.h"

//used int main function to run the game
int main(int argc, char *argv[]) {
    if (!initAllegro()) return -1; //check if allegro got setup properly

    //creates a display and loads the bitmaps for the map, tower, and slime
    //if error, error message pops up
    ALLEGRO_DISPLAY *display = createDisplay();
    if (!display) return -1;

    //loads bitmaps for the map, tower, slime, weeknd and microphone projectile
    ALLEGRO_BITMAP *image = nullptr, *drakeBitmap = nullptr, *slimeBitmap = nullptr, *weekndBitmap = nullptr, *microphoneBitmap = nullptr, *heartBitmap = nullptr, *drakeMicBitmap = nullptr, *elonBitmap = nullptr, *rocketBitmap = nullptr, *bankBitmap = nullptr, *icemanBitmap = nullptr, *starboyBitmap = nullptr, *teslaBitmap = nullptr;
    if (!Images(display, image, drakeBitmap, slimeBitmap, weekndBitmap, microphoneBitmap, heartBitmap, drakeMicBitmap, elonBitmap, rocketBitmap, bankBitmap, icemanBitmap, starboyBitmap, teslaBitmap)) {
        al_destroy_display(display);
        return -1;
    }

    //creates a timer, it is one of the most important parts of the game
    //it updates the game 60 times a second
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    eventQueue(event_queue, display, timer);
    //keyboard for the m test hotkey (jump to wave 50)
    al_install_keyboard();
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    loadPathFromMap(image); //loads the slime path from the map bitmap

    //arrays that hold every tower and the towers cooldown
    Tower towers[maxTowers];
    TowerState towerStates[maxTowers];
    ALLEGRO_FONT* font = al_create_builtin_font();

    //game state variables
    int towerCount = 0;
    int gold = 500; // starting amount of gold
    int playerHealth = 20; // player's set health
    bool drakeSelected = false;
    bool weekndSelected = false;
    bool elonSelected = false;
    bool bankSelected = false;
    int selectedTowerIndex = -1; // -1 means no tower is currently selected
    int drakeBitmapW = al_get_bitmap_width(drakeBitmap);
    int drakeBitmapH = al_get_bitmap_height(drakeBitmap);
    int weekndBitmapW = al_get_bitmap_width(weekndBitmap);
    int weekndBitmapH = al_get_bitmap_height(weekndBitmap);
    int elonBitmapW = al_get_bitmap_width(elonBitmap);
    int elonBitmapH = al_get_bitmap_height(elonBitmap);
    int bankBitmapW = al_get_bitmap_width(bankBitmap);
    int bankBitmapH = al_get_bitmap_height(bankBitmap);
    int icemanBitmapW = al_get_bitmap_width(icemanBitmap);
    int icemanBitmapH = al_get_bitmap_height(icemanBitmap);
    int starboyBitmapW = al_get_bitmap_width(starboyBitmap);
    int starboyBitmapH = al_get_bitmap_height(starboyBitmap);
    int teslaBitmapW = al_get_bitmap_width(teslaBitmap);
    int teslaBitmapH = al_get_bitmap_height(teslaBitmap);

    //arrays for slimes and bullets, and variables for wave manaagement
    //the static arrays are too large for Allegro secondary thread stack
    //reference: https://en.cppreference.com/cpp/language/storage_duration
    //https://craftofcoding.wordpress.com/2015/12/07/memory-in-c-the-stack-the-heap-and-static/
    //https://www.geeksforgeeks.org/cpp/difference-between-stack-allocated-and-heap-allocated-arrays/
    static Slime slimes[maxSlimes];
    int slimeCount = 0;
    static Bullet bullets[maxBullets];
    int bulletCount = 0;
    bool running = true;
    int mouseX = 0, mouseY = 0;
    //tracks where the mouse is so we can draw the tower preview at the cursor
    static Wave allWaves[maxWaves];
    int waveCount = loadWaves("waves.txt", allWaves, maxWaves);
    //self explanatory variables, current wave tells your wave,  spawn index tracks which enemies have spawned next, nextSpawnIn is for tracking and betweenwaves is check if the player is between waves
    int currentWave = 0;
    int spawnIndex = 0;
    int nextSpawnIn = 0;
    bool betweenWaves = true;

    int gameMenu = 1;   //the gameMenu integer is resposnible for what screen you see 1 = menu, 0 = game, 2 = win screen, 3 = game over screen
    //play button image is drawn scaled and centered
    const float playScale = 0.6f;
    const int playDrawW = (int)(screenW * playScale);
    const int playDrawH = (int)(screenH * playScale);
    const int playDrawX = (screenW - playDrawW) / 2;
    const int playDrawY = (screenH - playDrawH) / 2;
    const int playLeft   = (int)(640 + (345 - 640) * playScale);
    const int playRight  = (int)(640 + (935 - 640) * playScale);
    const int playTop    = (int)(480 + (384 - 480) * playScale);
    const int playBottom = (int)(480 + (557 - 480) * playScale);

    //retry and menu button with area
    const int retryLeft = 265, retryRight = 1015;
    const int retryDrawTop = 240, retryDrawBottom = 800;
    const int menuLeft  = 265, menuRight = 1015;
    const int menuDrawTop = 440, menuDrawBottom = 1000;
    const int btnClickSplit = 620;
    const int retryClickTop = 240, retryClickBottom = btnClickSplit;
    const int menuClickTop  = btnClickSplit, menuClickBottom = 1000;

    al_start_timer(timer);

    //https://gamefromscratch.com/allegro-tutorial-series-part-2-a-simple-game-loop/
    //https://lazyfoo.net/articles/article06/index.php
    //used while loop to run the game
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        //the window can be scaled down to fit smaller screens, but the mouse still reports the real window position
        //so we divide by displayScale to turn it back into our 1280x960 layout, otherwise every button would be in the wrong spot
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            event.mouse.x = (int)(event.mouse.x / displayScale);
            event.mouse.y = (int)(event.mouse.y / displayScale);
        }

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        //used test hotkey: m jumps straight to wave 50 and starts it
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_M) {
                gameMenu = 0;
                currentWave = 49;
                gold = 50000;
                slimeCount = 0;
                betweenWaves = false;
                spawnIndex = 0;
                nextSpawnIn = 0;
            }
        }

        //this handles all mouse click events our the game
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (gameMenu == 1) {
                if (event.mouse.button == 1 && event.mouse.x >= playLeft && event.mouse.x <= playRight && event.mouse.y >= playTop && event.mouse.y <= playBottom) {
                    gameMenu = 0;
                }
            } //resets the game when you press the retry button
            else if (gameMenu == 2 || gameMenu == 3) {
                if (event.mouse.button == 1 && event.mouse.x >= retryLeft && event.mouse.x <= retryRight && event.mouse.y >= retryClickTop && event.mouse.y <= retryClickBottom) {
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
                    gameMenu = 0;
                }//does the same but for menu button
                else if (event.mouse.button == 1 && event.mouse.x >= menuLeft && event.mouse.x <= menuRight && event.mouse.y >= menuClickTop && event.mouse.y <= menuClickBottom) {
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
                    gameMenu = 1;
                }
            } //self explanatory
            else if (event.mouse.button == 1 && betweenWaves && nextWaveButtonPressed(event.mouse.x, event.mouse.y)) {
                if (currentWave < waveCount) {
                    betweenWaves = false;
                    spawnIndex = 0;
                    nextSpawnIn = 0;
                }
            }
            
            
            //selecting towers with the buttions on the hud
            else if (event.mouse.button == 1 && drakeButtonPressed(event.mouse.x, event.mouse.y)) {
                drakeSelected = !drakeSelected;
                if (drakeSelected) { weekndSelected = false; elonSelected = false; bankSelected = false; }
                selectedTowerIndex = -1;
            } 
            else if (event.mouse.button == 1 && weekndButtonPressed(event.mouse.x, event.mouse.y)) {
                weekndSelected = !weekndSelected;
                if (weekndSelected) { drakeSelected = false; elonSelected = false; bankSelected = false; }
                selectedTowerIndex = -1;
            } 
            else if (event.mouse.button == 1 && elonButtonPressed(event.mouse.x, event.mouse.y)) {
                elonSelected = !elonSelected;
                if (elonSelected) { drakeSelected = false; weekndSelected = false; bankSelected = false; }
                selectedTowerIndex = -1;
            } 
            else if (event.mouse.button == 1 && bankButtonPressed(event.mouse.x, event.mouse.y)) {
                bankSelected = !bankSelected;
                if (bankSelected) { drakeSelected = false; weekndSelected = false; elonSelected = false; }
                selectedTowerIndex = -1;
            } 
            else if (event.mouse.button == 1 && selectedTowerIndex >= 0 && sellButtonPressed(event.mouse.x, event.mouse.y)) {
                gold += refundFor(towers[selectedTowerIndex].type);
                towers[selectedTowerIndex] = towers[--towerCount];
                towerStates[selectedTowerIndex] = towerStates[towerCount];
                selectedTowerIndex = -1;
            } 
            //this code is responsible for the upgrade button on weeknd, drake and elon musk. It subtracts the gold and recenters the tower since the new sprite is a different size
            else if (event.mouse.button == 1 && selectedTowerIndex >= 0 && upgrade1ButtonPressed(event.mouse.x, event.mouse.y)) {
                Tower& tower = towers[selectedTowerIndex];
                if (tower.type == towerDrake && gold >= drakeUpgradeCost) {
                    gold -= drakeUpgradeCost;
                    float cx = tower.x + tower.w / 2;
                    float cy = tower.y + tower.h / 2;
                    tower.type = towerIceman;
                    tower.w = icemanBitmapW * icemanScale;
                    tower.h = icemanBitmapH * icemanScale;
                    tower.x = cx - tower.w / 2;
                    tower.y = cy - tower.h / 2;
                } //same code but for the weeknd
                 if (tower.type == towerWeeknd && gold >= weekndUpgradeCost) {
                    gold -= weekndUpgradeCost;
                    float cx = tower.x + tower.w / 2;
                    float cy = tower.y + tower.h / 2;
                    tower.type = towerStarboy;
                    tower.w = starboyBitmapW * starboyScale;
                    tower.h = starboyBitmapH * starboyScale;
                    tower.x = cx - tower.w / 2;
                    tower.y = cy - tower.h / 2;
                }  //same code but for elon musk
                if (tower.type == towerElon && gold >= elonUpgradeCost) {
                    gold -= elonUpgradeCost;
                    float cx = tower.x + tower.w / 2;
                    float cy = tower.y + tower.h / 2;
                    tower.type = towerTeslaMan;
                    tower.w = teslaBitmapW * teslaScale;
                    tower.h = teslaBitmapH * teslaScale;
                    tower.x = cx - tower.w / 2;
                    tower.y = cy - tower.h / 2;
                }
            } 
            //the second universal upgrade button
            else if (event.mouse.button == 1 && selectedTowerIndex >= 0 && upgrade2ButtonPressed(event.mouse.x, event.mouse.y)) {
                Tower& tower = towers[selectedTowerIndex];
                int cost = 300 + 100 * tower.damageUpgradeLevel;
                if (gold >= cost && tower.damageUpgradeLevel < maxDamageUpgradeLevel) {
                    gold -= cost;
                    tower.damageUpgradeLevel += 1;
                }
            } 
            else if (event.mouse.button == 1 && selectedTowerIndex >= 0 && clickInsidePanel(event.mouse.x, event.mouse.y)) {
                // if click landed on the panel area but not on the sell button it does nothing
            } 
            //if the click didnt land on any button we check if the player clicked a tower
            else if (event.mouse.button == 1) {
                int clicked = selectTower(event.mouse.x, event.mouse.y, towers, towerCount);
                if (clicked >= 0) {
                    //clicking the same tower twice deselects it while clicking a different one selects it
                    if (clicked == selectedTowerIndex) {
                        selectedTowerIndex = -1;
                    }
                    else {
                        selectedTowerIndex = clicked;
                        drakeSelected = false;
                        weekndSelected = false;
                        elonSelected = false;
                        bankSelected = false;
                    }
                }
                else {
                    //if no tower was clicked  we try to place one if a tower button is selected
                    selectedTowerIndex = -1;
                    int cost;
                    if (drakeSelected) { cost = drakeCost; }
                    else if (weekndSelected) { cost = weekndCost; }
                    else if (elonSelected) { cost = elonCost; }
                    else { cost = bankCost; }
                    handleMouseClick(event, towers, towerCount, image, drakeBitmap, drakeBitmapW, drakeBitmapH, weekndBitmapW, weekndBitmapH, elonBitmapW, elonBitmapH, bankBitmapW, bankBitmapH, gold, drakeSelected, weekndSelected, elonSelected, bankSelected, cost);
                }
            }
        }

        //updates the saved mouse position every time the cursor moves
        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouseX = event.mouse.x;
            mouseY = event.mouse.y;
        }

        //this is where the main game logic happens: spawning slimes, updating their movement...
        if (event.type == ALLEGRO_EVENT_TIMER) {
            //we scale everything we draw so our 1280x960 layout fills the (possibly smaller) window
            //this is what actually makes the game shrink to fit short screens
            //reference: https://liballeg.org/a5docs/trunk/transformations.html#al_scale_transform
            ALLEGRO_TRANSFORM scaleTransform;
            al_identity_transform(&scaleTransform);
            al_scale_transform(&scaleTransform, displayScale, displayScale);
            al_use_transform(&scaleTransform);
            //we draw the menu screen here, the title image and the play button
            if (gameMenu == 1) {
                al_draw_scaled_bitmap(bitmaps[25], 0, 0, al_get_bitmap_width(bitmaps[25]), al_get_bitmap_height(bitmaps[25]), 0, 0, screenW, screenH, 0);
                al_draw_scaled_bitmap(bitmaps[26], 0, 0, al_get_bitmap_width(bitmaps[26]), al_get_bitmap_height(bitmaps[26]), playDrawX, playDrawY, playDrawW, playDrawH, 0);
                al_flip_display();
            }
            //we draw the win or gameover screen depending on gameMenu, then the retry and menu buttons on top
            else if (gameMenu == 3 || gameMenu == 2) {
                ALLEGRO_BITMAP* backdrop;
                if (gameMenu == 3) { backdrop = bitmaps[27]; } else { backdrop = bitmaps[28]; }
                al_draw_scaled_bitmap(backdrop, 0, 0, al_get_bitmap_width(backdrop), al_get_bitmap_height(backdrop), 0, 0, screenW, screenH, 0);
                al_draw_scaled_bitmap(bitmaps[29], 0, 0, al_get_bitmap_width(bitmaps[29]), al_get_bitmap_height(bitmaps[29]), retryLeft, retryDrawTop, retryRight - retryLeft, retryDrawBottom - retryDrawTop, 0);
                al_draw_scaled_bitmap(bitmaps[30], 0, 0, al_get_bitmap_width(bitmaps[30]), al_get_bitmap_height(bitmaps[30]), menuLeft, menuDrawTop, menuRight - menuLeft, menuDrawBottom - menuDrawTop, 0);
                al_flip_display();
            }
            else {
            //if statemtn that so that we only spawn slimes if a wave is currently running
            if (!betweenWaves) {
                if (spawnIndex < allWaves[currentWave].spawnCount) {
                    //we count down to the next spawn, when it hits 0 we spawn the next slime
                    if (nextSpawnIn <= 0) {
                        Spawn& s = allWaves[currentWave].spawns[spawnIndex];
                        if (slimeCount < maxSlimes) {
                            //this part is for spawning the slime and giving it the camo or metal variable
                            Slime newSlime = initSlime(s.bitmap, s.hp, s.speed);
                            if (isCamoSpawn(s)) newSlime.camo = true;
                            if (isMetalSpawn(s)) newSlime.metal = true;
                            if (isKingSpawn(s)) newSlime.isKing = true;
                            int splitCount; int splitHp; float splitSpeed; ALLEGRO_BITMAP* splitBitmap;
                            splitDataFor(s, splitCount, splitHp, splitSpeed, splitBitmap);
                            //if a slime splits we save its datat in here
                            if (splitCount > 0) {
                                newSlime.splitCount  = splitCount;
                                newSlime.splitHp     = splitHp;
                                newSlime.splitSpeed  = splitSpeed;
                                newSlime.splitBitmap = splitBitmap;
                            }
                            slimes[slimeCount++] = newSlime;
                        }
                        //the spawning mechanic
                        nextSpawnIn = s.spawnInterval;
                        spawnIndex++;
                    } 
                    else {
                        nextSpawnIn--;
                    }
                } 
                else {
                    //we check if all the slimes are dead if they are we end the wave
                    bool allDone = true;
                    for (int i = 0; i < slimeCount; i++)
                        if (!slimes[i].done) { allDone = false; break; }
                    if (allDone) {
                        //when wave is over the game gives the player their gold  
                        gold += 100 + (currentWave * 5);
                        int bankMoney = countBanks(towers, towerCount);
                        gold += 150 * bankMoney; //every bank pays out 150 gold at end of wave
                        currentWave++;
                        betweenWaves = true;
                        slimeCount = 0;
                        if (currentWave >= waveCount) gameMenu = 2; //all waves done go to win screen
                    }
                }
            }

            //we move every slime forward and check if any escaped, if they did we take a life fromthe player
            for (int i = 0; i < slimeCount; i++) {
                updateSlime(slimes[i]);
                if (slimes[i].escaped) {
                    if (slimes[i].isKing) gameMenu = 3; //king slime escaping is instant game over
                    playerHealth--;
                    slimes[i].escaped = false;
                    if (playerHealth <=0) {
                        gameMenu = 3;
                    }
                }
            }
            //we make the towers shoot at slimes and move the bullets
            updateTowers(towers, towerStates, towerCount, slimes, slimeCount, bullets, &bulletCount, microphoneBitmap, drakeMicBitmap, rocketBitmap);
            updateBullets(bullets, &bulletCount, slimes, slimeCount, &gold);
            //this code spawns slimelets of slimes that can split 
            for (int i = 0; i < slimeCount; i++) {
                if (!slimes[i].pendingSplit) continue;
                slimes[i].pendingSplit = false;
                for (int c = 0; c < slimes[i].splitCount && slimeCount < maxSlimes; c++) {
                    Slime slimelet = initSlime(slimes[i].splitBitmap, slimes[i].splitHp, slimes[i].splitSpeed);
                    slimelet.x = slimes[i].x;
                    slimelet.y = slimes[i].y;
                    slimelet.target = slimes[i].target;
                    //purple slime's slimelets also split into basic slimes
                    if (slimelet.bitmap == bitmaps[9]) {
                        slimelet.splitCount = 2;
                        slimelet.splitHp = slimelet.hp/2;
                        slimelet.splitSpeed = slimelet.speed*1.3f;
                        slimelet.splitBitmap = bitmaps[2];
                    }
                    slimes[slimeCount++] = slimelet;
                }
            }
            //we draw everything here, map first then towers then slimes then bullets then hud on top
            //without it nothing would appear
            al_draw_bitmap(image, 0, 0, 0);
            for (int i = 0; i < towerCount; i++) {
                ALLEGRO_BITMAP* sprite;
                int sw, sh;
                if (towers[i].type == towerWeeknd) {
                    sprite = weekndBitmap; 
                    sw = weekndBitmapW; 
                    sh = weekndBitmapH;
                }
                else if (towers[i].type == towerElon) {
                    sprite = elonBitmap; 
                    sw = elonBitmapW;   
                    sh = elonBitmapH;
                } 
                else if (towers[i].type == towerBank) {
                    sprite = bankBitmap;   
                    sw = bankBitmapW;   sh = bankBitmapH;
                } 
                else if (towers[i].type == towerIceman) {
                    sprite = icemanBitmap; 
                    sw = icemanBitmapW; 
                    sh = icemanBitmapH;
                } 
                else if (towers[i].type == towerStarboy) {
                    sprite = starboyBitmap; 
                    sw = starboyBitmapW; 
                    sh = starboyBitmapH;
                } 
                else if (towers[i].type == towerTeslaMan) {
                    sprite = teslaBitmap;
                     sw = teslaBitmapW; 
                     sh = teslaBitmapH;
                } 
                else {
                    sprite = drakeBitmap;  
                    sw = drakeBitmapW;  
                    sh = drakeBitmapH;
                }
                al_draw_scaled_bitmap(sprite, 0, 0, sw, sh, towers[i].x, towers[i].y, towers[i].w, towers[i].h, 0);
            }
            //highlighttower like the name suggest highlights the tower to indicate 
            highlightTower(towers, selectedTowerIndex, drakeBitmap, drakeBitmapW, drakeBitmapH, weekndBitmap, weekndBitmapW, weekndBitmapH, elonBitmap, elonBitmapW, elonBitmapH, bankBitmap, bankBitmapW, bankBitmapH, icemanBitmap, icemanBitmapW, icemanBitmapH, starboyBitmap, starboyBitmapW, starboyBitmapH, teslaBitmap, teslaBitmapW, teslaBitmapH);
            for (int i = 0; i < slimeCount; i++) drawSlime(slimes[i]);
            drawBullets(bullets, bulletCount);
            //draws the ghost tower and range circle under the hud so the hud always stays on top
            towerPlacement(drakeBitmap, drakeBitmapW, drakeBitmapH, weekndBitmap, weekndBitmapW, weekndBitmapH, elonBitmap, elonBitmapW, elonBitmapH, bankBitmap, bankBitmapW, bankBitmapH, mouseX, mouseY, image, towers, towerCount, gold, drakeSelected, weekndSelected, elonSelected, bankSelected);
            //we draw all the hud elements on top so they are always visible
            drawHud(font, gold, towerCount, drakeSelected, weekndSelected, elonSelected, bankSelected, playerHealth, heartBitmap, currentWave, waveCount);
            placeDrakeButton(font, drakeSelected);
            placeWeekndButton(font, weekndSelected);
            placeElonButton(font, elonSelected);
            placeBankButton(font, bankSelected);
            drawTowerPanel(font, drakeBitmap, drakeBitmapW, drakeBitmapH, weekndBitmap, weekndBitmapW, weekndBitmapH, elonBitmap, elonBitmapW, elonBitmapH, bankBitmap, bankBitmapW, bankBitmapH, icemanBitmap, icemanBitmapW, icemanBitmapH, starboyBitmap, starboyBitmapW, starboyBitmapH, teslaBitmap, teslaBitmapW, teslaBitmapH, towers, selectedTowerIndex);
            placeNextWaveButton(font, betweenWaves, currentWave, waveCount);
            drawTipBar(font, currentWave);
            //we push everything we drew to the actual screen, without this nothing would show up
            al_flip_display();
            }
        }
    }

    // cleans up the program resources at the end of the program
    al_destroy_font(font);
    deleteAllegro(timer, event_queue, slimeBitmap, drakeBitmap, weekndBitmap, microphoneBitmap, image, display);
    return 0;
}
