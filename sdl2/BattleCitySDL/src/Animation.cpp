#include <iostream>
#include "Bullets.h"
#include "Game.h"
#include "Sprites.h"

using namespace std;

void Animation::DestroyAllAnimation() {
    animationList.clear();
    numOfAnimation = 0;
}

void Animation::CreateAnimation(double x, double y, AnimationType type) {
    Effect tempEffect(numOfAnimation, x, y);
    ++numOfAnimation;

    tempEffect.destroyed = false;
    switch (type) {
    case ANIMATION_SPAWN:
        tempEffect.sprite = Game::instance().GetSprites()->Get("effect_spawn");
        tempEffect.duration = 0.5;
        break;
    case ANIMATION_EXPLODE:
        tempEffect.sprite = Game::instance().GetSprites()->Get("effect_explode");
        tempEffect.duration = 0.15;
        break;
    }

    animationList.insert(animationList.end(), tempEffect);
}

void Animation::Draw() {
    for (list<Effect>::iterator iter = animationList.begin(); iter != animationList.end(); ++iter) {
        Game::instance().GetRenderer()->DrawSprite(iter->sprite, iter->frame, iter->x, iter->y, iter->sprite.width, iter->sprite.height);
    }
}

void Animation::Update(double dt) {
    for (list<Effect>::iterator iter = animationList.begin(); iter != animationList.end(); ) {
        iter->currentDuration += dt;
        iter->currentFrameDuration += dt;

        if (iter->currentDuration >= iter->duration) {
            iter = DestroyEffect(iter);
        }
        else {
            if (iter->currentFrameDuration >= iter->sprite.frameDuration) {
                ++iter->frame;
                if (iter->frame >= iter->sprite.frameCount) {
                    if (iter->sprite.shouldLoop) iter->frame = 0;
                    else                    iter->frame = iter->sprite.frameCount - 1;
                }
                iter->currentFrameDuration = 0;
            }
            ++iter;
        }
    }
}

list<Effect>::iterator Animation::DestroyEffect(list<Effect>::iterator iter) {
    return animationList.erase(iter);
}
