#include <iostream>
#include "Bullets.h"
#include "Game.h"
#include "Sprites.h"

using namespace std;

void Bullets::DestroyAllBullets() {
    bullets.clear();
    bulletsNum = 0;

    Game::instance().GetPlayer()->ResetBullets();
    if (Game::instance().GetPlayerTwo() != NULL) Game::instance().GetPlayerTwo()->ResetBullets();
}

void Bullets::CreateBullet(double x, double y, Direction dir, Group owner, int ownerID, double bulletSpeed = 0) {
    Bullet tempBullet;
    tempBullet.owner = owner;
    tempBullet.ownerID = ownerID;
    tempBullet.isDestroyed = false;
    tempBullet.id = bulletsNum + 1;
    ++bulletsNum;

    static int tile_size = Game::instance().TailSize();

    if (bulletSpeed == 0)
        bulletSpeed = defaultSpeed;

    switch (dir) {
    case DIR_UP:
        tempBullet.x = x + (12 / double(tile_size));
        tempBullet.y = y + (24 / double(tile_size));
        tempBullet.xv = 0.0;
        tempBullet.yv = bulletSpeed;
        break;
    case DIR_DOWN:
        tempBullet.x = x + (12 / double(tile_size));
        tempBullet.y = y;
        tempBullet.xv = 0.0;
        tempBullet.yv = -bulletSpeed;
        break;
    case DIR_LEFT:
        tempBullet.x = x;
        tempBullet.y = y + (12 / double(tile_size));
        tempBullet.xv = -bulletSpeed;
        tempBullet.yv = 0.0;
        break;
    case DIR_RIGHT:
        tempBullet.x = x + (24 / double(tile_size));
        tempBullet.y = y + (12 / double(tile_size));
        tempBullet.xv = bulletSpeed;
        tempBullet.yv = 0.0;
        break;
    }

    bullets.insert(bullets.end(), tempBullet);
}

void Bullets::Draw() {
    double pos_x;
    double pos_y;
    SpriteData temp;
    for (list<Bullet>::iterator iter = bullets.begin(); iter != bullets.end(); ++iter) {
        pos_x = iter->x * Game::instance().TailSize();
        pos_y = iter->y * Game::instance().TailSize();

        temp = Game::instance().GetSprites()->Get("bullet");
        if (iter->xv < 0)              // left
            Game::instance().GetRenderer()->DrawSprite(temp, 3, pos_x, pos_y, temp.width, temp.height);
        else if (iter->xv > 0)         // right
            Game::instance().GetRenderer()->DrawSprite(temp, 1, pos_x, pos_y, temp.width, temp.height);
        else if (iter->yv < 0)         // down
            Game::instance().GetRenderer()->DrawSprite(temp, 2, pos_x, pos_y, temp.width, temp.height);
        else if (iter->yv > 0)          // up
            Game::instance().GetRenderer()->DrawSprite(temp, 0, pos_x, pos_y, temp.width, temp.height);
    }
}

void Bullets::Update(double dt) {
    for (list<Bullet>::iterator iter = bullets.begin(); iter != bullets.end(); ) {
        iter->x = iter->x + iter->xv * dt;
        iter->y = iter->y + iter->yv * dt;

        bool already_destroyed = false;

        if (iter->x <= 0) { iter->x = -1; iter = DestroyBullet(iter, DIR_LEFT); already_destroyed = true; }
        else if ((iter->x + 0.5) >= 26) { iter = DestroyBullet(iter, DIR_RIGHT); already_destroyed = true; }
        else if (iter->y <= 0) { iter->y = -1; iter = DestroyBullet(iter, DIR_DOWN); already_destroyed = true; }
        else if ((iter->y + 0.5) >= 26) { iter = DestroyBullet(iter, DIR_UP); already_destroyed = true; }
        else {
            int pos_x_l = static_cast<int>(iter->x), pos_x_s = static_cast<int>(iter->x + 0.5);
            int pos_y_d = static_cast<int>(iter->y), pos_y_s = static_cast<int>(iter->y + 0.5);
            BrickState brick1, brick2;

            if (iter->xv < 0 && iter->x > 0.0) {             // left
                brick1 = Game::instance().GetLevel()->BrickField(pos_x_l, pos_y_d);
                brick2 = Game::instance().GetLevel()->BrickField(pos_x_l, pos_y_s);
                if (Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_BRICK ||
                    (Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_BRICK_DAMAGED && (brick1.top_right || brick1.top_left)) ||
                    Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_WHITE ||
                    Game::instance().GetLevel()->MapField(pos_x_l, pos_y_s) == BT_BRICK ||
                    (Game::instance().GetLevel()->MapField(pos_x_l, pos_y_s) == BT_BRICK_DAMAGED && (brick2.bottom_right || brick2.bottom_left)) ||
                    Game::instance().GetLevel()->MapField(pos_x_l, pos_y_s) == BT_WHITE) {
                    iter = DestroyBullet(iter, DIR_LEFT); already_destroyed = true;
                }
            }
            else if (iter->xv > 0 && iter->x < 25.5) {     // right
                brick1 = Game::instance().GetLevel()->BrickField(pos_x_s, pos_y_d);
                brick2 = Game::instance().GetLevel()->BrickField(pos_x_s, pos_y_s);
                if (Game::instance().GetLevel()->MapField(pos_x_s, pos_y_d) == BT_BRICK ||
                    (Game::instance().GetLevel()->MapField(pos_x_s, pos_y_d) == BT_BRICK_DAMAGED && (brick1.top_left || brick1.top_right)) ||
                    Game::instance().GetLevel()->MapField(pos_x_s, pos_y_d) == BT_WHITE ||
                    Game::instance().GetLevel()->MapField(pos_x_s, pos_y_s) == BT_BRICK ||
                    (Game::instance().GetLevel()->MapField(pos_x_s, pos_y_s) == BT_BRICK_DAMAGED && (brick2.bottom_left || brick2.bottom_right)) ||
                    Game::instance().GetLevel()->MapField(pos_x_s, pos_y_s) == BT_WHITE) {
                    iter = DestroyBullet(iter, DIR_RIGHT); already_destroyed = true;
                }
            }
            else if (iter->yv < 0 && iter->y > 0.0) {      // down
                brick1 = Game::instance().GetLevel()->BrickField(pos_x_l, pos_y_d);
                brick2 = Game::instance().GetLevel()->BrickField(pos_x_s, pos_y_d);
                if (Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_BRICK ||
                    (Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_BRICK_DAMAGED && (brick1.top_right || brick1.bottom_right)) ||
                    Game::instance().GetLevel()->MapField(pos_x_l, pos_y_d) == BT_WHITE ||
                    Game::instance().GetLevel()->MapField(pos_x_s, pos_y_d) == BT_BRICK ||
                    (Game::instance().GetLevel()->MapField(pos_x_s, pos_y_d) == BT_BRICK_DAMAGED && (brick2.top_left || brick2.bottom_left)) ||
                    Game::instance().GetLevel()->MapField(pos_x_s, pos_y_d) == BT_WHITE) {
                    iter = DestroyBullet(iter, DIR_DOWN); already_destroyed = true;
                }
            }
            else if (iter->yv > 0 && iter->y < 25.5) {     // up
                brick1 = Game::instance().GetLevel()->BrickField(pos_x_l, pos_y_s);
                brick2 = Game::instance().GetLevel()->BrickField(pos_x_s, pos_y_s);
                if (Game::instance().GetLevel()->MapField(pos_x_l, pos_y_s) == BT_BRICK ||
                    (Game::instance().GetLevel()->MapField(pos_x_l, pos_y_s) == BT_BRICK_DAMAGED && (brick1.bottom_right || brick1.top_right)) ||
                    Game::instance().GetLevel()->MapField(pos_x_l, pos_y_s) == BT_WHITE ||
                    Game::instance().GetLevel()->MapField(pos_x_s, pos_y_s) == BT_BRICK ||
                    (Game::instance().GetLevel()->MapField(pos_x_s, pos_y_s) == BT_BRICK_DAMAGED && (brick2.bottom_left || brick2.top_left)) ||
                    Game::instance().GetLevel()->MapField(pos_x_s, pos_y_s) == BT_WHITE) {
                    iter = DestroyBullet(iter, DIR_UP); already_destroyed = true;
                }
            }
        }

        if (!already_destroyed) {
            double x1, x2, x3, x4, y1, y2, y3, y4;

            // bullets collide
            for (list<Bullet>::iterator iter2 = bullets.begin(); iter2 != bullets.end(); ++iter2) {
                if (iter != iter2) {
                    x1 = iter->x;       y1 = iter->y;
                    x2 = x1 + 0.5;      y2 = y1 + 0.5;
                    x3 = (*iter2).x;    y3 = (*iter2).y;
                    x4 = x3 + 0.5;      y4 = y3 + 0.5;

                    if (CheckCollision(x1, y1, x2, y2, x3, y3, x4, y4)) {
                        iter->isDestroyed = true;
                        (*iter2).isDestroyed = true;
                        break;
                    }
                }
            }

            // bullet <-> base
            x1 = iter->x;       y1 = iter->y;
            x2 = x1 + 0.5;      y2 = y1 + 0.5;
            x3 = 12;            y3 = 0;
            x4 = x3 + 2;        y4 = y3 + 2;
            if (CheckCollision(x1, y1, x2, y2, x3, y3, x4, y4)) {
                iter->isDestroyed = true;
                Game::instance().SetGameLost(true);
            }

            if (iter->isDestroyed) {
                if (iter->x > 12)
                    iter = DestroyBullet(iter, DIR_LEFT);
                else
                    iter = DestroyBullet(iter, DIR_RIGHT);
            }
            else {
                ++iter;
            }
        }
    }
}

list<Bullet>::iterator Bullets::DestroyBullet(list<Bullet>::iterator iter, Direction dir) {
    static int TailSize = Game::instance().TailSize();
    int power;

    if (iter->owner == GROUP_PLAYER_ONE) {
        power = Game::instance().GetPlayer()->BulletPower();
        Game::instance().GetPlayer()->DecreaseBullet();
    }
    else if (iter->owner == GROUP_PLAYER_TWO) {
        power = Game::instance().GetPlayerTwo()->BulletPower();
        Game::instance().GetPlayerTwo()->DecreaseBullet();
    }
    else {
        power = 1;
        Game::instance().GetEnemies()->DecreaseBullet(iter->ownerID);
    }

    int pos_x = static_cast<int>(iter->x);
    int pos_y = static_cast<int>(iter->y);
    switch (dir) {
    case DIR_UP:
        Game::instance().GetLevel()->DestroyTile(pos_x, pos_y + 1, pos_x + 1, pos_y + 1, power, dir); break;
    case DIR_DOWN:
        Game::instance().GetLevel()->DestroyTile(pos_x, pos_y, pos_x + 1, pos_y, power, dir); break;
    case DIR_LEFT:
        Game::instance().GetLevel()->DestroyTile(pos_x, pos_y, pos_x, pos_y + 1, power, dir); break;
    case DIR_RIGHT:
        Game::instance().GetLevel()->DestroyTile(pos_x + 1, pos_y, pos_x + 1, pos_y + 1, power, dir); break;
    }

    Game::instance().GetAnimation()->CreateAnimation(int(iter->x - 0.5) * TailSize, int(iter->y - 0.5) * TailSize, ANIMATION_EXPLODE);
    Game::instance().GetAudio()->PlayChunk(SOUND_EXPLOSION);
    return bullets.erase(iter);
}

void Bullets::DestroyBullet(int id) {
    for (list<Bullet>::iterator iter = bullets.begin(); iter != bullets.end();) {
        if (iter->id == id) {
            if (iter->owner == GROUP_PLAYER_ONE) {
                Game::instance().GetPlayer()->DecreaseBullet();
            }
            else if (iter->owner == GROUP_PLAYER_TWO) {
                Game::instance().GetPlayerTwo()->DecreaseBullet();
            }
            iter = bullets.erase(iter); break;
        }
        else {
            ++iter;
        }
    }
}
