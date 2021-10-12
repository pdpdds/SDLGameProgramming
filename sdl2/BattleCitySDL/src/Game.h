#ifndef GAME_H
#define GAME_H

#include "Screen.h"
#include "Audio.h"
#include "Map.h"
#include "Sprites.h"
#include "Player.h"
#include "Renderer.h"
#include "Bullets.h"
#include "MapEditor.h"
#include "Enemy.h"
#include "Enemies.h"
#include "Menu.h"
#include "GameTimer.h"
#include "Items.h"
#include "UI.h"
#include "Animation.h"

class Game {
public:
    static Game &instance() {
        static Game game;
        return game;
    }

    void Init();
    int GameLoop();
    void ProcessEvents();
    void StartGameplay();
    void SetupPlayerTwo();
    void DeletePlayerTwo();

    bool isEnd() { return !gameIsRunning; }
    void StopGame() { gameIsRunning = false; }
    void SetGameState(GameStateType state);
    GameStateType GameState() { return gameState; }
    void SetGameLost(bool status) { gameIsLost = status; }
    bool GameLost() { return gameIsLost; }

    int TailSize() { return tailSize; }
    int GameScreenOffsetX() { return gameScreenOffsetX; }
    int GameScreenOffsetY() { return gameScreenOffsetY; }

    Screen*    GetScreen() { return m_Screen; }
    Audio*     GetAudio() { return m_Audio; }
    Map*     GetLevel() { return m_Map; }
    Sprites*   GetSprites() { return m_Sprites; }
    Renderer*  GetRenderer() { return m_Renderer; }
    Player*    GetPlayer() { return m_Player; }
    Player*    GetPlayerTwo() { return m_PlayerTwo; }
    Bullets*   GetBullets() { return m_Bullets; }
    Editor*    GetEditor() { return m_Editor; }
    Enemies*   GetEnemies() { return m_Enemies; }
    Menu*      GetMenu() { return m_Menu; }
    GameTimer* GetGameTimer() { return m_GameTimer; }
    Items*     GetItems() { return m_Items; }
    UI*       GetUI() { return m_UI; }
    Animation*   GetAnimation() { return m_Animation; }

private:
    Game() {
        m_Screen = new Screen;
        m_Audio = new Audio;
        m_Sprites = new Sprites;
        m_Map = new Map;
        m_Renderer = new Renderer;
        m_Player = new Player;
        m_PlayerTwo = NULL;
        m_Bullets = new Bullets;
        m_Editor = new Editor;
        m_Enemies = new Enemies;
        m_Menu = new Menu;
        m_GameTimer = new GameTimer;
        m_Items = new Items;
        m_UI = new UI;
        m_Animation = new Animation;

        gameIsRunning = true;
        tailSize = 16;
    }

    ~Game() {
        delete m_Screen;
        delete m_Audio;
        delete m_Map;
        delete m_Sprites;
        delete m_Renderer;
        delete m_Player;
        delete m_PlayerTwo;
        delete m_Bullets;
        delete m_Editor;
        delete m_Enemies;
        delete m_Menu;
        delete m_GameTimer;
        delete m_Items;
        delete m_UI;
        delete m_Animation;

        SDL_Quit();
    }

private:
    // time calc
    int ticksOld, ticksNew;
    double deltaTime;

    // FPS calc
    int FPSCount;
    int FPSMax;
    double FPSTime;

    bool gameIsLost;
    bool gameIsRunning;
    int tailSize;

    GameStateType gameState;

    int gameScreenOffsetX;
    int gameScreenOffsetY;

    Screen*     m_Screen;
    Audio*      m_Audio;
    Map*        m_Map;
    Sprites*    m_Sprites;
    Renderer*   m_Renderer;
    Player*     m_Player;
    Player*     m_PlayerTwo;
    Bullets*    m_Bullets;
    Editor*     m_Editor;
    Enemies*    m_Enemies;
    Menu*       m_Menu;
    GameTimer*  m_GameTimer;
    Items*      m_Items;
    UI*         m_UI;
    Animation*  m_Animation;
};

// TODO: optimize
inline bool CheckCollision(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
    if (x1 > x3) {
        swap(x1, x3); swap(y1, y3);
        swap(x2, x4); swap(y2, y4);
    }
    if (y1 > y2) swap(y1, y2);
    if (y3 > y4) swap(y3, y4);
    if (x1 > x2) swap(x1, x2);
    if (x3 > x4) swap(x3, x4);

    if (y1 > y3) {
        swap(y1, y3);
        swap(y2, y4);
    }
    if (y1 == y3 && y2 > y4)
        swap(y2, y4);

    if (x3 < x2 && y3 < y2) {
        return true;
    }
    return false;
}

#endif // GAME_H
