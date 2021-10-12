#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>
#include <fstream>
#include "Types.h"

using namespace std;

struct BrickState {
    BrickState() : top_left(false), top_right(false), bottom_left(false), bottom_right(false) { }
    void Reset() {
        top_left = top_right = bottom_left = bottom_right = true;
    }

    bool top_left;
    bool top_right;
    bool bottom_left;
    bool bottom_right;
};

class Map {
public:
    Map();

    bool LoadMap(const string fileName);
    void DrawMap(bool up);
    void DrawBackground();
    void DestroyTile(int x1, int y1, int x2, int y2, int power, Direction dir);

    void DumpMap(); // for debugging

    void SaveMap();

    int LevelNum() { return currentMapIndex; }
    int MapWidth() { return mapWidth; }
    int MapHeight() { return mapHeight; }
    int NumOfMaps() { return numOfMaps; }

    void SetMapBrick(int x, int y, BrickType type) { fieldData[y][x] = type; }
    void SetLevelNum(int num) { currentMapIndex = num; }
    BrickType MapField(int x, int y) { return fieldData[y][x]; }

    BrickState BrickField(int x, int y) { return bricks[y][x]; }
    void SetBrickTopLeft(int x, int y, bool value) { bricks[y][x].top_left = value; }
    void SetBrickTopRight(int x, int y, bool value) { bricks[y][x].top_right = value; }
    void SetBrickBottomLeft(int x, int y, bool value) { bricks[y][x].bottom_left = value; }
    void SetBrickBottomRight(int x, int y, bool value) { bricks[y][x].bottom_right = value; }
    void ResetBrick(int x, int y) { bricks[y][x].Reset(); }

private:
    int mapWidth;
    int mapHeight;

    int numOfMaps;

    vector<vector<BrickState>> bricks;
    vector<vector<BrickType>> fieldData;
    int currentMapIndex;
};

#endif // LEVEL_H
