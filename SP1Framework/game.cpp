// This is the main file for the game logic and function
//
//
#include "game.h"
#include "Framework\console.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

std::string getFileContents(std::ifstream&);
double  g_dElapsedTime;
double  g_dDeltaTime;
SKeyEvent g_skKeyEvent[K_COUNT];
SMouseEvent g_mouseEvent;


// Game specific variables here
SGameChar   g_sChar;
SBulletChar* g_bullet[5] = {};
SBulletChar* g_enemyBullet[25] = {};
SFireChar* g_sFire[100] = {};
SBossFireChar* g_sBoss[1] = {};
int TutorialEnemies = 36;
int Level1Enemies = 90;
int Level2Enemies = 16;
int Level3Enemies = 46;
int bossHealthCounter = 50;
int playerMove = 0;
bool AllEnemiesCleared = false;
bool fireMove = false;
bool enemyBulletMove = false;
bool gameOver = false;
int LivesLeft = 5;
int stagecounter = 0;
EGAMESTATES g_eGameState = S_SPLASHSCREEN; // initial state

// Console object
Console g_Console(80, 25, "Black Summer");//setconsolesize

//--------------------------------------------------------------
// Purpose  : Initialisation function
//            Initialize variables, allocate memory, load data from file, etc. 
//            This is called once before entering into your main loop
// Input    : void
// Output   : void
//--------------------------------------------------------------
void init(void)
{
    // Set precision for floating point output
    g_dElapsedTime = 0.0;

    // sets the initial state for the game
    g_eGameState = S_SPLASHSCREEN;
    g_sChar.m_cLocation.X = 39;
    g_sChar.m_cLocation.Y = 23;
    g_sChar.m_bActive = true;
    // sets the width, height and the font name to use in the console
    g_Console.setConsoleFont(0, 16, L"Consolas");

    // remember to set your keyboard handler, so that your functions can be notified of input events
    g_Console.setKeyboardHandler(keyboardHandler);
    g_Console.setMouseHandler(mouseHandler);
    spawnFire(0);
}

//--------------------------------------------------------------
// Purpose  : Reset before exiting the program
//            Do your clean up of memory here
//            This is called once just before the game exits
// Input    : Void
// Output   : void
//--------------------------------------------------------------
void shutdown(void)
{
    // Reset to white text on black background
    colour(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

    g_Console.clearBuffer();
}

//--------------------------------------------------------------
// Purpose  : Get all the console input events
//            This function sets up the keyboard and mouse input from the console.
//            We will need to reset all the keyboard status, because no events will be sent if no keys are pressed.
//
//            Remember to set the handlers for keyboard and mouse events.
//            The function prototype of the handler is a function that takes in a const reference to the event struct
//            and returns nothing. 
//            void pfKeyboardHandler(const KEY_EVENT_RECORD&);
//            void pfMouseHandlerconst MOUSE_EVENT_RECORD&);
// Input    : Void
// Output   : void
//--------------------------------------------------------------
void getInput(void)
{
    // resets all the keyboard events
    memset(g_skKeyEvent, 0, K_COUNT * sizeof(*g_skKeyEvent));
    // then call the console to detect input from user
    g_Console.readConsoleInput();
}

//--------------------------------------------------------------
// Purpose  : This is the handler for the keyboard input. Whenever there is a keyboard event, this function will be called.
//            Ideally, you should pass the key event to a specific function to handle the event.
//            This is because in some states, some keys would be disabled. Hence, to reduce your code complexity, 
//            it would be wise to split your keyboard input handlers separately.
//            
//            The KEY_EVENT_RECORD struct has more attributes that you can use, if you are adventurous enough.
//
//            In this case, we are not handling any keyboard event in the Splashscreen state
//            
// Input    : const KEY_EVENT_RECORD& keyboardEvent - reference to a key event struct
// Output   : void
//--------------------------------------------------------------
void keyboardHandler(const KEY_EVENT_RECORD& keyboardEvent)
{
    switch (g_eGameState)
    {
    case S_SPLASHSCREEN: // don't handle anything for the splash screen
        break;
    case S_GAME: gameplayKBHandler(keyboardEvent); // handle gameplay keyboard event 
        break;
    case S_LEVEL1:
        break;
    case S_LEVEL2:
        break;
    case S_LEVEL3:
        break;
    case S_VICTORY:
        break;
    }
}

//--------------------------------------------------------------
// Purpose  : This is the handler for the mouse input. Whenever there is a mouse event, this function will be called.
//            Ideally, you should pass the key event to a specific function to handle the event.
//            This is because in some states, some keys would be disabled. Hence, to reduce your code complexity, 
//            it would be wise to split your keyboard input handlers separately.
//            
//            For the mouse event, if the mouse is not moved, no event will be sent, hence you should not reset the mouse status.
//            However, if the mouse goes out of the window, you would not be able to know either. 
//
//            The MOUSE_EVENT_RECORD struct has more attributes that you can use, if you are adventurous enough.
//
//            In this case, we are not handling any mouse event in the Splashscreen state
//            
// Input    : const MOUSE_EVENT_RECORD& mouseEvent - reference to a mouse event struct
// Output   : void
//--------------------------------------------------------------
void mouseHandler(const MOUSE_EVENT_RECORD& mouseEvent)
{
    switch (g_eGameState)
    {
    case S_SPLASHSCREEN: // don't handle anything for the splash screen
        break;
    case S_GAME: gameplayMouseHandler(mouseEvent); // handle gameplay mouse event
        break;
    case S_LEVEL1:
        break;
    case S_LEVEL2:
        break;
    case S_LEVEL3:
        break;
    case S_VICTORY:
        break;
    }
}

//--------------------------------------------------------------
// Purpose  : This is the keyboard handler in the game state. Whenever there is a keyboard event in the game state, this function will be called.
//            
//            Add more keys to the enum in game.h if you need to detect more keys
//            To get other VK key defines, right click on the VK define (e.g. VK_UP) and choose "Go To Definition" 
//            For Alphanumeric keys, the values are their ascii values (uppercase).
// Input    : const KEY_EVENT_RECORD& keyboardEvent
// Output   : void
//--------------------------------------------------------------
void gameplayKBHandler(const KEY_EVENT_RECORD& keyboardEvent)
{
    // here, we map the key to our enums
    EKEYS key = K_COUNT;
    switch (keyboardEvent.wVirtualKeyCode)
    {
        //case VK_UP: key = K_UP; break;
        //case VK_DOWN: key = K_DOWN; break;
    case 'A': key = K_LEFT; break;
    case 'D': key = K_RIGHT; break;
    case VK_SPACE: key = K_SPACE; break;
    case VK_ESCAPE: key = K_ESCAPE; break;
    }
    // a key pressed event would be one with bKeyDown == true
    // a key released event would be one with bKeyDown == false
    // if no key is pressed, no event would be fired.
    // so we are tracking if a key is either pressed, or released
    if (key != K_COUNT)
    {
        g_skKeyEvent[key].keyDown = keyboardEvent.bKeyDown;
        g_skKeyEvent[key].keyReleased = !keyboardEvent.bKeyDown;
    }
}

//--------------------------------------------------------------
// Purpose  : This is the mouse handler in the game state. Whenever there is a mouse event in the game state, this function will be called.
//            
//            If mouse clicks are detected, the corresponding bit for that mouse button will be set.
//            mouse wheel, 
//            
// Input    : const KEY_EVENT_RECORD& keyboardEvent
// Output   : void
//--------------------------------------------------------------
void gameplayMouseHandler(const MOUSE_EVENT_RECORD& mouseEvent)
{
    if (mouseEvent.dwEventFlags & MOUSE_MOVED) // update the mouse position if there are no events
    {
        g_mouseEvent.mousePosition = mouseEvent.dwMousePosition;
    }
    g_mouseEvent.buttonState = mouseEvent.dwButtonState;
    g_mouseEvent.eventFlags = mouseEvent.dwEventFlags;
}

//--------------------------------------------------------------
// Purpose  : Update function
//            This is the update function
//            double dt - This is the amount of time in seconds since the previous call was made
//
//            Game logic should be done here.
//            Such as collision checks, determining the position of your game characters, status updates, etc
//            If there are any calls to write to the console here, then you are doing it wrong.
//
//            If your game has multiple states, you should determine the current state, and call the relevant function here.
//
// Input    : dt = deltatime
// Output   : void
//--------------------------------------------------------------
void update(double dt)
{
    // get the delta time
    g_dElapsedTime += dt;
    g_dDeltaTime = dt;

    if (TutorialEnemies == 0) { // if tutorial is cleared, go to the next Level. Level 1.
        g_eGameState = S_LEVEL1;
        if (AllEnemiesCleared == true)
        {
            resetPlayer();
            spawnFire(1);
            AllEnemiesCleared = false;
        }
        updateGame();
        renderGame();
    }

    else if (Level1Enemies == 0 && stagecounter == 2) { //Level 2
        g_eGameState = S_LEVEL2;
        if (AllEnemiesCleared == true)
        {
            resetPlayer();
            spawnFire(2);
            AllEnemiesCleared = false;
        }
        updateGame();
        renderGame();
    }

    else if (Level2Enemies == 0 && stagecounter == 3) { //Level 2
        g_eGameState = S_LEVEL3;
        if (AllEnemiesCleared == true)
        {
            resetPlayer();
            spawnFire(3);
            AllEnemiesCleared = false;
        }
        updateGame();
        renderGame();
    }

    switch (g_eGameState)
    {
    case S_SPLASHSCREEN: splashScreenWait(); // game logic for the splash screen
        break;
    case S_GAME: updateGame(); // gameplay logic when we are in the game
        break;
    case S_LEVEL1: splashScreenWait();
        break;
    case S_LEVEL2: splashScreenWait();
        break;
    case S_LEVEL3: splashScreenWait();
        break;
    case S_VICTORY: renderSplashScreenGameOver();
        break;
    case S_GAMEOVER: renderSplashScreenGameOver();
        break;
    }
}

void splashScreenWait()    // waits for time to pass in splash screen
{
    if (g_dElapsedTime > 4.0) // wait for 3 seconds to switch to game mode, else do nothing
        g_eGameState = S_GAME;
}

void updateGame()       // gameplay logic
{
    processUserInput(); // checks if you should change states or do something else with the game, e.g. pause, exit
    if (!gameOver)
    {
        moveCharacter();    // moves the character, collision detection, physics, etc
        moveFire();         // moves the enemies
                            // sound can be played here too.
    }
}

void moveCharacter()
{
    // Updating the location of the character based on the key release
    // providing a beep sound whenver we shift the character
    if (g_skKeyEvent[K_LEFT].keyReleased && g_sChar.m_cLocation.X >= 30)
    {
        Beep(1440, 30);
        if (g_sChar.m_cLocation.X > 30) {
            g_sChar.m_cLocation.X--;
            playerMove++;
            fireMove = TRUE;
            enemyBulletMove = TRUE;
        }
        else if (g_sChar.m_cLocation.X == 30) {
            Beep(1109, 30);
        }
    }
    if (g_skKeyEvent[K_RIGHT].keyReleased && g_sChar.m_cLocation.X <= 47)
    {
        Beep(1440, 30);
        if (g_sChar.m_cLocation.X < 47) {
            g_sChar.m_cLocation.X++;
            playerMove++;
            fireMove = TRUE;
            enemyBulletMove = TRUE;
        }
        else if (g_sChar.m_cLocation.X == 47) {
            Beep(1109, 30);
        }
    }
    if (g_skKeyEvent[K_SPACE].keyReleased)
    {
        shootBullet();
        // g_sChar.m_bActive = !g_sChar.m_bActive;
        playerMove++;
        fireMove = TRUE;
        enemyBulletMove = TRUE;
    }
}
void moveFire()
{
    int randomizer = rand() % 2;
    if (fireMove)
    {
        if (g_sBoss[0] != nullptr)
        {
            if (randomizer == 0 && playerMove % 6 == 0 && g_sBoss[0]->bossLocation.X < 47)
                g_sBoss[0]->bossLocation.X++;
            else if (randomizer == 1 && playerMove % 6 == 0 && g_sBoss[0]->bossLocation.X > 30)
                g_sBoss[0]->bossLocation.X--;
        }
        if (stagecounter == 2 || stagecounter == 3)
        {
            for (int i = 0; i < sizeof(g_sFire) / sizeof(*g_sFire); i++)
            {
                if (g_sFire[i] != nullptr && g_sFire[i]->advancedFire)
                {
                    int chanceToFire = rand() % 3;
                    if (playerMove % 8 == 0)
                    {
                        if (chanceToFire == 2)
                        {
                            enemyShoot(g_sFire[i]->fireLocation.X, g_sFire[i]->fireLocation.Y);
                        }
                    }
                    if (randomizer == 0 && playerMove % 10 == 0 && g_sFire[i]->fireLocation.X < 47)
                        g_sFire[i]->fireLocation.X++;
                    else if (randomizer == 1 && playerMove % 10 == 0 && g_sFire[i]->fireLocation.X > 30)
                        g_sFire[i]->fireLocation.X--;
                }
            }
        }
        if (playerMove % 18 == 0)
        {
            if (g_sBoss[0] != nullptr)
            {
                g_sBoss[0]->bossLocation.Y++;
                if (g_sBoss[0]->bossLocation.Y > 23)
                {
                    delete g_sBoss[0];
                    g_sBoss[0] = nullptr;
                    LivesLeft = 0;
                }
            }
            for (int i = 0; i < sizeof(g_sFire) / sizeof(*g_sFire); i++)
            {
                if (g_sFire[i] != nullptr)
                {
                    g_sFire[i]->fireLocation.Y++;

                    if (g_sFire[i]->fireLocation.Y > 23)
                    {
                        delete g_sFire[i];
                        g_sFire[i] = nullptr;
                        LivesLeft = LivesLeft - 1;
                        TutorialEnemies--;
                        if (stagecounter == 1) {
                            Level1Enemies--;
                        }
                        else if (stagecounter == 2) {
                            Level2Enemies--;
                        }
                        else if (stagecounter == 3) {
                            Level3Enemies--;
                        }
                    }
                    if (stagecounter == 0 && TutorialEnemies == 0) {
                        if (LivesLeft <= 0) {
                            clearScreen();
                            renderSplashScreenGameOver();
                            break;
                        }
                        g_dElapsedTime = 0;
                        g_eGameState = S_LEVEL1;
                        stagecounter++;
                        AllEnemiesCleared = true;

                    }
                    else if (stagecounter == 1 && Level1Enemies == 0) {
                        if (LivesLeft <= 0) {
                            clearScreen();
                            renderSplashScreenGameOver();
                            break;
                        }
                        g_dElapsedTime = 0;
                        g_eGameState = S_LEVEL2;
                        stagecounter++;
                        AllEnemiesCleared = true;

                    }
                    else if (stagecounter == 2 && Level2Enemies == 0) {
                        if (LivesLeft <= 0) {
                            clearScreen();
                            renderSplashScreenGameOver();
                            break;
                        }
                        g_dElapsedTime = 0;
                        g_eGameState = S_LEVEL3;
                        stagecounter++;
                        AllEnemiesCleared = true;

                    }
                    else if (stagecounter == 3 && Level3Enemies == 0) {
                        if (LivesLeft <= 0) {
                            clearScreen();
                            renderSplashScreenGameOver();
                            break;
                        }
                        clearScreen();
                        renderSplashScreenVictory();
                        break;
                    }
                }
            }
        }
        fireMove = false;
    }
}
void renderEnemyBullet()
{
    for (int i = 0; i < 25; i++)
    {
        if (g_enemyBullet[i] != nullptr)
        {
            g_Console.writeToBuffer(g_enemyBullet[i]->bulletLocation, "Û", 0x0C);
            if (g_enemyBullet[i]->bulletLocation.Y == 23)
            {
                if (g_enemyBullet[i] != nullptr)
                {
                    delete g_enemyBullet[i];
                    g_enemyBullet[i] = nullptr;
                }
            }
            else if (enemyBulletMove)
            {
                g_enemyBullet[i]->bulletLocation.Y += 1;
            }
            enemyBulletCollision();
        }
    }
    enemyBulletMove = false;
}
void shootBullet()
{
    for (int i = 0; i < 5; i++)
    {
        if (g_bullet[i] == nullptr)
        {
            g_bullet[i] = new SBulletChar;
            g_bullet[i]->bulletLocation.X = g_sChar.m_cLocation.X;
            g_bullet[i]->bulletLocation.Y = 24;
            break;
        }
    }
}
void enemyShoot(int x, int y)
{
    for (int i = 0; i < 25; i++)
    {
        if (g_enemyBullet[i] == nullptr)
        {
            g_enemyBullet[i] = new SBulletChar;
            g_enemyBullet[i]->bulletLocation.X = x;
            g_enemyBullet[i]->bulletLocation.Y = y;
            break;
        }
    }
}
void renderMenuStats() {

    COORD startPos = { 50, 5 };
    std::ostringstream ss;
    std::string stats;
    for (int i = 2; i < K_COUNT; ++i)
    {
        ss.str("");
        switch (i)
        {
        case K_LEFT:
            ss << "Lives left:", stats = "", ss << LivesLeft;
            break;
        case K_RIGHT:
            if (stagecounter == 1) {
                ss << "Enemies Left:", stats = "", ss << Level1Enemies;
            }
            else if (stagecounter == 2) {
                ss << "Enemies Left:", stats = "", ss << Level2Enemies;
            }
            else if (stagecounter == 3) {
                ss << "Enemies Left:", stats = "", ss << Level3Enemies;
            }
            else
                ss << "Enemies Left:", stats = "", ss << TutorialEnemies;
            break;
        case K_SPACE:
            if (stagecounter == 3)
                ss << "Boss Health:", stats = "", ss << bossHealthCounter;
            break;
        default:ss << "Stage:", stats = "", ss << stagecounter;
            break;
        }

        ss << stats;

        COORD c = { startPos.X, startPos.Y + i };
        g_Console.writeToBuffer(c, ss.str(), FOREGROUND_GREEN);
    }
}

void bulletCollision()
{
    for (int f = 0; f < 100; f++)
    {
        for (int b = 0; b < 5; b++)
        {
            if (g_bullet[b] != nullptr)
            {
                if (g_sFire[f] != nullptr)
                {
                    if (g_bullet[b]->bulletLocation.X == g_sFire[f]->fireLocation.X && g_bullet[b]->bulletLocation.Y == g_sFire[f]->fireLocation.Y)
                    {
                        TutorialEnemies--;

                        delete g_bullet[b];
                        g_bullet[b] = nullptr;

                        g_sFire[f]->fireHealth--;

                        if (g_sFire[f]->fireHealth == 0)
                        {
                            delete g_sFire[f];
                            g_sFire[f] = nullptr;
                            if (stagecounter == 1)
                                Level1Enemies--;
                            if (stagecounter == 2)
                                Level2Enemies--;
                            if (stagecounter == 3)
                                Level3Enemies--;
                        }
                        Beep(1440, 30);

                        if (stagecounter == 0 && TutorialEnemies == 0) {
                            g_dElapsedTime = 0;
                            g_eGameState = S_LEVEL1;
                            stagecounter++;
                            AllEnemiesCleared = true;
                        }
                        else if (stagecounter == 1 && Level1Enemies == 0) {
                            g_dElapsedTime = 0;
                            g_eGameState = S_LEVEL2;
                            stagecounter++;
                            AllEnemiesCleared = true;
                        }
                        else if (stagecounter == 2 && Level2Enemies == 0) {
                            g_dElapsedTime = 0;
                            g_eGameState = S_LEVEL3;
                            stagecounter++;
                            AllEnemiesCleared = true;
                        }
                        else if (stagecounter == 3 && Level3Enemies == 0) {
                            g_eGameState = S_VICTORY;
                            clearScreen();
                            renderSplashScreenVictory();
                            break;
                        }
                    }
                }
                if (g_bullet[b] != nullptr)
                {
                    if (g_sBoss[0] != nullptr)
                    {
                        if (g_bullet[b]->bulletLocation.X == g_sBoss[0]->bossLocation.X && g_bullet[b]->bulletLocation.Y == g_sBoss[0]->bossLocation.Y)
                        {
                            delete g_bullet[b];
                            g_bullet[b] = nullptr;

                            g_sBoss[0]->bossHealth--;
                            bossHealthCounter--;
                            Beep(1000, 30);
                            if (g_sBoss[0]->bossHealth == 0)
                            {
                                delete g_sBoss[1];
                                g_sBoss[0] = nullptr;
                                Level3Enemies--;
                            }
                            if (stagecounter == 3 && Level3Enemies == 0) {
                                g_eGameState = S_VICTORY;
                                clearScreen();
                                renderSplashScreenVictory();
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}
void enemyBulletCollision()
{
    for (int i = 0; i < 25; i++)
    {
        if (g_enemyBullet[i] != nullptr)
        {
            if (g_enemyBullet[i]->bulletLocation.X == g_sChar.m_cLocation.X && g_enemyBullet[i]->bulletLocation.Y == g_sChar.m_cLocation.Y)
            {
                delete g_enemyBullet[i];
                g_enemyBullet[i] = nullptr;
                LivesLeft -= 1;
                Beep(450, 30);
            }
        }
    }
}
void spawnFire(int wave)
{
    switch (wave)
    {
    case 0:
        for (int i = 0; i < 36; i++)
        {
            if (g_sFire[i] == nullptr)
            {
                g_sFire[i] = new SFireChar;
                g_sFire[i]->fireHealth = 1;
                if (i < 18)
                {
                    g_sFire[i]->fireLocation.X = 30 + (i);
                    g_sFire[i]->fireLocation.Y = 8;
                }
                else
                {
                    g_sFire[i]->fireLocation.X = 12 + (i);
                    g_sFire[i]->fireLocation.Y = 7;
                }
            }
        }
        break;
    case 1:
        for (int i = 0; i < 90; i++)
        {
            if (g_sFire[i] == nullptr)
            {
                g_sFire[i] = new SFireChar;
                g_sFire[i]->fireHealth = 1;
                if (i < 18)
                {
                    g_sFire[i]->fireLocation.X = 30 + (i);
                    g_sFire[i]->fireLocation.Y = 11;
                }
                else if (i >= 18 && i < 36)
                {
                    g_sFire[i]->fireLocation.X = 12 + (i);
                    g_sFire[i]->fireLocation.Y = 10;
                }
                else if (i >= 36 && i < 54)
                {
                    g_sFire[i]->fireLocation.X = -6 + (i);
                    g_sFire[i]->fireLocation.Y = 9;
                }
                else if (i >= 54 && i < 72)
                {
                    g_sFire[i]->fireLocation.X = -24 + (i);
                    g_sFire[i]->fireLocation.Y = 8;
                }
                else
                {
                    g_sFire[i]->fireLocation.X = -42 + (i);
                    g_sFire[i]->fireLocation.Y = 7;
                }
            }
        }
        break;
    case 2:
        for (int i = 0; i < 16; i++)
        {
            if (g_sFire[i] == nullptr)
            {
                g_sFire[i] = new SFireChar;
                g_sFire[i]->advancedFire = true;
                g_sFire[i]->fireHealth = 3;
                if (i < 10)
                {
                    g_sFire[i]->fireLocation.X = 34 + (i);
                    g_sFire[i]->fireLocation.Y = 9;
                }
                else if (i >= 10)
                {
                    g_sFire[i]->fireLocation.X = 26 + (i);
                    g_sFire[i]->fireLocation.Y = 10;
                }
            }
        }
        break;
    case 3:
        for (int i = 0; i < 45; i++)
        {
            if (g_sFire[i] == nullptr)
            {
                g_sFire[i] = new SFireChar;
                if (i < 18)
                {
                    g_sFire[i]->fireHealth = 1;
                    g_sFire[i]->fireLocation.X = 30 + (i);
                    g_sFire[i]->fireLocation.Y = 11;
                }
                else if (i >= 18 && i < 36)
                {
                    g_sFire[i]->fireHealth = 1;
                    g_sFire[i]->fireLocation.X = 12 + (i);
                    g_sFire[i]->fireLocation.Y = 10;
                }
                else if (i >= 36 && i < 45)
                {
                    g_sFire[i]->advancedFire = true;
                    g_sFire[i]->fireHealth = 3;
                    g_sFire[i]->fireLocation.X = -1 + (i);
                    g_sFire[i]->fireLocation.Y = 9;
                }
            }
            if (g_sBoss[0] == nullptr)
            {
                g_sBoss[0] = new SBossFireChar;
                g_sBoss[0]->bossHealth = 50;
                g_sBoss[0]->bossLocation.X = 40;
                g_sBoss[0]->bossLocation.Y = 7;
            }
        }
    }
}
void resetPlayer()
{
    for (int i = 0; i < 5; i++)
    {
        delete g_bullet[i];
        g_bullet[i] = nullptr;
    }
    for (int g = 0; g < 25; g++)
    {
        delete g_enemyBullet[g];
        g_enemyBullet[g] = nullptr;
    }
    g_sChar.m_cLocation.X = 39;
    playerMove = 0;
}
void processUserInput()
{
    // quits the game if player hits the escape key
    if (g_skKeyEvent[K_ESCAPE].keyReleased)
        g_bQuitGame = true;
}

//--------------------------------------------------------------
// Purpose  : Render function is to update the console screen
//            At this point, you should know exactly what to draw onto the screen.
//            Just draw it!
//            To get an idea of the values for colours, look at console.h and the URL listed there
// Input    : void
// Output   : void
//--------------------------------------------------------------
void render()
{
    clearScreen();      // clears the current screen and draw from scratch 
    switch (g_eGameState)
    {
    case S_SPLASHSCREEN: renderSplashScreen();
        break;
    case S_GAME: renderGame();
        break;
    case S_LEVEL1: renderSplashScreen();
        break;
    case S_LEVEL2: renderSplashScreen();
        break;
    case S_LEVEL3: renderSplashScreen();
        break;
    }
    renderFramerate();      // renders debug information, frame rate, elapsed time, etc
    renderToScreen();
}

void clearScreen()
{
    // Clears the buffer with this colour attribute
    g_Console.clearBuffer();
}

void renderToScreen()
{
    // Writes the buffer to the console, hence you will see what you have written
    g_Console.flushBufferToConsole();
}

void renderMisc()
{
    COORD bs1 = g_Console.getConsoleSize();
    COORD bs2 = g_Console.getConsoleSize();
    COORD bs3 = g_Console.getConsoleSize();
    COORD bs4 = g_Console.getConsoleSize();


    bs1.Y = 0;
    bs1.X = 5;

    g_Console.writeToBuffer(bs1, "  ____  _            _       _____                                     ", 0x0B);
    bs1.Y++;
    g_Console.writeToBuffer(bs1, " |  _ || |          | |     / ____|                                    ", 0x0B);
    bs1.Y++;
    g_Console.writeToBuffer(bs1, " | |_) | | __ _  ___| | __ | (___  _   _ _ __ ___  _ __ ___   ___ _ __ ", 0x0B);
    bs1.Y++;
    g_Console.writeToBuffer(bs1, " |  _ <| |/ _` |/ __| |/ /  |___ || | | | '_ ` _ || '_ ` _ | / _ | '__|", 0x0B);
    bs1.Y++;
    g_Console.writeToBuffer(bs1, " | |_) | | (_| | (__|   <   ____) | |_| | | | | | | | | | | |  __/ |   ", 0x0B);
    bs1.Y++;
    g_Console.writeToBuffer(bs1, " |____/|_||__,_||___|_||_| |_____/ |__,_|_| |_| |_|_| |_| |_||___|_|   ", 0x0B);

    bs2.Y = 10;
    bs2.X = 0;

    for (int i = 0; i <= 79; i++)
    {
        g_Console.writeToBuffer(bs2, "_", 0x0B);
        bs2.X++;
    }

    bs3.Y = 12;
    bs3.X = 49;

    g_Console.writeToBuffer(bs3, "               ,@@@@@@@,", 0x0A);
    bs3.Y++;
    g_Console.writeToBuffer(bs3, "       ,,,.   ,@@@@@@/@@,  ", 0x0A);
    bs3.Y++;
    g_Console.writeToBuffer(bs3, "    ,&%%&%&&%,@@@@@/@@@@@@,", 0x0A);
    bs3.Y++;
    g_Console.writeToBuffer(bs3, "   ,%&\%&&%&&%,@@@\@@@/@@@", 0x0A);
    bs3.Y++;
    g_Console.writeToBuffer(bs3, "   %&&%&%&/%&&%@@|@@/ /@@@", 0x0A);
    bs3.Y++;
    g_Console.writeToBuffer(bs3, "   %&&%/ %&%%&&@@| V /@@' ", 0x0A);
    bs3.Y++;
    g_Console.writeToBuffer(bs3, "   `&%\ ` /%&'    |.|        '", 0x0A);
    bs3.Y++;
    g_Console.writeToBuffer(bs3, "       |o|        | |         ", 0x0A);
    bs3.Y++;
    g_Console.writeToBuffer(bs3, "       |.|        | |         ", 0x0A);
    bs3.Y++;
    g_Console.writeToBuffer(bs3, "    |\/ ._|//_/__/  ,\_//__\|", 0x0A);

    bs4.Y = 12;
    bs4.X = 3;

    g_Console.writeToBuffer(bs4, "  ,@@@@@@@,", 0x0A);
    bs4.Y++;
    g_Console.writeToBuffer(bs4, " ,@@@@@@/@@,  .oo8888o.", 0x0A);
    bs4.Y++;
    g_Console.writeToBuffer(bs4, ",@@@@@/@@@@@@,8888\88/8o", 0x0A);
    bs4.Y++;
    g_Console.writeToBuffer(bs4, ",@@@|@@@/@@@88|88888/88'", 0x0A);
    bs4.Y++;
    g_Console.writeToBuffer(bs4, " @@\@@/ /@@@88888\88888'", 0x0A);
    bs4.Y++;
    g_Console.writeToBuffer(bs4, " @@\ V /@@' `88\8 `/88'", 0x0A);
    bs4.Y++;
    g_Console.writeToBuffer(bs4, "    |.|         \ '|8''", 0x0A);
    bs4.Y++;
    g_Console.writeToBuffer(bs4, "    | |         | |", 0x0A);
    bs4.Y++;
    g_Console.writeToBuffer(bs4, "    | |         | | ", 0x0A);
    bs4.Y++;
    g_Console.writeToBuffer(bs4, " __/  ,\_//__\\/.  \_//__/_", 0x0A);
}


void renderSplashScreen()  // renders the splash screen
{
    renderMisc();
    COORD c = g_Console.getConsoleSize();

    c.Y /= 3;
    c.X = c.X / 2 - 5;

    if (stagecounter == 0) {

        g_Console.writeToBuffer(c, "TUTORIAL STAGE", 0x03);

    }
    if (stagecounter == 1) {
        g_Console.writeToBuffer(c, "STAGE 1", 0x03);
    }
    if (stagecounter == 2) {
        g_Console.writeToBuffer(c, "STAGE 2", 0x03);
    }
    if (stagecounter == 3) {
        g_Console.writeToBuffer(c, "STAGE 3", 0x03);
    }
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 25;
    g_Console.writeToBuffer(c, "Press <Space> to fire and change character colour", 0x09);
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 9;
    g_Console.writeToBuffer(c, "Press 'Esc' to quit", 0x09);
}

void renderSplashScreenGameOver()
{
    clearScreen();
    gameOver = true;
    COORD c = g_Console.getConsoleSize();
    COORD v = g_Console.getConsoleSize();
    v.Y = 0;
    v.X = 12;
    g_Console.writeToBuffer(v, " _____                           _____                    ", 0x03);
    v.Y++;
    g_Console.writeToBuffer(v, "|  __ |                         |  _  |                   ", 0x03);
    v.Y++;
    g_Console.writeToBuffer(v, "| |  |/  __ _  _ __ ___    ___  | | | |__   __  ___  _ __ ", 0x03);
    v.Y++;
    g_Console.writeToBuffer(v, "| | __  / _` || '_ ` _ |  / _ | | | | || | / / / _ || '__|", 0x03);
    v.Y++;
    g_Console.writeToBuffer(v, "| |_| || (_| || | | | | ||  __/ | |_/ / | V / |  __/| |   ", 0x03);
    v.Y++;
    g_Console.writeToBuffer(v, " |____/ |__,_||_| |_| |_| |___|  |___/   |_/   |___||_|   ", 0x03);

    c.Y /= 3;
    c.X = c.X / 2 - 5;
    g_Console.writeToBuffer(c, "GAME OVER YOU LOST!", 0x03);
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 9;
    g_Console.writeToBuffer(c, "Press 'Esc' to quit", 0x09);

}
void renderSplashScreenVictory()
{
    clearScreen();
    gameOver = true;
    COORD c = g_Console.getConsoleSize();
    COORD v = g_Console.getConsoleSize();
    v.Y = 0;
    v.X = 20;
    g_Console.writeToBuffer(v, "__   __                _    _  _         _ ", 0x03);
    v.Y++;
    g_Console.writeToBuffer(v, "| | / /               | |  | |(_)       | |", 0x03);
    v.Y++;
    g_Console.writeToBuffer(v, " | V /   ___   _   _  | |  | | _  _ __  | |", 0x03);
    v.Y++;
    g_Console.writeToBuffer(v, "  | /   / _ | | | | | | |/|| || || '_ | | |", 0x03);
    v.Y++;
    g_Console.writeToBuffer(v, "  | |  | (_) || |_| | |  /|  /| || | | ||_|", 0x03);
    v.Y++;
    g_Console.writeToBuffer(v, "  |_/   |___/  |__,_|  |/  |/ |_||_| |_|(_)", 0x03);

    c.Y /= 3;
    c.X = c.X / 2 - 5;
    g_Console.writeToBuffer(c, "GAME OVER YOU WIN!", 0x03);
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 9;
    g_Console.writeToBuffer(c, "Press 'Esc' to quit", 0x09);
}

void renderGame()
{
    renderInputEvents();
    renderMap();        // renders the map to the buffer first
    renderFire();
    renderBullet();     // renders the bullets into the buffer
    renderEnemyBullet();
    renderCharacter();
    renderMenuStats();// renders the character into the buffer
    if (LivesLeft <= 0) {
        clearScreen();
        renderSplashScreenGameOver();
    }
    if (stagecounter == 3 && Level3Enemies == 0) {
        clearScreen();
        renderSplashScreenVictory();
    }
}

void renderMap()
{
    renderMisc();

    /*
    // Set up sample colours, and output shadings
    const WORD colors[] = {
        0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F,
        0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6
    };
    COORD c;
    for (int i = 0; i < 12; ++i)
    {
        c.X = 5 * i;
        c.Y = i + 1;
        colour(colors[i]);
        g_Console.writeToBuffer(c, " °±²Û", colors[i]);
    }
    */

    COORD Map = g_Console.getConsoleSize();
    Map.Y /= 4;
    Map.X = Map.X / 2 - 11;
    g_Console.writeToBuffer(Map, "ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ", 0x03);
    for (int i = 0; i < 17; i++)
    {
        Map.Y += 1;
        Map.X = g_Console.getConsoleSize().X / 2 - 11;
        g_Console.writeToBuffer(Map, "Û                  Û", 0x09);
    }
    Map.Y += 1;
    Map.X = g_Console.getConsoleSize().X / 2 - 11;
    g_Console.writeToBuffer(Map, "ÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛÛ", 0x03);
}

void renderBullet()
{
    for (int i = 0; i < 5; i++)
    {
        if (g_bullet[i] != nullptr)
        {
            if (g_bullet[i]->bulletLocation.Y == 7)
            {
                if (g_bullet[i] != nullptr)
                {
                    delete g_bullet[i];
                    g_bullet[i] = nullptr;
                }
            }
            else
            {
                g_bullet[i]->bulletLocation.Y -= 1;
                g_Console.writeToBuffer(g_bullet[i]->bulletLocation, "|", 0x03);
            }
            bulletCollision();
        }
    }
}

void renderCharacter()
{
    // Draw the location of the character
    WORD charColor = 0x0A;
    if (LivesLeft <= 3 && LivesLeft > 1)
    {
        charColor = FOREGROUND_RED | FOREGROUND_GREEN;
    }
    if (LivesLeft == 1)
    {
        charColor = FOREGROUND_RED;
    }
    g_Console.writeToBuffer(g_sChar.m_cLocation, "P", charColor);
}


void renderFire()
{
    for (int i = 0; i < sizeof(g_sFire) / sizeof(*g_sFire); i++)
    {
        if (g_sFire[i] != nullptr)
        {
            if (g_sFire[i]->fireHealth > 1)
                g_Console.writeToBuffer(g_sFire[i]->fireLocation, "F", 0x0C);
            else
                g_Console.writeToBuffer(g_sFire[i]->fireLocation, "F", FOREGROUND_RED | FOREGROUND_GREEN);
        }
        else if (g_sBoss[0] != nullptr)
        {
            if (g_sBoss[0]->bossHealth <= 30 && g_sBoss[0]->bossHealth > 10)
                g_Console.writeToBuffer(g_sBoss[0]->bossLocation, "B", FOREGROUND_RED | FOREGROUND_GREEN);
            else if (g_sBoss[0]->bossHealth <= 10)
                g_Console.writeToBuffer(g_sBoss[0]->bossLocation, "B", FOREGROUND_RED);
            else
                g_Console.writeToBuffer(g_sBoss[0]->bossLocation, "B", 0x0D);
        }
    }
}

void renderFramerate()
{
    COORD c;
    // displays the framerate
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(3);
    ss << 1.0 / g_dDeltaTime << "fps";
    c.X = g_Console.getConsoleSize().X - 9;
    c.Y = 0;
    g_Console.writeToBuffer(c, ss.str());

    // displays the elapsed time
    ss.str("");
    ss << g_dElapsedTime << "secs";
    c.X = 0;
    c.Y = 0;
    g_Console.writeToBuffer(c, ss.str(), 0x03);
}


// this is an example of how you would use the input events
void renderInputEvents()
{
    // keyboard events
    COORD startPos = { 8, 5 };
    std::ostringstream ss;
    std::string key;
    for (int i = 2; i < K_COUNT; ++i)
    {
        ss.str("");
        switch (i)
        {
        case K_LEFT: key = "A to move left";
            break;
        case K_RIGHT: key = "D to move right";
            break;
        case K_SPACE: key = "";
            break;
        default: key = "SPACE TO SHOOT";
        }
        ss << key;

        COORD c = { startPos.X, startPos.Y + i };
        g_Console.writeToBuffer(c, ss.str(), FOREGROUND_RED);
    }
}
