#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <iostream>
#include <string>
#include <ctime>
#include "Game.h"

using namespace std;

void Game::StartGameplay() {
    GetPlayer()->SetPosition(8, 0);
    GetPlayer()->SetAlive(false);
    GetPlayer()->SetDirection(DIR_UP);
    GetPlayer()->SetState(PLAYER_STOP);
    GetPlayer()->SetXVelocity(0);
    GetPlayer()->SetYVelocity(0);

    if (GetPlayerTwo() != NULL) {
        GetPlayerTwo()->SetPosition(16, 0);
        GetPlayerTwo()->SetAlive(false);
        GetPlayerTwo()->SetDirection(DIR_UP);
        GetPlayerTwo()->SetState(PLAYER_STOP);
        GetPlayerTwo()->SetXVelocity(0);
        GetPlayerTwo()->SetYVelocity(0);
    }
}

void Game::SetupPlayerTwo() {
    m_PlayerTwo = new Player;
    GetPlayerTwo()->Init(16, 0, 2);
}

void Game::DeletePlayerTwo() {
    if (m_PlayerTwo != NULL) {
        delete m_PlayerTwo;
        m_PlayerTwo = NULL;
    }
}

void Game::Init() {
    srand(static_cast<int>(time(NULL)));

    GetScreen()->Set(640, 480, false);
    GetScreen()->Init();
    GetAudio()->Init();
    GetRenderer()->Init();
    GetPlayer()->Init(8, 0, 1);
    GetMenu()->Init();

}

int Game::GameLoop()
{
    gameScreenOffsetX = 32;
    gameScreenOffsetY = 16;
    gameState = GS_MENU;
    deltaTime = 0.0;

    cout << endl;

    double time_step = 1.0 / 60;
    double accumulator = 0.0;
    double max_time = 1.0;

    ticksOld = SDL_GetTicks();

    while (gameIsRunning) {
        switch (gameState) {
        case GS_MENU:
            GetMenu()->ProcessEvents();        break;
        case GS_STAGESELECT:
            GetGameTimer()->ProcessEvents();   break;
        case GS_GAMEPLAY:
        case GS_EDITOR:
            ProcessEvents();                   break;
        }

        // Frame Processing
        ticksNew = SDL_GetTicks();
        deltaTime = double((ticksNew - ticksOld)) / 1000.0;
        ticksOld = ticksNew;
        if (deltaTime < 0)
            deltaTime = 0;

        accumulator += deltaTime;
        if (accumulator < 0)
            accumulator = 0;
        else if (accumulator > max_time)
            accumulator = max_time;


        // FPS Counter
        FPSTime += deltaTime;
        ++FPSCount;
        if (FPSTime >= 1.0) {
            int fps_num = static_cast<int>(FPSCount / FPSTime);

            SDL_SetWindowTitle(m_Screen->currentWindow, (GetScreen()->WindowTitle() + " | FPS: " + to_string(fps_num)).c_str());
            // cout << "FPS: " << fps_num << endl;
            FPSCount = 0;
            FPSTime = 0;
        }

        // Audio
        if ((GetPlayer()->OnMove() && GetPlayerTwo() == NULL) || (GetPlayer()->OnMove() && GetPlayerTwo()->OnMove())) {
            GetAudio()->PlayChunk(SOUND_ONMOVE);
        }
        else {
            GetAudio()->StopChunk(SOUND_ONMOVE);
        }

        while (accumulator > time_step) {
            if (gameState == GS_GAMEPLAY) {
                GetGameTimer()->Update(time_step);
                if (GetPlayer()->Alive())
                    GetPlayer()->Update(time_step);
                if (GetPlayerTwo() != NULL && GetPlayerTwo()->Alive())
                    GetPlayerTwo()->Update(time_step);
                if (GetItems()->GetCurrentItem().destroyed == false)
                    GetItems()->Update(time_step);
                GetEnemies()->Update(time_step);
                GetBullets()->Update(time_step);
                GetAnimation()->Update(time_step);
            }
            else if (gameState == GS_STAGESELECT) {
                GetGameTimer()->Update(time_step);
            }
            accumulator -= time_step;
        }

        // Rendering
        GetRenderer()->StartRendering();
        switch (gameState) {
        case GS_MENU:
            GetMenu()->DrawMenu();
            break;
        case GS_STAGESELECT:
            GetGameTimer()->DrawStageSelect();
            break;
        case GS_GAMEPLAY:
            GetLevel()->DrawBackground();
            GetLevel()->DrawMap(0);

            if (GetPlayer()->Alive())
                GetPlayer()->Draw();
            if (GetPlayerTwo() != NULL && GetPlayerTwo()->Alive())
                GetPlayerTwo()->Draw();
            GetEnemies()->Draw();
            GetBullets()->Draw();
            GetAnimation()->Draw();

            GetLevel()->DrawMap(1);

            if (GetItems()->GetCurrentItem().destroyed == false && GetItems()->GetCurrentItem().isVisible)
                GetItems()->Draw();

            GetUI()->DrawGameplayStats();
            break;
        case GS_EDITOR:
            GetLevel()->DrawBackground();
            GetLevel()->DrawMap(0);

            GetEditor()->DrawEditor();

            GetLevel()->DrawMap(1);
            break;
        }
        GetRenderer()->StopRendering(m_Screen->currentWindow);

    }
    return 0;
}

void Game::SetGameState(GameStateType state) {
    if (gameState == GS_GAMEPLAY) {
        GetBullets()->DestroyAllBullets();
        GetEnemies()->DestroyAllEnemies();
        GetAudio()->StopAll();
    }
    if (state == GS_GAMEPLAY)
        StartGameplay();
    else if (state == GS_MENU) {
        SetGameLost(false);
        GetPlayer()->SetPlayerLevel(0);
        if (GetPlayerTwo() != NULL)
            GetPlayerTwo()->SetPlayerLevel(0);
        GetGameTimer()->SetSelectedLevel(1);
        GetLevel()->SetLevelNum(0);
    }
    gameState = state;
}

void Game::ProcessEvents() {
    if (isEnd()) return;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            gameIsRunning = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                SetGameState(GS_MENU);
            }
            else if (event.key.keysym.sym == SDLK_UP && GetPlayer()->Alive() && !GameLost()) {
                GetPlayer()->SetDirection(DIR_UP);
                GetPlayer()->Drive();
            }
            else if (event.key.keysym.sym == SDLK_LEFT && GetPlayer()->Alive() && !GameLost()) {
                GetPlayer()->SetDirection(DIR_LEFT);
                GetPlayer()->Drive();
            }
            else if (event.key.keysym.sym == SDLK_RIGHT && GetPlayer()->Alive() && !GameLost()) {
                GetPlayer()->SetDirection(DIR_RIGHT);
                GetPlayer()->Drive();
            }
            else if (event.key.keysym.sym == SDLK_DOWN && GetPlayer()->Alive() && !GameLost()) {
                GetPlayer()->SetDirection(DIR_DOWN);
                GetPlayer()->Drive();
            }
            else if (event.key.keysym.sym == SDLK_SPACE && GetPlayer()->Alive() && !GameLost()) {
                GetPlayer()->Shoot();
            }
            else if (event.key.keysym.sym == SDLK_w && GetPlayerTwo() != NULL && GetPlayerTwo()->Alive() && !GameLost()) {
                GetPlayerTwo()->SetDirection(DIR_UP);
                GetPlayerTwo()->Drive();
            }
            else if (event.key.keysym.sym == SDLK_a && GetPlayerTwo() != NULL && GetPlayerTwo()->Alive() && !GameLost()) {
                GetPlayerTwo()->SetDirection(DIR_LEFT);
                GetPlayerTwo()->Drive();
            }
            else if (event.key.keysym.sym == SDLK_d && GetPlayerTwo() != NULL && GetPlayerTwo()->Alive() && !GameLost()) {
                GetPlayerTwo()->SetDirection(DIR_RIGHT);
                GetPlayerTwo()->Drive();
            }
            else if (event.key.keysym.sym == SDLK_s && GetPlayerTwo() != NULL && GetPlayerTwo()->Alive() && !GameLost()) {
                GetPlayerTwo()->SetDirection(DIR_DOWN);
                GetPlayerTwo()->Drive();
            }
            else if (event.key.keysym.sym == SDLK_LCTRL && GetPlayerTwo() != NULL && GetPlayerTwo()->Alive() && !GameLost()) {
                GetPlayerTwo()->Shoot();
            }
            else if (event.key.keysym.sym == SDLK_F5) {
                if (gameState == GS_EDITOR)
                    GetLevel()->SaveMap();
            }
        }
        else if (event.type == SDL_KEYUP) {
            if (event.key.keysym.sym == SDLK_UP) {
                GetPlayer()->Stop(DIR_UP);
            }
            else if (event.key.keysym.sym == SDLK_LEFT) {
                GetPlayer()->Stop(DIR_LEFT);
            }
            else if (event.key.keysym.sym == SDLK_RIGHT) {
                GetPlayer()->Stop(DIR_RIGHT);
            }
            else if (event.key.keysym.sym == SDLK_DOWN) {
                GetPlayer()->Stop(DIR_DOWN);
            }
            else if (event.key.keysym.sym == SDLK_w && GetPlayerTwo() != NULL) {
                GetPlayerTwo()->Stop(DIR_UP);
            }
            else if (event.key.keysym.sym == SDLK_a && GetPlayerTwo() != NULL) {
                GetPlayerTwo()->Stop(DIR_LEFT);
            }
            else if (event.key.keysym.sym == SDLK_d && GetPlayerTwo() != NULL) {
                GetPlayerTwo()->Stop(DIR_RIGHT);
            }
            else if (event.key.keysym.sym == SDLK_s && GetPlayerTwo() != NULL) {
                GetPlayerTwo()->Stop(DIR_DOWN);
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN && gameState == GS_EDITOR) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                if (gameState == GS_EDITOR) {
                    GetEditor()->LeftButtonDownHandle(event.button.x, event.button.y);
                }
            }
            else if (event.button.button == SDL_BUTTON_RIGHT) {
                if (gameState == GS_EDITOR) {
                    GetEditor()->RightButtonDownHandle(event.button.x, event.button.y);
                }
            }
        }
    }
}
