#include <iostream>
#include "Game.h"
#include "Menu.h"
#include "Renderer.h"

using namespace std;

void Menu::Init() {
    backgroundSprite = Game::instance().GetSprites()->Get("lvl_background");
    logoSprite = Game::instance().GetSprites()->Get("menu_logo");
    menuOptionsSprite = Game::instance().GetSprites()->Get("menu_options");
    pointerSprite = Game::instance().GetSprites()->Get("player_one_right_0");
}

void Menu::DrawMenu() {
    Game::instance().GetRenderer()->DrawSprite(backgroundSprite, 0, -32, -16, 640, 480);
    Game::instance().GetRenderer()->DrawSprite(logoSprite, 0, 110, 260, logoSprite.width, logoSprite.height);
    Game::instance().GetRenderer()->DrawSprite(menuOptionsSprite, 0, 232, 150, menuOptionsSprite.width, menuOptionsSprite.height);

    switch (selectedItem) {
    case 1:
        Game::instance().GetRenderer()->DrawSprite(pointerSprite, 0, 190, 205, pointerSprite.width, pointerSprite.height);
        break;
    case 2:
        Game::instance().GetRenderer()->DrawSprite(pointerSprite, 0, 190, 173, pointerSprite.width, pointerSprite.height);
        break;
    case 3:
        Game::instance().GetRenderer()->DrawSprite(pointerSprite, 0, 190, 140, pointerSprite.width, pointerSprite.height);
        break;
    }

    Game::instance().GetUI()->RenderText("REMAKE-BY-LYM", 200, 0);
}

void Menu::ChooseItem() {
    switch (selectedItem) {
    case 1:
        Game::instance().SetGameState(GS_STAGESELECT);
        Game::instance().GetPlayer()->SetLifes(2);
        Game::instance().GetPlayer()->Born();
        Game::instance().DeletePlayerTwo();
        break;
    case 2:
        Game::instance().SetGameState(GS_STAGESELECT);
        Game::instance().SetupPlayerTwo();
        Game::instance().GetPlayer()->SetLifes(2);
        Game::instance().GetPlayerTwo()->SetLifes(2);
        Game::instance().GetPlayer()->Born();
        Game::instance().GetPlayerTwo()->Born();
        break;
    case 3:
        Game::instance().SetGameState(GS_EDITOR);
        Game::instance().GetLevel()->LoadMap("data/map/temp");
        break;
    }
}

void Menu::ProcessEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            Game::instance().StopGame();
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                Game::instance().StopGame();
            }
            else if (event.key.keysym.sym == SDLK_UP) {
                int i = SelectedItem() - 1;
                if (i == 0) 
                    i = 3;
                SelectItem(i);
            }
            else if (event.key.keysym.sym == SDLK_DOWN) {
                int i = SelectedItem() + 1;
                if (i == 4) 
                    i = 1;
                SelectItem(i);
            }
            else if (event.key.keysym.sym == SDLK_RETURN) {
                ChooseItem();
            }
        }
    }
}
