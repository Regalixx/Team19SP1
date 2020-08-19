// This is the main file for the game logic and function
//
//
#include "game.h"
#include "Framework\console.h"
#include <iostream>
#include <iomanip>
#include <sstream>

double  g_dElapsedTime;
double  g_dDeltaTime;
SKeyEvent g_skKeyEvent[K_COUNT];
SMouseEvent g_mouseEvent;


// Game specific variables here
SGameChar   g_sChar;
SBulletChar* g_bullet[5] = {};
SFireChar* g_sFire[36] = {};
int scorecounter = 0;
int playerMove = 0;
bool fireMove = false;
EGAMESTATES g_eGameState = S_SPLASHSCREEN; // initial state

// Console object
Console g_Console(80, 25, "testest");//setconsolesize

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

    g_sChar.m_cLocation.X = g_Console.getConsoleSize().X / 2;
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
    case VK_UP: key = K_UP; break;
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

    switch (g_eGameState)
    {
    case S_SPLASHSCREEN: splashScreenWait(); // game logic for the splash screen
        break;
    case S_GAME: updateGame(); // gameplay logic when we are in the game
        break;
    }
}


void splashScreenWait()    // waits for time to pass in splash screen
{
    if (g_dElapsedTime > 3.0) // wait for 3 seconds to switch to game mode, else do nothing
        g_eGameState = S_GAME;
}

void updateGame()       // gameplay logic
{
    processUserInput(); // checks if you should change states or do something else with the game, e.g. pause, exit
    moveCharacter();    // moves the character, collision detection, physics, etc
    moveFire();
                        // sound can be played here too.
}

void moveCharacter()
{
    // Updating the location of the character based on the key release
    // providing a beep sound whenver we shift the character
    if (g_skKeyEvent[K_LEFT].keyReleased && g_sChar.m_cLocation.X > 30)
    {
        Beep(1440, 30);
        g_sChar.m_cLocation.X--;
        playerMove++;
        fireMove = true;
    }
    if (g_skKeyEvent[K_RIGHT].keyReleased && g_sChar.m_cLocation.X < 47)
    {
        Beep(1440, 30);
        g_sChar.m_cLocation.X++;
        playerMove++;
        fireMove = true;
    }
    if (g_skKeyEvent[K_SPACE].keyReleased)
    {
        shootBullet();
        g_sChar.m_bActive = !g_sChar.m_bActive;
        playerMove++;
        fireMove = true;
    }


}

void moveFire()
{
    if (fireMove)
    {
        if (playerMove % 8 == 0)
        {
            for (int i = 0; i < sizeof(g_sFire) / sizeof(*g_sFire); i++)
            {
                if (g_sFire[i] != nullptr)
                    g_sFire[i]->fireLocation.Y++;
            }
        }
        fireMove = false;
    }
}
void shootBullet()
{
    for (int i = 0; i < 5; i++)
    {
        if (g_bullet[i] == nullptr)
        {
            g_bullet[i] = new SBulletChar;
            g_bullet[i]->bulletLocation.X = g_sChar.m_cLocation.X;
            g_bullet[i]->bulletLocation.Y = 22;
            break;
        }
    }
}
void bulletCollision()
{
    for (int f = 0; f < 36; f++)
    {
        for (int b = 0; b < 5; b++)
        {
            if (g_bullet[b] != nullptr)
            {
                if (g_sFire[f] != nullptr)
                {
                    if (g_bullet[b]->bulletLocation.X == g_sFire[f]->fireLocation.X && g_bullet[b]->bulletLocation.Y == g_sFire[f]->fireLocation.Y)
                    {
                        scorecounter++;
                        delete g_bullet[b];
                        g_bullet[b] = nullptr;

                        delete g_sFire[f];
                        g_sFire[f] = nullptr;
                        //Beep(440, 300);
                    }
                }
            }
        }
    }
}
void spawnFire(int wave)
{
    switch (wave)
    {
    case 0:
        for (int i = 0; i < sizeof(g_sFire) / sizeof(*g_sFire); i++)
        {
            if (g_sFire[i] == nullptr)
            {
                g_sFire[i] = new SFireChar;
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
            if (g_sFire[i] != nullptr)
            {
                g_Console.writeToBuffer(g_sFire[i]->fireLocation, "F", 0x0C);
            }
        }
    case 1:
        for (int i = 0; i < 18; i++)
        {
            if (g_sFire[i] == nullptr)
            {
                g_sFire[i] = new SFireChar;
                if (i < 9)
                {
                    g_sFire[i]->fireLocation.X = 36 + (i);
                    g_sFire[i]->fireLocation.Y = 10;
                }
                else
                {
                    g_sFire[i]->fireLocation.X = 27 + (i);
                    g_sFire[i]->fireLocation.Y = 11;
                }
            }
            if (g_sFire[i] != nullptr)
            {
                g_Console.writeToBuffer(g_sFire[i]->fireLocation, "F", 0x0C);
            }
        }
    }
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
    }
    renderFramerate();      // renders debug information, frame rate, elapsed time, etc
    renderInputEvents();    // renders status of input events
    renderToScreen();       // dump the contents of the buffer to the screen, one frame worth of game
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

void renderSplashScreen()  // renders the splash screen
{
    COORD c = g_Console.getConsoleSize();
    c.Y /= 3;
    c.X = c.X / 2 - 5;
    g_Console.writeToBuffer(c, "STAGE 1 - 0", 0x03);
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 25;
    g_Console.writeToBuffer(c, "Press <Space> to fire and change character colour", 0x09);
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 9;
    g_Console.writeToBuffer(c, "Press 'Esc' to quit", 0x09);
}

void renderMenuStats() {

    COORD startPos = { 50, 5 };
    std::ostringstream ss;
    std::string stats;
    const int LivesLeft = 5;
    const int Score = 0;
    const int Level = 1;
    const int playerMove = 0;
    for (int i = 2; i < K_COUNT; ++i)
    {
        ss.str("");
        switch (i)
        {
        case K_LEFT:
            ss << "Lives left:", stats = "", ss << LivesLeft;
            break;
        case K_RIGHT:
            ss << "Score:", stats = "", ss << scorecounter;
            break;
        case K_UP:
            ss << "player movement:", stats = "", ss << playerMove;
            break;
        case K_SPACE:
            ss << "";
            break;
        default:ss << "Stage:", stats = "", ss << Level;
        }

        ss << stats;


        COORD c = { startPos.X, startPos.Y + i };
        g_Console.writeToBuffer(c, ss.str(), FOREGROUND_GREEN);
    }
}

void renderGame()
{
    renderMap();        // renders the map to the buffer first
    renderFire();
    renderBullet();     // renders the bullets into the buffer
    renderCharacter();
    renderMenuStats();// renders the character into the buffer
}

void renderMap()
{
    COORD Map = g_Console.getConsoleSize();
    Map.Y /= 4;
    Map.X = Map.X / 2 - 11;
    g_Console.writeToBuffer(Map, "xxxxxxxxxxxxxxxxxxxx", 0x03);
    for (int i = 0; i < 17; i++)
    {
        Map.Y += 1;
        Map.X = g_Console.getConsoleSize().X / 2 - 11;
        g_Console.writeToBuffer(Map, "x                  x", 0x09);
    }
    Map.Y += 1;
    Map.X = g_Console.getConsoleSize().X / 2 - 11;
    g_Console.writeToBuffer(Map, "xxxxxxxxxxxxxxxxxxxx", 0x03);
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
                    Beep(100, 100);
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
    WORD charColor = 0x0C;
    if (g_sChar.m_bActive)
    {
        charColor = 0x0A;
    }
    g_Console.writeToBuffer(g_sChar.m_cLocation, "P", charColor);
}


void renderFire()
{
    for (int i = 0; i < 36; i++)
    {
        if (g_sFire[i] != nullptr)
        {
            g_Console.writeToBuffer(g_sFire[i]->fireLocation, "F", 0x0C);
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
    g_Console.writeToBuffer(c, ss.str(), 0x59);
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
        if (g_skKeyEvent[i].keyDown)
            ss << key << "pressed";
        else if (g_skKeyEvent[i].keyReleased)
            ss << key << " released";
        else
            ss << key;


        COORD c = { startPos.X, startPos.Y + i };
        g_Console.writeToBuffer(c, ss.str(), FOREGROUND_RED);
    }

    // mouse events    
    ss.str("");
    ss << "Mouse position (" << g_mouseEvent.mousePosition.X << ", " << g_mouseEvent.mousePosition.Y << ")";
    g_Console.writeToBuffer(g_mouseEvent.mousePosition, ss.str(), 0x59);
    ss.str("");
    switch (g_mouseEvent.eventFlags)
    {
    case 0:
        if (g_mouseEvent.buttonState == FROM_LEFT_1ST_BUTTON_PRESSED)
        {
            ss.str("Left Button Pressed");
            g_Console.writeToBuffer(g_mouseEvent.mousePosition.X, g_mouseEvent.mousePosition.Y + 1, ss.str(), 0x59);
        }
        else if (g_mouseEvent.buttonState == RIGHTMOST_BUTTON_PRESSED)
        {
            ss.str("Right Button Pressed");
            g_Console.writeToBuffer(g_mouseEvent.mousePosition.X, g_mouseEvent.mousePosition.Y + 2, ss.str(), 0x59);
        }
        else
        {
            ss.str("Some Button Pressed");
            g_Console.writeToBuffer(g_mouseEvent.mousePosition.X, g_mouseEvent.mousePosition.Y + 3, ss.str(), 0x59);
        }
        break;
    case DOUBLE_CLICK:
        ss.str("Double Clicked");
        g_Console.writeToBuffer(g_mouseEvent.mousePosition.X, g_mouseEvent.mousePosition.Y + 4, ss.str(), 0x59);
        break;
    case MOUSE_WHEELED:
        if (g_mouseEvent.buttonState & 0xFF000000)
            ss.str("Mouse wheeled down");
        else
            ss.str("Mouse wheeled up");
        g_Console.writeToBuffer(g_mouseEvent.mousePosition.X, g_mouseEvent.mousePosition.Y + 5, ss.str(), 0x59);
        break;
    default:
        break;
    }

}

