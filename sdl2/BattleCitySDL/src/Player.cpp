#include <cmath>
#include <iostream>
#include <string>
#include "Player.h"
#include "Game.h"
#include "Sprites.h"

using namespace std;

void Player::Init(int x, int y, int id_) {
    currentX = x;
    currentY = y; 
    id = id_;
    movingSpeed = 5.5;
    currentFrame = 0;
    currentFrameDuration = 0.0;
    width = 32;
    height = 32;
    canShoot = true;
    bulletsInScreen = 0;

    startX = x;  
    startY = y;

    SetPlayerLevel(0);
}

void Player::SetPlayerLevel(int level) {
    string playerIndex = "one";
    if (id == 2) 
        playerIndex = "two";
    switch (level) {
    case 0:
        sprite_up = "player_" + playerIndex + "_up_0";
        sprite_down = "player_" + playerIndex + "_down_0";
        sprite_left = "player_" + playerIndex + "_left_0";
        sprite_right = "player_" + playerIndex + "_right_0";
        playerLevel = 0;
        bulletSpeed = 13;
        bulletPower = 1;
        maxBulletsInScreen = 1;
        break;
    case 1:
        sprite_up = "player_" + playerIndex + "_up_1";
        sprite_down = "player_" + playerIndex + "_down_1";
        sprite_left = "player_" + playerIndex + "_left_1";
        sprite_right = "player_" + playerIndex + "_right_1";
        playerLevel = 1;
        bulletSpeed = 26;
        maxBulletsInScreen = 1;
        bulletPower = 1;
        break;
    case 2:
        sprite_up = "player_" + playerIndex + "_up_2";
        sprite_down = "player_" + playerIndex + "_down_2";
        sprite_left = "player_" + playerIndex + "_left_2";
        sprite_right = "player_" + playerIndex + "_right_2";
        playerLevel = 2;
        bulletSpeed = 26;
        maxBulletsInScreen = 2;
        bulletPower = 1;
        break;
    case 3:
        sprite_up = "player_" + playerIndex + "_up_3";
        sprite_down = "player_" + playerIndex + "_down_3";
        sprite_left = "player_" + playerIndex + "_left_3";
        sprite_right = "player_" + playerIndex + "_right_3";
        playerLevel = 3;
        bulletSpeed = 26;
        maxBulletsInScreen = 2;
        bulletPower = 2;
        break;
    }
}

void Player::Drive() {
    SetStopping(false);
    SetYVelocity(0); 
    SetXVelocity(0);
    playerState = PLAYER_DRIVE;
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

void Player::Stop(Direction dir) {
    bool is_on_slide = false;
    if (Game::instance().GetLevel()->MapField(int(currentX), int(currentY)) == BT_SLIDE && Game::instance().GetLevel()->MapField(int(currentX + 1), int(currentY)) == BT_SLIDE &&
        Game::instance().GetLevel()->MapField(int(currentX), int(currentY + 1)) == BT_SLIDE && Game::instance().GetLevel()->MapField(int(currentX + 1), int(currentY + 1)) == BT_SLIDE)
        is_on_slide = true;

    if (!is_on_slide) {
        if (dir == DIR_LEFT || dir == DIR_RIGHT)
            SetXVelocity(0);
        else
            SetYVelocity(0);

        SetState(PLAYER_STOP);
    }
    else {
        if (dir == DIR_LEFT || dir == DIR_RIGHT) {
            velocityX = velocityX / 1.05;
            stoppingDirection = dir;
            if (fabs(velocityX) < 0.05) {
                velocityX = 0;
                SetState(PLAYER_STOP);
                SetStopping(false);
            }
            else {
                SetStopping(true);
            }
        }
        else {
            velocityY = velocityY / 1.05;
            stoppingDirection = dir;
            if (fabs(velocityY) < 0.05) {
                velocityY = 0;
                SetState(PLAYER_STOP);
                SetStopping(false);
            }
            else {
                SetStopping(true);
            }
        }
    }
}

void Player::Shoot() {
    if (canShoot) {
        Game::instance().GetAudio()->PlayChunk(SOUND_FIRE);
        if (id == 1)
            Game::instance().GetBullets()->CreateBullet(currentX, currentY, direction, GROUP_PLAYER_ONE, id, bulletSpeed);
        else if (id == 2)
            Game::instance().GetBullets()->CreateBullet(currentX, currentY, direction, GROUP_PLAYER_TWO, id, bulletSpeed);
        ++bulletsInScreen;
        if (bulletsInScreen >= maxBulletsInScreen) {
            canShoot = false;
        }
    }
}

void Player::DecreaseBullet() {
    --bulletsInScreen;
    if (bulletsInScreen < 0)
        bulletsInScreen = 0;
    canShoot = true;
}

void Player::Draw() {
    double pos_x = currentX * Game::instance().TailSize();
    double pos_y = currentY * Game::instance().TailSize();

    switch (direction) {
    case DIR_UP:
        Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get(sprite_up), currentFrame, pos_x, pos_y, width, height); break;
    case DIR_DOWN:
        Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get(sprite_down), currentFrame, pos_x, pos_y, width, height); break;
    case DIR_LEFT:
        Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get(sprite_left), currentFrame, pos_x, pos_y, width, height); break;
    case DIR_RIGHT:
        Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get(sprite_right), currentFrame, pos_x, pos_y, width, height); break;
    }
    if (isInvincible) {
        Game::instance().GetRenderer()->DrawSprite(Game::instance().GetSprites()->Get("player_border"), currentInvFrame, pos_x, pos_y, width, height);
    }
}

void Player::EarnStar() {
    switch (PlayerLevel()) {
    case 0: SetPlayerLevel(1); break;
    case 1: SetPlayerLevel(2); break;
    case 2: SetPlayerLevel(3); break;
    case 3: break;
    }
}

void Player::EarnLife() {
    ++lifes;
}

void Player::Born() {
    SetPosition(startX, startY);
    SetAlive(true);
    SetDirection(DIR_UP);
    SetState(PLAYER_STOP);
    SetXVelocity(0);
    SetYVelocity(0);
    SetInvincibility(3.0);
}

bool Player::OnMove() {
    if (velocityX != 0 || velocityY != 0)
        return true;
    return false;
}

void Player::Die() {
    static int TailSize = Game::instance().TailSize();
    if (!isInvincible) {
        SetPlayerLevel(0);
        Game::instance().GetAudio()->PlayChunk(SOUND_DIE);
        Game::instance().GetAnimation()->CreateAnimation(GetX() * TailSize, GetY() * TailSize, ANIMATION_EXPLODE);
        LoseLife();
        SetAlive(false);
        if (Lifes() < 0)
            SetPosition(26, 26);
    }
}

void Player::SetInvincibility(double time) {
    isInvincible = true;
    invRemainingTime = time;
}

void Player::StripInvincibility() {
    isInvincible = false;
    invRemainingTime = 0.0;
}

void Player::Update(double dt) {
    if (playerState == PLAYER_DRIVE && isStopped) {
        Stop(stoppingDirection);
    }

    currentX = currentX + velocityX * dt * speedRatio;
    currentY = currentY + velocityY * dt * speedRatio;

    if (isInvincible) {
        invRemainingTime -= dt;
        if (invRemainingTime <= 0)    isInvincible = false;
    }

    if (currentX < 0.0)  currentX = 0.0;
    if (currentX > 24.0) currentX = 24.0;
    if (currentY < 0.0)  currentY = 0.0;
    if (currentY > 24.0) currentY = 24.0;

    int pos_x_l = static_cast<int>(currentX), pos_x_s = static_cast<int>(currentX) + 1, pos_x_p = static_cast<int>(currentX) + 2;
    int pos_y_d = static_cast<int>(currentY), pos_y_s = static_cast<int>(currentY) + 1, pos_y_g = static_cast<int>(currentY) + 2;

    // collision detection
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
        }
    }

    // player one collide with enemy
    list <Enemy*> enemies = Game::instance().GetEnemies()->EnemiesList();
    double x1, x2, x3, x4, y1, y2, y3, y4;

    for (list <Enemy*>::iterator iter = enemies.begin(); iter != enemies.end(); ++iter) {
        x1 = GetX();                y1 = GetY();
        x2 = x1 + 2;                y2 = y1 + 2;
        x3 = (*iter)->GetX();       y3 = (*iter)->GetY();
        x4 = x3 + 2;                y4 = y3 + 2;

        if (CheckCollision(x1, y1, x2, y2, x3, y3, x4, y4)) {
            switch (GetDirection()) {
            case DIR_UP:
                if (y1 < y3) {
                    double new_y = y3 - 2;
                    if (new_y < 0)
                        new_y = 0;
                    SetPosition(x1, new_y);
                } break;
            case DIR_DOWN:
                if (y1 > y3) {
                    double new_y = y3 + 2;
                    if (new_y > 24)
                        new_y = 24;
                    SetPosition(x1, new_y);
                } break;
            case DIR_LEFT:
                if (x1 > x3) {
                    double new_x = x3 + 2;
                    if (new_x > 24)
                        new_x = 24;
                    SetPosition(new_x, y1);
                } break;
            case DIR_RIGHT:
                if (x1 < x3) {
                    double new_x = x3 - 2;
                    if (new_x < 0)
                        new_x = 0;
                    SetPosition(new_x, y1);
                } break;
            }
        }
    }

    // player two collide with enemy
    if (Game::instance().GetPlayerTwo() != NULL) {
        if (id == 1) {
            x1 = Game::instance().GetPlayer()->GetX();      y1 = Game::instance().GetPlayer()->GetY();
            x2 = x1 + 2;                                    y2 = y1 + 2;
            x3 = Game::instance().GetPlayerTwo()->GetX();   y3 = Game::instance().GetPlayerTwo()->GetY();
            x4 = x3 + 2;                                    y4 = y3 + 2;
        }
        else if (id == 2) {
            x1 = Game::instance().GetPlayerTwo()->GetX();   y1 = Game::instance().GetPlayerTwo()->GetY();
            x2 = x1 + 2;                                    y2 = y1 + 2;
            x3 = Game::instance().GetPlayer()->GetX();      y3 = Game::instance().GetPlayer()->GetY();
            x4 = x3 + 2;                                    y4 = y3 + 2;
        }

        if (CheckCollision(x1, y1, x2, y2, x3, y3, x4, y4)) {
            switch (GetDirection()) {
            case DIR_UP:
                if (y1 < y3) {
                    double new_y = y3 - 2;
                    if (new_y < 0)
                        new_y = 0;
                    SetPosition(x1, new_y);
                } break;
            case DIR_DOWN:
                if (y1 > y3) {
                    double new_y = y3 + 2;
                    if (new_y > 24)
                        new_y = 24;
                    SetPosition(x1, new_y);
                } break;
            case DIR_LEFT:
                if (x1 > x3) {
                    double new_x = x3 + 2;
                    if (new_x > 24)
                        new_x = 24;
                    SetPosition(new_x, y1);
                } break;
            case DIR_RIGHT:
                if (x1 < x3) {
                    double new_x = x3 - 2;
                    if (new_x < 0)
                        new_x = 0;
                    SetPosition(new_x, y1);
                } break;
            }
        }
    }

    // player get item
    Item item = Game::instance().GetItems()->GetCurrentItem();
    if (item.destroyed == false) {
        double x1, x2, x3, x4, y1, y2, y3, y4;
        x1 = item.x;        y1 = item.y;
        x2 = x1 + 2;        y2 = y1 + 2;
        x3 = GetX();        y3 = GetY();
        x4 = x3 + 2;        y4 = y3 + 2;

        if (CheckCollision(x1, y1, x2, y2, x3, y3, x4, y4)) {
            Game::instance().GetAudio()->PlayChunk(SOUND_BONUS);
            switch (item.type) {
            case ITEM_STAR:
                EarnStar(); break;
            case ITEM_LIFE:
                EarnLife(); break;
            case ITEM_INVINCIBILITY:
                SetInvincibility(); break;
            case ITEM_PROTECT:
                Game::instance().GetGameTimer()->SetLevelProtect(true); break;
            case ITEM_TIME:
                Game::instance().GetEnemies()->PauseEnemies(); break;
            case ITEM_GRANADE:
                Game::instance().GetEnemies()->DestroyAllEnemies(false); break;
            }
            Game::instance().GetItems()->DestroyItem();
        }
    }

    // collide with bullet
    list<Bullet> temp = Game::instance().GetBullets()->AllBullets();

    for (list<Bullet>::iterator iter = temp.begin(); iter != temp.end(); ++iter) {
        if (iter->owner == GROUP_ENEMY) {
            x1 = GetX();        y1 = GetY();
            x2 = x1 + 2;        y2 = y1 + 2;
            x3 = iter->x;       y3 = iter->y;
            x4 = x3 + 0.5;      y4 = y3 + 0.5;

            if (CheckCollision(x1, y1, x2, y2, x3, y3, x4, y4)) {
                Die();

                Game::instance().GetBullets()->DestroyBullet(iter->id);
                if (Game::instance().GetEnemies()->SingleEnemy(iter->ownerID) != NULL)
                    Game::instance().GetEnemies()->SingleEnemy(iter->ownerID)->DecreaseBullet();
                break;
            }
        }
    }

    static SpriteData temp_inv = Game::instance().GetSprites()->Get("player_border");
    if (isInvincible) {
        currentInvDuration += dt;
        if (currentInvDuration >= temp_inv.frameDuration) {
            ++currentInvFrame;
            if (currentInvFrame >= temp_inv.frameCount) {
                if (temp_inv.shouldLoop)   
                    currentInvFrame = 0;
                else                
                    currentInvFrame = temp_inv.frameCount - 1;
            }
            currentInvDuration = 0.0;
        }
    }

    if (velocityX == 0.0 && velocityY == 0.0 && playerState == PLAYER_STOP)
        currentFrame = 0;
    else {
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
            ++currentFrame;
            if (currentFrame >= temp.frameCount) {
                if (temp.shouldLoop) 
                    currentFrame = 0;
                else          
                    currentFrame = temp.frameCount - 1;
            }
            currentFrameDuration = 0.0;
        }
    }
}
