#include "MapEditor.h"
#include "Game.h"
#include <iostream>

using namespace std;

void Editor::DrawEditor() {
    static SpriteData sprEditor = Game::instance().GetSprites()->Get("editor");
    Game::instance().GetRenderer()->DrawSprite(sprEditor, 0, 450, 194, sprEditor.width, sprEditor.height);
}

void Editor::LeftButtonDownHandle(int x, int y) {
    int x_off = x - Game::instance().GameScreenOffsetX();
    int y_off = Game::instance().GetScreen()->WindowHeight() - y - Game::instance().GameScreenOffsetY();
    static int tail_size = Game::instance().TailSize();

    if (x_off >= 0 && x_off < 416 && y_off >= 0 && y_off < 416) {
        Game::instance().GetLevel()->SetMapBrick(x_off / tail_size, y_off / tail_size, selectedField);
    }
    else if (x_off >= 459 && x_off <= 491) {
        if (y_off >= 354 && y_off <= 386) {
            selectedField = BT_BRICK;
        }
        else if (y_off >= 315 && y_off <= 347) {
            selectedField = BT_WHITE;
        }
        else if (y_off >= 278 && y_off <= 310) {
            selectedField = BT_SLIDE;
        }
        else if (y_off >= 242 && y_off <= 274) {
            selectedField = BT_BUSH;
        }
        else if (y_off >= 199 && y_off <= 231) {
            selectedField = BT_RIVER;
        }
    }
}

void Editor::RightButtonDownHandle(int x, int y) {
    int x_off = x - Game::instance().GameScreenOffsetX();
    int y_off = Game::instance().GetScreen()->WindowHeight() - y - Game::instance().GameScreenOffsetY();
    static int tail_size = Game::instance().TailSize();

    if (x_off >= 0 && x_off < 416 && y_off >= 0 && y_off < 416) {
        Game::instance().GetLevel()->SetMapBrick(x_off / tail_size, y_off / tail_size, BT_NONE);
    }
}
