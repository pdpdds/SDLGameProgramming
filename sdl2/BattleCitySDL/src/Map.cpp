#include "Game.h"
#include "Player.h"
#include "Map.h"
#include <iostream>

using namespace std;

Map::Map() {
    mapWidth = 26;
    mapHeight = 26;

    fieldData.resize(mapWidth);
    for (int i = 0; i < mapHeight; ++i)
        fieldData.at(i).resize(mapWidth);

    currentMapIndex = 0;
    numOfMaps = 20;
}

bool Map::LoadMap(const string fileName) {

    bricks.clear();
    bricks.resize(mapWidth);
    for (int i = 0; i < mapHeight; ++i)
        bricks[i].resize(mapWidth);

    ifstream fin(fileName.c_str());

    if (!fin) {
        cout << "Unable to load " << fileName << endl;
        return false;
    }

    fin >> currentMapIndex;
    int tmp;
    for (int y = mapHeight - 1; y >= 0; --y) {
        for (int x = 0; x < mapWidth; ++x) {
            fin >> tmp;

            if (BrickType(tmp) == BT_BRICK)
                ResetBrick(x, y);

            fieldData[y][x] = (BrickType)tmp;
        }
    }

    fin >> tmp;
    if (tmp != -1) {
        Game::instance().GetEnemies()->SetEnemyType(0, EnemyType(tmp));
        for (int i = 1; i < 20; ++i) {
            fin >> tmp;
            Game::instance().GetEnemies()->SetEnemyType(i, EnemyType(tmp));
        }
    }
    else {
        for (int i = 0; i < 20; ++i) {
            tmp = rand() % 4;
            if (i == 3 || i == 10 || i == 17)
                tmp += 4;
            Game::instance().GetEnemies()->SetEnemyType(i, EnemyType(tmp));
        }
    }

    fin.close();

    SetMapBrick(0, 24, BT_NONE);
    SetMapBrick(0, 25, BT_NONE);
    SetMapBrick(1, 24, BT_NONE);
    SetMapBrick(1, 25, BT_NONE);

    SetMapBrick(12, 24, BT_NONE);
    SetMapBrick(12, 25, BT_NONE);
    SetMapBrick(13, 24, BT_NONE);
    SetMapBrick(13, 25, BT_NONE);

    SetMapBrick(24, 24, BT_NONE);
    SetMapBrick(24, 25, BT_NONE);
    SetMapBrick(25, 24, BT_NONE);
    SetMapBrick(25, 25, BT_NONE);

    SetMapBrick(12, 0, BT_EAGLE);
    SetMapBrick(12, 1, BT_EAGLE);
    SetMapBrick(13, 0, BT_EAGLE);
    SetMapBrick(13, 1, BT_EAGLE);

    cout << "- Load file '" << fileName << "'\n";
    return true;
}

void Map::DrawBackground() {
    static int tail_size = Game::instance().TailSize();
    Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get("lvl_background"), 0, 0, 0, 26 * tail_size, 26 * tail_size);
}

// up = 1: draw bush
void Map::DrawMap(bool up) {
    static int tail_size = Game::instance().TailSize();
    bool eagle_drawn = false;

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            switch (fieldData[y][x]) {
            case BT_BRICK_DAMAGED:
            {
                if (!up) {
                    BrickState b = BrickField(x, y);
                    int frame = 0;
                    if (b.top_left && b.top_right && b.bottom_left && b.bottom_right) {
                        frame = 0;
                    }
                    else if (b.top_left && b.top_right && b.bottom_left && !b.bottom_right) {
                        frame = 1;
                    }
                    else if (b.top_left && b.top_right && !b.bottom_left && b.bottom_right) {
                        frame = 2;
                    }
                    else if (!b.top_left && b.top_right && b.bottom_left && b.bottom_right) {
                        frame = 3;
                    }
                    else if (b.top_left && !b.top_right && b.bottom_left && b.bottom_right) {
                        frame = 4;
                    }
                    else if (b.top_left && !b.top_right && b.bottom_left && !b.bottom_right) {
                        frame = 5;
                    }
                    else if (!b.top_left && !b.top_right && b.bottom_left && b.bottom_right) {
                        frame = 6;
                    }
                    else if (!b.top_left && b.top_right && !b.bottom_left && b.bottom_right) {
                        frame = 7;
                    }
                    else if (b.top_left && b.top_right && !b.bottom_left && !b.bottom_right) {
                        frame = 8;
                    }
                    else if (b.top_left && !b.top_right && !b.bottom_left && !b.bottom_right) {
                        frame = 9;
                    }
                    else if (!b.top_left && b.top_right && !b.bottom_left && !b.bottom_right) {
                        frame = 10;
                    }
                    else if (!b.top_left && !b.top_right && !b.bottom_left && b.bottom_right) {
                        frame = 11;
                    }
                    else if (!b.top_left && !b.top_right && b.bottom_left && !b.bottom_right) {
                        frame = 12;
                    }
                    else if (!b.top_left && !b.top_right && !b.bottom_left && !b.bottom_right) {
                        frame = 13;
                    }
                    Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get("lvl_brick"), frame, x * tail_size, y * tail_size, tail_size, tail_size); break;
                }
            }
            case BT_BRICK:
                if (!up)
                    Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get("lvl_brick"), 0, x * tail_size,
                        y * tail_size, tail_size, tail_size);
                break;
            case BT_WHITE:
                if (!up)
                    Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get("lvl_white"), 0, x * tail_size,
                        y * tail_size, tail_size, tail_size);
                break;
            case BT_SLIDE:
                if (!up)
                    Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get("lvl_slide"), 0, x * tail_size,
                        y * tail_size, tail_size, tail_size);
                break;
            case BT_BUSH:
                if (up)
                    Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get("lvl_bush"), 0, x * tail_size,
                        y * tail_size, tail_size, tail_size);
                break;
            case BT_RIVER:
                if (!up)
                    Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get("lvl_water"), 0, x * tail_size,
                        y * tail_size, tail_size, tail_size);
                break;
            case BT_EAGLE:
                if (!eagle_drawn && !up) {
                    if (!Game::instance().GameLost()) {
                        Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get("lvl_eagle"), 0, x * tail_size,
                            y * tail_size, 32, 32); eagle_drawn = true;
                    }
                    else {
                        Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get("lvl_eagle"), 1, x * tail_size,
                            y * tail_size, 32, 32); eagle_drawn = true;
                    }

                }
                break;
            }
        }
    }
}

void Map::DumpMap() {
    for (int y = mapHeight - 1; y >= 0; --y) {
        for (int x = 0; x < mapWidth; ++x) {
            cout << int(fieldData[y][x]) << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void Map::SaveMap() {
    ofstream fout("data/map/temp");
    if (!fout) {
        cout << "- Save map fail (Unable to open data/map/temp)" << endl;
        fout.close();
        return;
    }
    fout << "0" << endl;
    for (int y = mapHeight - 1; y >= 0; --y) {
        for (int x = 0; x < mapWidth; ++x) {
            fout << int(fieldData[y][x]) << " ";
        }
        fout << endl;
    }

    fout.close();
    cout << "Save to data/map/temp" << endl;
}

void Map::DestroyTile(int x1, int y1, int x2, int y2, int bulletPower, Direction dir) {
    if (bulletPower == 1) {
        if (x1 < 0 || x1 > 25 || y1 < 0 || y1 > 25 || x2 < 0 || x2 > 25 || y2 < 0 || y2 > 25)
            return;
        else {
            bool change_1 = false, change_2 = false;
            if (MapField(x1, y1) == BT_BRICK || MapField(x1, y1) == BT_BRICK_DAMAGED) {
                SetMapBrick(x1, y1, BT_BRICK_DAMAGED);
                change_1 = true;
            }

            if (MapField(x2, y2) == BT_BRICK || MapField(x2, y2) == BT_BRICK_DAMAGED) {
                SetMapBrick(x2, y2, BT_BRICK_DAMAGED);
                change_2 = true;
            }

            switch (dir) {
            case DIR_UP:
                if (BrickField(x1, y1).bottom_left || BrickField(x1, y1).bottom_right || BrickField(x2, y2).bottom_left || BrickField(x2, y2).bottom_right) {
                    if (change_1) { SetBrickBottomLeft(x1, y1, false); SetBrickBottomRight(x1, y1, false); }
                    if (change_2) { SetBrickBottomLeft(x2, y2, false); SetBrickBottomRight(x2, y2, false); }
                }
                else {
                    if (change_1) { SetBrickTopLeft(x1, y1, false); SetBrickTopRight(x1, y1, false); }
                    if (change_2) { SetBrickTopLeft(x2, y2, false); SetBrickTopRight(x2, y2, false); }
                } break;
            case DIR_DOWN:
                if (BrickField(x1, y1).top_left || BrickField(x1, y1).top_right || BrickField(x2, y2).top_left || BrickField(x2, y2).top_right) {
                    if (change_1) { SetBrickTopLeft(x1, y1, false); SetBrickTopRight(x1, y1, false); }
                    if (change_2) { SetBrickTopLeft(x2, y2, false); SetBrickTopRight(x2, y2, false); }
                }
                else {
                    if (change_1) { SetBrickBottomLeft(x1, y1, false); SetBrickBottomRight(x1, y1, false); }
                    if (change_2) { SetBrickBottomLeft(x2, y2, false); SetBrickBottomRight(x2, y2, false); }
                } break;
            case DIR_LEFT:
                if (BrickField(x1, y1).top_right || BrickField(x1, y1).bottom_right || BrickField(x2, y2).top_right || BrickField(x2, y2).bottom_right) {
                    if (change_1) { SetBrickTopRight(x1, y1, false); SetBrickBottomRight(x1, y1, false); }
                    if (change_2) { SetBrickTopRight(x2, y2, false); SetBrickBottomRight(x2, y2, false); }
                }
                else {
                    if (change_1) { SetBrickTopLeft(x1, y1, false); SetBrickBottomLeft(x1, y1, false); }
                    if (change_2) { SetBrickTopLeft(x2, y2, false); SetBrickBottomLeft(x2, y2, false); }
                } break;
            case DIR_RIGHT:
                if (BrickField(x1, y1).top_left || BrickField(x1, y1).bottom_left || BrickField(x2, y2).top_left || BrickField(x2, y2).bottom_left) {
                    if (change_1) { SetBrickTopLeft(x1, y1, false); SetBrickBottomLeft(x1, y1, false); }
                    if (change_2) { SetBrickTopLeft(x2, y2, false); SetBrickBottomLeft(x2, y2, false); }
                }
                else {
                    if (change_1) { SetBrickTopRight(x1, y1, false); SetBrickBottomRight(x1, y1, false); }
                    if (change_2) { SetBrickTopRight(x2, y2, false); SetBrickBottomRight(x2, y2, false); }
                } break;
            }

            if (MapField(x1, y1) == BT_BRICK_DAMAGED && !BrickField(x1, y1).top_left && !BrickField(x1, y1).top_right && !BrickField(x1, y1).bottom_left && !BrickField(x1, y1).bottom_right)
                SetMapBrick(x1, y1, BT_NONE);
            if (MapField(x2, y2) == BT_BRICK_DAMAGED && !BrickField(x2, y2).top_left && !BrickField(x2, y2).top_right && !BrickField(x2, y2).bottom_left && !BrickField(x2, y2).bottom_right)
                SetMapBrick(x2, y2, BT_NONE);

        }
    }
    else if (bulletPower == 2) {
        if (x1 >= 0 && x1 <= 25 && y1 >= 0 && y1 <= 25) {
            if (MapField(x1, y1) == BT_BRICK || MapField(x1, y1) == BT_BRICK_DAMAGED || MapField(x1, y1) == BT_WHITE)
                SetMapBrick(x1, y1, BT_NONE);

        }
        if (x2 >= 0 && x2 <= 25 && y2 >= 0 && y2 <= 25) {
            if (MapField(x2, y2) == BT_BRICK || MapField(x2, y2) == BT_BRICK_DAMAGED || MapField(x2, y2) == BT_WHITE)
                SetMapBrick(x2, y2, BT_NONE);
        }
    }
}
