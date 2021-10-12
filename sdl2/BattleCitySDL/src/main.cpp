#include "Game.h"

int main(int argc, char *argv[]) {

    Game::instance().Init();

    return Game::instance().GameLoop();

}
