#include <iostream>
#include <algorithm>
#include <cmath>
#include "Enemies.h"
#include "Enemy.h"
#include "Game.h"
#include "Sprites.h"
#include "Bullets.h"

using namespace std;

Enemy* Enemies::SingleEnemy(int id) {
    for (list <Enemy*>::iterator iter = enemyList.begin(); iter != enemyList.end(); ++iter) {
        if ((*iter)->Id() == id) {
            return (*iter);
        }
    }
    return NULL;
}

void Enemies::CreateEnemy() {
    if (NumOfEnemies() < 20)
        CreateEnemy(enemyTypeList[NumOfEnemies()]);
}

int Enemies::NextSpawnX() {
    switch (numOfEnemies % 3) {
    case 0: return 12; break;
    case 1: return 24; break;
    case 2: return 0; break;
    default: exit(1);
    }
}

void Enemies::CreateEnemy(EnemyType type) {
    Enemy* tempEnemy = new Enemy;

    int x;
    switch (numOfEnemies % 3) {
    case 0: x = 12; break;
    case 1: x = 24; break;
    case 2: x = 0; break;
    }

    tempEnemy->SetDirection(DIR_DOWN);
    tempEnemy->Init(x, 24, type);
    tempEnemy->SetId(NumOfEnemies());

    if (int(tempEnemy->Type()) >= 4) {
        Game::instance().GetItems()->DestroyItem();
    }

    enemyList.push_back(tempEnemy);
    ++numOfEnemies;
}

void Enemies::Draw() {
    for (list<Enemy*>::iterator iter = enemyList.begin(); iter != enemyList.end(); ++iter) {
        (*iter)->Draw();
    }
}

void Enemies::DestroyAllEnemies(bool wipe) {
    static int TailSize = Game::instance().TailSize();
    for (list<Enemy*>::iterator iter = enemyList.begin(); iter != enemyList.end(); ++iter) {
        Game::instance().GetAnimation()->CreateAnimation((*iter)->GetX() * TailSize, (*iter)->GetY() * TailSize, ANIMATION_EXPLODE);
        delete (*iter);
    }
    enemyList.clear();
    if (wipe)
        numOfEnemies = 0;
}

void Enemies::DecreaseBullet(int id) {
    for (list<Enemy*>::iterator iter = enemyList.begin(); iter != enemyList.end(); ++iter) {
        if ((*iter)->Id() == id) 
            (*iter)->DecreaseBullet();
    }
}

void Enemies::Update(double dt) {
    if (isPaused) {
        currentPauseTime += dt;
        if (currentPauseTime >= pauseTime) {
            isPaused = false;
            currentPauseTime = 0.0;
        }
    }

    for (list<Enemy*>::iterator iter = enemyList.begin(); iter != enemyList.end(); ) {
        if (!isPaused)
            (*iter)->Update(dt);
        else if (int((*iter)->Type()) >= 4) {
            (*iter)->UpdatePaused(dt);
        }

        if ((*iter)->Level() <= 0) {
            (*iter)->Destroy();
        }

        if ((*iter)->Destroyed())
            iter = DestroyEnemy(iter);
        else {
            (*iter)->SetCollide(false);
            // collide with other tank
            double x1, x2, x3, x4, y1, y2, y3, y4;
            for (list<Enemy*>::iterator iter2 = enemyList.begin(); iter2 != enemyList.end(); ++iter2) {
                if ((*iter)->Id() != (*iter2)->Id()) {
                    x1 = (*iter)->GetX();       y1 = (*iter)->GetY();
                    x2 = x1 + 2;                y2 = y1 + 2;
                    x3 = (*iter2)->GetX();      y3 = (*iter2)->GetY();
                    x4 = x3 + 2;                y4 = y3 + 2;

                    if (CheckCollision(x1, y1, x2, y2, x3, y3, x4, y4)) {
                        switch ((*iter)->GetDirection()) {
                        case DIR_UP:
                            if ((*iter2)->GetY() > (*iter)->GetY()) {
                                double new_y = (*iter2)->GetY() - 2;
                                if (new_y < 0)
                                    new_y = 0;
                                (*iter)->SetPosition((*iter)->GetX(), new_y);
                                (*iter)->SetCollide(true);
                            }
                            break;
                        case DIR_DOWN:
                            if ((*iter2)->GetY() < (*iter)->GetY()) {
                                double new_y = (*iter2)->GetY() + 2;
                                if (new_y > 24)
                                    new_y = 24;
                                (*iter)->SetPosition((*iter)->GetX(), new_y);
                                (*iter)->SetCollide(true);
                            }
                            break;
                        case DIR_LEFT:
                            if ((*iter2)->GetX() < (*iter)->GetX()) {
                                double new_x = (*iter2)->GetX() + 2;
                                if (new_x > 24)
                                    new_x = 24;
                                (*iter)->SetPosition(new_x, (*iter)->GetY());
                                (*iter)->SetCollide(true);
                            }
                            break;
                        case DIR_RIGHT:
                            if ((*iter2)->GetX() > (*iter)->GetX()) {
                                double new_x = (*iter2)->GetX() - 2;
                                if (new_x < 0)
                                    new_x = 0;
                                (*iter)->SetPosition(new_x, (*iter)->GetY());
                                (*iter)->SetCollide(true);
                            }
                            break;
                        default: break;
                        }
                    }
                }
            }

            // collide with player one
            x1 = (*iter)->GetX();                   y1 = (*iter)->GetY();
            x2 = x1 + 2;                            y2 = y1 + 2;
            x3 = Game::instance().GetPlayer()->GetX();     y3 = Game::instance().GetPlayer()->GetY();
            x4 = x3 + 2;                            y4 = y3 + 2;

            if (CheckCollision(x1, y1, x2, y2, x3, y3, x4, y4)) {
                switch ((*iter)->GetDirection()) {
                case DIR_UP:
                    if (y3 > y1) {
                        double new_y = y3 - 2;
                        if (new_y < 0)
                            new_y = 0;
                        (*iter)->SetPosition(x1, new_y);
                        (*iter)->SetCollide(true);
                    } 
                    break;
                case DIR_DOWN:
                    if (y3 < y1) {
                        double new_y = y3 + 2;
                        if (new_y > 24)
                            new_y = 24;
                        (*iter)->SetPosition(x1, new_y);
                        (*iter)->SetCollide(true);
                    } 
                    break;
                case DIR_LEFT:
                    if (x3 < x1) {
                        double new_x = x3 + 2;
                        if (new_x > 24)
                            new_x = 24;
                        (*iter)->SetPosition(new_x, y1);
                        (*iter)->SetCollide(true);
                    } 
                    break;
                case DIR_RIGHT:
                    if (x3 > x1) {
                        double new_x = x3 - 2;
                        if (new_x < 0)
                            new_x = 0;
                        (*iter)->SetPosition(new_x, y1);
                        (*iter)->SetCollide(true);
                    } 
                    break;
                }
            }

            // collide wit player two
            if (Game::instance().GetPlayerTwo() != NULL) {
                x1 = (*iter)->GetX();                   y1 = (*iter)->GetY();
                x2 = x1 + 2;                            y2 = y1 + 2;
                x3 = Game::instance().GetPlayerTwo()->GetX();  y3 = Game::instance().GetPlayerTwo()->GetY();
                x4 = x3 + 2;                            y4 = y3 + 2;

                if (CheckCollision(x1, y1, x2, y2, x3, y3, x4, y4)) {
                    switch ((*iter)->GetDirection()) {
                    case DIR_UP:
                        if (y3 > y1) {
                            double new_y = y3 - 2;
                            if (new_y < 0)
                                new_y = 0;
                            (*iter)->SetPosition(x1, new_y);
                            (*iter)->SetCollide(true);
                        }
                        break;
                    case DIR_DOWN:
                        if (y3 < y1) {
                            double new_y = y3 + 2;
                            if (new_y > 24)
                                new_y = 24;
                            (*iter)->SetPosition(x1, new_y);
                            (*iter)->SetCollide(true);
                        }
                        break;
                    case DIR_LEFT:
                        if (x3 < x1) {
                            double new_x = x3 + 2;
                            if (new_x > 24)
                                new_x = 24;
                            (*iter)->SetPosition(new_x, y1);
                            (*iter)->SetCollide(true);
                        }
                        break;
                    case DIR_RIGHT:
                        if (x3 > x1) {
                            double new_x = x3 - 2;
                            if (new_x < 0)
                                new_x = 0;
                            (*iter)->SetPosition(new_x, y1);
                            (*iter)->SetCollide(true);
                        }
                        break;
                    }
                }
            }


            // collide with bullet
            list<Bullet> temp = Game::instance().GetBullets()->AllBullets();
            for (list<Bullet>::iterator iter2 = temp.begin(); iter2 != temp.end(); ++iter2) {
                if ((*iter2).owner != GROUP_ENEMY) {
                    double x1, x2, x3, x4, y1, y2, y3, y4;
                    x1 = (*iter)->GetX();       y1 = (*iter)->GetY();
                    x2 = x1 + 2;                y2 = y1 + 2;
                    x3 = (*iter2).x;            y3 = (*iter2).y;
                    x4 = x3 + 0.5;              y4 = y3 + 0.5;

                    if (CheckCollision(x1, y1, x2, y2, x3, y3, x4, y4)) {
                        (*iter)->DecreaseLevel();
                        if ((*iter)->Level() == 3 && (*iter)->GetEnemyType() == ENEMY_BIG_BONUS) {
                            (*iter)->SetEnemyType(ENEMY_BIG);
                            Game::instance().GetItems()->CreateItem();
                        }
                        Game::instance().GetBullets()->DestroyBullet((*iter2).id);
                        break;
                    }
                }
            }
            ++iter;
        }
    }
}

list<Enemy*>::iterator Enemies::DestroyEnemy(list<Enemy*>::iterator iter) {
    if ((*iter)->Level() <= 0) {
        if (int((*iter)->Type()) >= 4) {
            Game::instance().GetItems()->CreateItem();
        }
        static int TailSize = Game::instance().TailSize();
        Game::instance().GetAnimation()->CreateAnimation((*iter)->GetX() * TailSize, (*iter)->GetY() * TailSize, ANIMATION_EXPLODE);
        Game::instance().GetAudio()->PlayChunk(SOUND_DIE);

        delete (*iter);
        return enemyList.erase(iter);
    }
    else {
        return ++iter;
    }
}
