#include <iostream>
#include "GameTimer.h"
#include "Game.h"
using namespace std;

void GameTimer::Init() {
    enemySpawnTime = 3;
    currentEnemyTime = 2.5;
    playerSpawnTime = 1;
    currentPlayerTime = 0;
    nextmapChangeTime = 3;
    currentNextmapTime = 0;
    currentLevelProtectTime = 0;
    levelProtectTime = 15.0;
    currentPlayerOneRssurectionTime = 0;
    currentPlayerTwoRssurectionTime = 0;
    currentGameLostTime = 0;
    gameLostTime = 6.0;

    levelProtectState = false;

    maxEnemiesInScreen = 4;
    playerRessurectionTime = 0.5;
}

void GameTimer::SetLevelProtect(bool protect) {
    levelProtectState = true;

    BrickType field;
    if (protect)
        field = BT_WHITE;
    else
        field = BT_BRICK;

    Game::instance().GetLevel()->SetMapBrick(11, 0, field);
    Game::instance().GetLevel()->SetMapBrick(11, 1, field);
    Game::instance().GetLevel()->SetMapBrick(11, 2, field);
    Game::instance().GetLevel()->SetMapBrick(12, 2, field);
    Game::instance().GetLevel()->SetMapBrick(13, 2, field);
    Game::instance().GetLevel()->SetMapBrick(14, 0, field);
    Game::instance().GetLevel()->SetMapBrick(14, 1, field);
    Game::instance().GetLevel()->SetMapBrick(14, 2, field);
}

void GameTimer::DrawStageSelect() {
    Game::instance().GetUI()->RenderText("STAGE " + to_string(showedLevel), 230, 230, COLOR_BLACK);
}

void GameTimer::ProcessEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            Game::instance().StopGame();
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                Game::instance().SetGameState(GS_MENU);
            }
            else if (Game::instance().GetLevel()->LevelNum() == 0) {
                if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_RIGHT) {
                    int i = selectedLevel + 1;
                    if (i > Game::instance().GetLevel()->NumOfMaps())
                        i = Game::instance().GetLevel()->NumOfMaps();
                    selectedLevel = showedLevel = i;
                }
                else if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_LEFT) {
                    int i = selectedLevel - 1;
                    if (i < 0)
                        i = 0;
                    selectedLevel = showedLevel = i;
                }
                else if (event.key.keysym.sym == SDLK_RETURN) {
                    ChooseLevel();
                }
            }
        }
    }
}

void GameTimer::ChooseLevel() {
    if (selectedLevel > Game::instance().GetLevel()->NumOfMaps())
        selectedLevel = 1;

    string level = "data/map/" + to_string(selectedLevel);

    if (selectedLevel == 0) {
        level = "data/map/temp";
    }

    if (Game::instance().GetLevel()->LoadMap(level) == false) {
        Game::instance().SetGameState(GS_MENU);
        return;
    }

    Game::instance().GetAudio()->PlayChunk(SOUND_GAMESTART);

    Game::instance().GetBullets()->DestroyAllBullets();
    Game::instance().GetAnimation()->DestroyAllAnimation();
    Game::instance().GetEnemies()->DestroyAllEnemies();
    Game::instance().GetEnemies()->UnPause();

    Game::instance().GetPlayer()->Born();
    if (Game::instance().GetPlayerTwo() != NULL) {
        Game::instance().GetPlayerTwo()->Born();
    }

    Game::instance().GetItems()->DestroyItem();
    Game::instance().StartGameplay();
    Game::instance().SetGameState(GS_GAMEPLAY);
    Init();
}

void GameTimer::Update(double dt) {
    static int TailSize = Game::instance().TailSize();
    if (Game::instance().GameState() == GS_GAMEPLAY) {
        // try to add enemy
        if (Game::instance().GetEnemies()->AliveEnemies() < maxEnemiesInScreen && Game::instance().GetEnemies()->NumOfEnemies() < 20) {
            currentEnemyTime += dt;
            if (currentEnemyTime >= enemySpawnTime) {
                Game::instance().GetEnemies()->CreateEnemy();
                currentEnemyTime = 0;
                enemySpawnAnimation = false;
            }
            else if (currentEnemyTime >= enemySpawnTime - 0.5 && enemySpawnAnimation == false) {
                int x = Game::instance().GetEnemies()->NextSpawnX();
                Game::instance().GetAnimation()->CreateAnimation(TailSize * x, TailSize * 24, ANIMATION_SPAWN);
                enemySpawnAnimation = true;
            }
        }
        else if (Game::instance().GetEnemies()->NumOfEnemies() == 20 && Game::instance().GetEnemies()->AliveEnemies() == 0 && Game::instance().GameLost() == false) {
            currentNextmapTime += dt;
            if (currentNextmapTime >= nextmapChangeTime) {
                // change game state
                Game::instance().SetGameState(GS_STAGESELECT);
                ++selectedLevel;
                ++showedLevel;
                currentNextmapTime = 0.0;
            }
        }

        if (Game::instance().GetPlayer()->Lifes() < 0 && (Game::instance().GetPlayerTwo() == NULL || Game::instance().GetPlayerTwo()->Lifes() < 0)) {
            Game::instance().SetGameLost(true);
        }

        if (Game::instance().GameLost()) {
            if (currentGameLostTime == 0.0)
                Game::instance().GetAudio()->PlayChunk(SOUND_GAMEOVER);

            currentGameLostTime += dt;
            if (currentGameLostTime >= gameLostTime) {
                Game::instance().SetGameState(GS_MENU);
                currentGameLostTime = 0.0;
            }
        }

        // player rebirth
        if (Game::instance().GetPlayer()->Alive() == false && Game::instance().GetPlayer()->Lifes() >= 0) {
            if (currentPlayerOneRssurectionTime == 0.0)
                Game::instance().GetAnimation()->CreateAnimation(TailSize * 8, 0, ANIMATION_SPAWN);

            currentPlayerOneRssurectionTime += dt;
            if (currentPlayerOneRssurectionTime >= playerRessurectionTime) {
                Game::instance().GetPlayer()->Born();
                currentPlayerOneRssurectionTime = 0;
            }
        }
        if (Game::instance().GetPlayerTwo() != NULL) {
            if (Game::instance().GetPlayerTwo()->Alive() == false && Game::instance().GetPlayerTwo()->Lifes() >= 0) {
                if (currentPlayerTwoRssurectionTime == 0.0)
                    Game::instance().GetAnimation()->CreateAnimation(TailSize * 16, 0, ANIMATION_SPAWN);

                currentPlayerTwoRssurectionTime += dt;
                if (currentPlayerTwoRssurectionTime >= playerRessurectionTime) {
                    Game::instance().GetPlayerTwo()->Born();
                    currentPlayerTwoRssurectionTime = 0;
                }
            }
        }

        // protection state
        if (levelProtectState == true) {
            currentLevelProtectTime += dt;
            if (currentLevelProtectTime >= levelProtectTime) {
                levelProtectState = false;
                SetLevelProtect(false);
                currentLevelProtectTime = 0.0;
            }

            if (currentLevelProtectTime >= levelProtectTime - 0.5)
                SetLevelProtect(true);
            else if (currentLevelProtectTime >= levelProtectTime - 1)
                SetLevelProtect(false);
            else if (currentLevelProtectTime >= levelProtectTime - 1.5)
                SetLevelProtect(true);
            else if (currentLevelProtectTime >= levelProtectTime - 2)
                SetLevelProtect(false);
        }
    }
    else if (Game::instance().GameState() == GS_STAGESELECT) {
        if (Game::instance().GetLevel()->LevelNum() != 0) {
            currentNextmapTime += dt;
            if (currentNextmapTime >= nextmapChangeTime) {
                currentNextmapTime = 0.0;
                ChooseLevel();
            }
        }
    }
}
