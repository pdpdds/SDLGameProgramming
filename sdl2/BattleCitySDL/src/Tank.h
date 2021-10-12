#ifndef TANK_H
#define TANK_H

#include "Types.h"
#include <string>

using namespace std;

class Tank {
public:
    Tank() : velocityX(0.0), velocityY(0.0), speedRatio(1.0), maxBulletsInScreen(1), direction(DIR_DOWN) { }

public:
    void SetXVelocity(double v) { velocityX = v; }
    void SetYVelocity(double v) { velocityY = v; }

protected:
    int id;

    Direction direction;
    double movingSpeed;
    double currentX, currentY;
    double velocityX, velocityY;
    double speedRatio;

    // animation frame
    int currentFrame;
    double currentFrameDuration;

    int bulletsInScreen;
    int maxBulletsInScreen;
    bool canShoot;
    double bulletSpeed;
    int bulletPower;

    string sprite_up;
    string sprite_down;
    string sprite_left;
    string sprite_right;

    int width;
    int height;

};

#endif // TANK_H
