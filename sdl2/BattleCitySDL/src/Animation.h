#ifndef EFFECTS_H
#define EFFECTS_H

#include <list>
#include "Sprites.h"
#include "Types.h"

using namespace std;

struct Effect {
    Effect() : currentDuration(0.0), frame(0) { }
    Effect(int id, double x, double y) : id(id), x(x), y(y), currentDuration(0.0), frame(0), currentFrameDuration(0.0) { }
    int id;
    double x, y;
    SpriteData sprite;
    double currentDuration;
    double currentFrameDuration;
    double duration;
    int frame;
    bool destroyed;
};

class Animation {
public:
    Animation() : numOfAnimation(0) { }
    void Update(double dt);
    void Draw();
    void CreateAnimation(double x, double y, AnimationType type);

    void DestroyAllAnimation();
    list<Effect>::iterator DestroyEffect(list<Effect>::iterator iter);

    list<Effect> AnimationList() { return animationList; }

private:
    int             numOfAnimation;
    list <Effect>   animationList;
};

#endif // EFFECTS_H
