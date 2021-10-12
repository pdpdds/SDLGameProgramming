#ifndef ENEMY_H
#define ENEMY_H

#include "Types.h"
#include "Tank.h"
#include <string>

using namespace std;

class Enemy :public Tank {
public:
    Enemy() : Tank(),
        currentShootTime(0.2), shootWaitingTime(0.7), currentTurnWaitingTime(0.0),
        turnWaitTime(2.0), destroyed(false), currentCollisionTurnTime(0.0), collisionTurnTime(1) { }
    void Init(int x, int y, EnemyType type);

    void Update(double dt);
    void UpdatePaused(double dt);
    void Draw();

    void Drive();
    void ChooseDirection(bool collision);

    void Shoot();
    void DecreaseBullet();
    void DecreaseLevel() { --enemyLevel; }
    int Level() { return enemyLevel; }

    int Id() { return id; }
    void SetId(int id_) { id = id_; }
    void SetCollide(bool colide) { collide = colide; }
    void Destroy() { destroyed = true; }

    bool Destroyed() { return destroyed; }
    double GetX() { return currentX; }
    double GetY() { return currentY; }
    int CBullets() { return bulletsInScreen; }
    int MaxBullets() { return maxBulletsInScreen; }
    int GetEnemyType() { return enemyType; }
    int BulletPower() { return bulletPower; }
    Direction GetDirection() { return direction; }
    EnemyType Type() { return enemyType; }

    void SetEnemyType(EnemyType type);
    void SetPosition(double x, double y) { currentX = x; currentY = y; }
    void SetDirection(Direction dir) { direction = dir; }

private:
    bool destroyed;
    bool collide;
    int enemyLevel;

    EnemyType enemyType;

    double currentShootTime;
    double shootWaitingTime;

    double currentTurnWaitingTime;
    double turnWaitTime;

    double currentCollisionTurnTime;
    double collisionTurnTime;




};

#endif // ENEMY_H
