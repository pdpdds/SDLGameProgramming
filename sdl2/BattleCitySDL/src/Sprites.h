#ifndef SPRITES_H
#define SPRITES_H

#include <string>
#include <map>
using namespace std;

struct SpriteData {
    SpriteData() { }
    SpriteData(int frame_count, double frame_duration, int left, int bottom, int width, int height, bool loop, string atlas) :
        frameCount(frame_count), frameDuration(frame_duration), left(left), bottom(bottom), width(width),
        height(height), shouldLoop(loop), atlas(atlas) { }

    int frameCount;
    double frameDuration;
    int left, bottom, width, height;
    bool shouldLoop;
    string atlas;
};


class Sprites {
public:
    Sprites();
    SpriteData& Get(const string &name);

private:
    void Insert(const string& name, const SpriteData& data) { sprites.insert(make_pair(name, data)); }
    map <string, SpriteData> sprites;
};

#endif // SPRITES_H
