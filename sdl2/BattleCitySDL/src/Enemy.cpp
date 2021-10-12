#include <cmath>
#include <iostream>
#include "Enemy.h"
#include "Game.h"
#include "Sprites.h"

using namespace std;

void Enemy::Init(int x, int y, EnemyType type) {
    currentX = x; currentY = y;
    movingSpeed = 3;

    if (static_cast<int>(type) >= 4)
        currentFrame = 1;
    else
        currentFrame = 0;

    currentFrameDuration = 0.0;
    width = 32;
    height = 32;
    canShoot = true;
    bulletsInScreen = 0;
    turnWaitTime = (rand() % 36 + 5) / 10.0;

    if (type == ENEMY_BIG || type == ENEMY_BIG_BONUS)
        enemyLevel = 4;
    else
        enemyLevel = 1;

    SetEnemyType(type);
    Drive();
}

void Enemy::Drive() {
    SetXVelocity(0);
    SetYVelocity(0);

    switch (direction) {
    case DIR_UP:
        velocityY = movingSpeed; 
        break;
    case DIR_DOWN:
        velocityY = -movingSpeed; 
        break;
    case DIR_LEFT:
        velocityX = -movingSpeed; 
        break;
    case DIR_RIGHT:
        velocityX = movingSpeed; 
        break;
    }
}

void Enemy::SetEnemyType(EnemyType type) {
    switch (type) {
    case ENEMY_SLOW:
        sprite_up = "enemy_slow_up";
        sprite_down = "enemy_slow_down";
        sprite_left = "enemy_slow_left";
        sprite_right = "enemy_slow_right";
        enemyType = type;
        bulletSpeed = 8;
        bulletPower = 1;
        maxBulletsInScreen = 1;
        break;
    case ENEMY_SLOW_BONUS:
        sprite_up = "enemy_slow_bonus_up";
        sprite_down = "enemy_slow_bonus_down";
        sprite_left = "enemy_slow_bonus_left";
        sprite_right = "enemy_slow_bonus_right";
        enemyType = type;
        bulletSpeed = 8;
        bulletPower = 1;
        maxBulletsInScreen = 1;
        break;
    case ENEMY_FAST:
        sprite_up = "enemy_fast_up";
        sprite_down = "enemy_fast_down";
        sprite_left = "enemy_fast_left";
        sprite_right = "enemy_fast_right";
        enemyType = type;
        movingSpeed = 7;
        bulletSpeed = 15;
        bulletPower = 1;
        maxBulletsInScreen = 1;
        break;
    case ENEMY_FAST_BONUS:
        sprite_up = "enemy_fast_bonus_up";
        sprite_down = "enemy_fast_bonus_down";
        sprite_left = "enemy_fast_bonus_left";
        sprite_right = "enemy_fast_bonus_right";
        enemyType = type;
        movingSpeed = 7;
        bulletSpeed = 15;
        bulletPower = 1;
        maxBulletsInScreen = 1;
        break;
    case ENEMY_MEDIUM:
        sprite_up = "enemy_medium_up";
        sprite_down = "enemy_medium_down";
        sprite_left = "enemy_medium_left";
        sprite_right = "enemy_medium_right";
        enemyType = type;
        bulletSpeed = 23;
        bulletPower = 1;
        maxBulletsInScreen = 1;
        break;
    case ENEMY_MEDIUM_BONUS:
        sprite_up = "enemy_medium_bonus_up";
        sprite_down = "enemy_medium_bonus_down";
        sprite_left = "enemy_medium_bonus_left";
        sprite_right = "enemy_medium_bonus_right";
        enemyType = type;
        bulletSpeed = 23;
        bulletPower = 1;
        maxBulletsInScreen = 1;
        break;
    case ENEMY_BIG:
        sprite_up = "enemy_big_up";
        sprite_down = "enemy_big_down";
        sprite_left = "enemy_big_left";
        sprite_right = "enemy_big_right";
        enemyType = type;
        movingSpeed = 3;
        bulletSpeed = 15;
        bulletPower = 1;
        maxBulletsInScreen = 1;
        break;
    case ENEMY_BIG_BONUS:
        sprite_up = "enemy_big_bonus_up";
        sprite_down = "enemy_big_bonus_down";
        sprite_left = "enemy_big_bonus_left";
        sprite_right = "enemy_big_bonus_right";
        enemyType = type;
        movingSpeed = 3;
        bulletSpeed = 15;
        bulletPower = 1;
        maxBulletsInScreen = 1;
        break;
    }
}

void Enemy::Shoot() {
    if (canShoot) {
        Game::instance().GetAudio()->PlayChunk(SOUND_FIRE);
        Game::instance().GetBullets()->CreateBullet(currentX, currentY, direction, GROUP_ENEMY, id, bulletSpeed);
        ++bulletsInScreen;
        if (bulletsInScreen >= maxBulletsInScreen) {
            canShoot = false;
        }
    }
}

void Enemy::DecreaseBullet() {
    --bulletsInScreen;
    canShoot = true;
}

void Enemy::Draw() {
    double pos_x = currentX * Game::instance().TailSize();
    double pos_y = currentY * Game::instance().TailSize();

    int frame = currentFrame;
    if (enemyType == ENEMY_BIG) {
        switch (enemyLevel) {
        case 2: frame += 6; break;
        case 3: frame += 8; break;
        case 4: frame += 10; break;
        }
    }

    switch (direction) {
    case DIR_UP:
        Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get(sprite_up), frame, pos_x, pos_y, width, height); break;
    case DIR_DOWN:
        Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get(sprite_down), frame, pos_x, pos_y, width, height); break;
    case DIR_LEFT:
        Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get(sprite_left), frame, pos_x, pos_y, width, height); break;
    case DIR_RIGHT:
        Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get(sprite_right), frame, pos_x, pos_y, width, height); break;
    }
}

void Enemy::Update(double dt) {
    currentX = currentX + velocityX * dt * speedRatio;
    currentY = currentY + velocityY * dt * speedRatio;

    collide = false;

    if (currentX < 0.0) { currentX = 0.0; collide = true; }
    if (currentX > 24.0) { currentX = 24.0; collide = true; }
    if (currentY < 0.0) { currentY = 0.0; collide = true; }
    if (currentY > 24.0) { currentY = 24.0; collide = true; }

    int pos_x_l = int(currentX), pos_x_s = int(currentX) + 1, pos_x_p = int(currentX) + 2;
    int pos_y_d = int(currentY), pos_y_s = int(currentY) + 1, pos_y_g = int(currentY) + 2;

    // collistion detection
    if (velocityX < 0 && currentX > 0.0) {
        if (Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_BRICK ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_BRICK_DAMAGED ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_WHITE ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_RIVER ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_EAGLE ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_s) == BT_BRICK ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_s) == BT_BRICK_DAMAGED ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_s) == BT_WHITE ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_s) == BT_RIVER ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_s) == BT_EAGLE) {
            currentX = pos_x_s;
            collide = true;
        }
    }
    else if (velocityX > 0 && currentX < 24.0) {
        if (Game::instance().GetLevel()->MapField(pos_x_p, pos_y_d) == BT_BRICK ||
            Game::instance().GetLevel()->MapField(pos_x_p, pos_y_d) == BT_BRICK_DAMAGED ||
            Game::instance().GetLevel()->MapField(pos_x_p, pos_y_d) == BT_WHITE ||
            Game::instance().GetLevel()->MapField(pos_x_p, pos_y_d) == BT_RIVER ||
            Game::instance().GetLevel()->MapField(pos_x_p, pos_y_d) == BT_EAGLE ||
            Game::instance().GetLevel()->MapField(pos_x_p, pos_y_s) == BT_BRICK ||
            Game::instance().GetLevel()->MapField(pos_x_p, pos_y_s) == BT_BRICK_DAMAGED ||
            Game::instance().GetLevel()->MapField(pos_x_p, pos_y_s) == BT_WHITE ||
            Game::instance().GetLevel()->MapField(pos_x_p, pos_y_s) == BT_RIVER ||
            Game::instance().GetLevel()->MapField(pos_x_p, pos_y_s) == BT_EAGLE) {
            currentX = pos_x_l;
            collide = true;
        }
    }
    else if (velocityY < 0 && currentY > 0.0) {
        if (Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_BRICK ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_BRICK_DAMAGED ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_WHITE ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_RIVER ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_EAGLE ||
            Game::instance().GetLevel()->MapField(pos_x_s, pos_y_d) == BT_BRICK ||
            Game::instance().GetLevel()->MapField(pos_x_s, pos_y_d) == BT_BRICK_DAMAGED ||
            Game::instance().GetLevel()->MapField(pos_x_s, pos_y_d) == BT_WHITE ||
            Game::instance().GetLevel()->MapField(pos_x_s, pos_y_d) == BT_RIVER ||
            Game::instance().GetLevel()->MapField(pos_x_s, pos_y_d) == BT_EAGLE) {
            currentY = pos_y_s;
            collide = true;
        }
    }
    else if (velocityY > 0 && currentY < 24.0) {
        if (Game::instance().GetLevel()->MapField(pos_x_l, pos_y_g) == BT_BRICK ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_g) == BT_BRICK_DAMAGED ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_g) == BT_WHITE ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_g) == BT_RIVER ||
            Game::instance().GetLevel()->MapField(pos_x_l, pos_y_g) == BT_EAGLE ||
            Game::instance().GetLevel()->MapField(pos_x_s, pos_y_g) == BT_BRICK ||
            Game::instance().GetLevel()->MapField(pos_x_s, pos_y_g) == BT_BRICK_DAMAGED ||
            Game::instance().GetLevel()->MapField(pos_x_s, pos_y_g) == BT_WHITE ||
            Game::instance().GetLevel()->MapField(pos_x_s, pos_y_g) == BT_RIVER ||
            Game::instance().GetLevel()->MapField(pos_x_s, pos_y_g) == BT_EAGLE) {
            currentY = pos_y_d;
            collide = true;
        }
    }

    SpriteData temp;
    switch (direction) {
    case DIR_UP:
        currentX = round(currentX);
        temp = Game::instance().GetSprites()->Get(sprite_up);
        break;
    case DIR_DOWN:
        currentX = round(currentX);
        temp = Game::instance().GetSprites()->Get(sprite_down);
        break;
    case DIR_LEFT:
        currentY = round(currentY);
        temp = Game::instance().GetSprites()->Get(sprite_left);
        break;
    case DIR_RIGHT:
        currentY = round(currentY);
        temp = Game::instance().GetSprites()->Get(sprite_right);
        break;
    }

    // animation
    currentFrameDuration += dt;
    if (currentFrameDuration >= temp.frameDuration) {
        ++currentFrame;
        if (currentFrame >= temp.frameCount) {
            if (temp.shouldLoop)
                currentFrame = 0;
            else
                currentFrame = temp.frameCount - 1;
        }
        currentFrameDuration = 0.0;
    }

    // shoot
    if (canShoot) {
        currentShootTime += dt;
        if (currentShootTime >= shootWaitingTime) {
            Shoot();
            currentShootTime = 0;
        }
    }

    // select direction
    currentTurnWaitingTime += dt;
    if (currentTurnWaitingTime >= turnWaitTime) {
        ChooseDirection(false);
        currentTurnWaitingTime = 0;
        currentCollisionTurnTime = 0;
    }

    // if collide, change direction immediately
    if (collide) {
        currentCollisionTurnTime += dt;

        if (currentCollisionTurnTime > collisionTurnTime) {
            ChooseDirection(true);
            currentCollisionTurnTime = 0;
            collisionTurnTime = (rand() % 10) / (double(movingSpeed) * 1.5);
        }
    }
}

void Enemy::UpdatePaused(double dt) {
    SpriteData temp;
    switch (direction) {
    case DIR_UP:
        currentX = round(currentX);
        temp = Game::instance().GetSprites()->Get(sprite_up);
        break;
    case DIR_DOWN:
        currentX = round(currentX);
        temp = Game::instance().GetSprites()->Get(sprite_down);
        break;
    case DIR_LEFT:
        currentY = round(currentY);
        temp = Game::instance().GetSprites()->Get(sprite_left);
        break;
    case DIR_RIGHT:
        currentY = round(currentY);
        temp = Game::instance().GetSprites()->Get(sprite_right);
        break;
    }

    currentFrameDuration += dt;
    if (currentFrameDuration >= temp.frameDuration) {
        if (currentFrame == 0)
            currentFrame = 1;
        else
            currentFrame = 0;
        currentFrameDuration = 0;
    }
}

void Enemy::ChooseDirection(bool collision) {
    if (collide) {
        int test = rand() % 4;
        if (test == 3 || test == 1) {
            turnWaitTime = 1;
            return;
        }
        else {
            turnWaitTime = 2;
        }
    }

    int randed;
    if (static_cast<int>(currentY) < 10) {      // if can shoot base
        randed = rand() % 5;
    }
    else {
        randed = rand() % 6;
    }

    Direction dir;
    if (currentX < 10) {
        switch (randed) {
        case 0: dir = DIR_DOWN; break;
        case 1: dir = DIR_RIGHT; break;
        case 2: dir = DIR_LEFT; break;
        case 3: dir = DIR_UP; break;
        case 4: dir = DIR_RIGHT; break;
        case 5: dir = DIR_DOWN; break;
        default: dir = DIR_DOWN; break;
        }
    }
    else if (currentX <= 12) {
        switch (randed) {
        case 0: dir = DIR_DOWN; break;
        case 1: dir = DIR_RIGHT; break;
        case 2: dir = DIR_LEFT; break;
        case 3: dir = DIR_UP; break;
        case 4: dir = DIR_UP; break;
        default: dir = DIR_UP; break;
        }
    }
    else {
        switch (randed) {
        case 0: dir = DIR_DOWN; break;
        case 1: dir = DIR_RIGHT; break;
        case 2: dir = DIR_LEFT; break;
        case 3: dir = DIR_UP; break;
        case 4: dir = DIR_LEFT; break;
        case 5: dir = DIR_DOWN; break;
        default: dir = DIR_DOWN; break;
        }
    }

    SetDirection(dir); Drive();
    turnWaitTime = (rand() % 31 + 5) / (static_cast<double>(movingSpeed) * 1.5);
}
