#ifndef BULLETS_H
#define BULLETS_H

#include <list>
#include "Types.h"

using namespace std;

struct Bullet {
    Bullet() {}
    Bullet(double x, double y, Group owner) : x(x), y(y), owner(owner) { }

    int id;
    double x, y;
    double xv, yv;
    Group owner;
    int ownerID;
    bool isDestroyed;
};

class Bullets {
public:
    Bullets() : defaultSpeed(10), bulletsNum(0) { }
    void Update(double dt);
    void Draw();
    void CreateBullet(double x, double y, Direction dir, Group owner, int ownerID, double bulletSpeed);

    list<Bullet>::iterator DestroyBullet(list<Bullet>::iterator iter, Direction dir);

    void DestroyBullet(int id);
    void DestroyAllBullets();

    int BulletsNum() { return bullets.size(); }
    list<Bullet> AllBullets() { return bullets; }

private:
    list<Bullet> bullets;
    double defaultSpeed;
    int bulletsNum;
};

#endif // BULLETS_H
