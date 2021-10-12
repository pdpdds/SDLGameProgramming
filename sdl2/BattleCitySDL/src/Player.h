#ifndef PLAYER_H
#define PLAYER_H

#include "Types.h"
#include "Tank.h"
#include <string>

using namespace std;

class Player :public Tank {
public:
    Player() :Tank(), playerState(PLAYER_STOP), isInvincible(false),
        lifes(2), isStopped(false) { }
    void Init(int x, int y, int id_);

    void Update(double dt);
    void Draw();
    void Shoot();
    void DecreaseBullet();
    void Die();
    void Born();
    void EarnStar();
    void EarnLife();
    void SetInvincibility(double time = 15.0);
    void StripInvincibility();

    int Lifes() { return lifes; }
    void LoseLife() { --lifes; }
    void SetLifes(int lifes) { lifes = lifes; if (lifes >= 0) SetAlive(true); }

    bool OnMove();
    bool Alive() { return isAlive; }
    void SetAlive(bool alive) { isAlive = alive; }
    bool Stopping() { return isStopped; }
    bool SetStopping(bool value) { isStopped = value; return true; }

    double GetX() { return currentX; }
    double GetY() { return currentY; }
    int Bullets() { return bulletsInScreen; }
    int MaxBullets() { return maxBulletsInScreen; }
    int PlayerLevel() { return playerLevel; }
    int BulletPower() { return bulletPower; }
    Direction GetDirection() { return direction; }

    void SetPosition(double x, double y) { currentX = x; currentY = y; }
    void SetDirection(Direction dir) { direction = dir; }
    void SetState(PlayerState state) { playerState = state; }
    void SetPlayerLevel(int level);
    void ResetBullets() { bulletsInScreen = 0; canShoot = true; }

    void Drive();
    void Stop(Direction dir);

private:

    Direction stoppingDirection;
    PlayerState playerState;

    int playerLevel;
    int lifes;
    bool isAlive;

    int currentInvFrame;        // invincible
    double currentInvDuration;

    bool isInvincible;
    double invRemainingTime;

    double startX;
    double startY;

    bool isStopped;

};

#endif // PLAYER_H
