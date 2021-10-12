#include "UI.h"
#include "Game.h"
#include <algorithm>

using namespace std;

void UI::DrawGameplayStats() {
    static int x = 27;
    static int y = 25;
    static int TailSize = Game::instance().TailSize();
    static SpriteData EnemyTank = Game::instance().GetSprites()->Get("enemy_stat");
    static SpriteData PlayerTank = Game::instance().GetSprites()->Get("player_stat");
    static SpriteData Flag = Game::instance().GetSprites()->Get("flag");

    for (int i = 0; i < 20 - Game::instance().GetEnemies()->NumOfEnemies(); ++i) {
        if (i % 2 == 0)
            Game::instance().GetRenderer()->DrawSprite(EnemyTank, 0, x * TailSize, (y - (i / 2)) * TailSize, EnemyTank.width, EnemyTank.height);
        else
            Game::instance().GetRenderer()->DrawSprite(EnemyTank, 0, (x + 1) * TailSize, (y - (i / 2)) * TailSize, EnemyTank.width, EnemyTank.height);
    }

    // player one
    string lifes = "0";
    if (Game::instance().GetPlayer()->Lifes() > 0)
        lifes = std::to_string(Game::instance().GetPlayer()->Lifes());

    RenderText("IP", x * TailSize, 10 * TailSize, COLOR_BLACK);
    Game::instance().GetRenderer()->DrawSprite(PlayerTank, 0, x * TailSize, 9 * TailSize, PlayerTank.width, PlayerTank.height);
    RenderText(lifes, (x + 1) * TailSize, 9 * TailSize, COLOR_BLACK);

    // player two
    if (Game::instance().GetPlayerTwo() != NULL) {
        lifes = "0";
        if (Game::instance().GetPlayerTwo()->Lifes() > 0)
            lifes = std::to_string(Game::instance().GetPlayerTwo()->Lifes());

        RenderText("IIP", x * TailSize, 7 * TailSize, COLOR_BLACK);
        Game::instance().GetRenderer()->DrawSprite(PlayerTank, 0, x * TailSize, 6 * TailSize, PlayerTank.width, PlayerTank.height);
        RenderText(lifes, (x + 1) * TailSize, 6 * TailSize, COLOR_BLACK);
    }

    Game::instance().GetRenderer()->DrawSprite(Flag, 0, x * TailSize, 3 * TailSize, Flag.width, Flag.height);
    RenderText(to_string(Game::instance().GetGameTimer()->ShowedLevel()), (x + 1) * TailSize, 2 * TailSize, COLOR_BLACK);

    if (Game::instance().GameLost()) {
        RenderText("GAME", 11 * TailSize, 14 * TailSize, COLOR_RED);
        RenderText("OVER", 11 * TailSize, 13 * TailSize, COLOR_RED);
    }
}

void UI::RenderText(const string& text, double x, double y, ColorType color) {
    static SpriteData chars = Game::instance().GetSprites()->Get("chars");
    static SpriteData signs = Game::instance().GetSprites()->Get("signs");
    static int pixels_per_one_sign = max(chars.width, signs.width);

    char temp;
    for (std::size_t i = 0; i < text.length(); ++i) {
        temp = text[i];
        if (temp >= 'A' && temp <= 'Z') {
            Game::instance().GetRenderer()->DrawSprite(chars, temp - 'A', x + i*pixels_per_one_sign, y, chars.width, chars.height, color);
        }
        else if (temp >= '0' && temp <= '9') {
            Game::instance().GetRenderer()->DrawSprite(signs, temp - '0', x + i*pixels_per_one_sign, y, chars.width, chars.height, color);
        }
    }
}
