#ifndef ENEMIES_H
#define ENEMIES_H

#include <list>
#include <iostream>
#include <vector>
#include "Enemy.h"
#include "Types.h"

using namespace std;

class Enemies {
public:
    Enemies() : numOfEnemies(0), pauseTime(15), isPaused(false) { enemyTypeList.resize(20); }
    ~Enemies() { DestroyAllEnemies(); }

    void Update(double dt);
    void Draw();
    void CreateEnemy(EnemyType type);
    void CreateEnemy();
    list<Enemy*>::iterator DestroyEnemy(list<Enemy*>::iterator iter);
    void DestroyAllEnemies(bool wipe = true);
    void DecreaseBullet(int id);


    void PauseEnemies() { isPaused = true; currentPauseTime = 0; }
    void UnPause() { isPaused = false; currentPauseTime = 0; }
    void SetEnemyType(int id, EnemyType type) { enemyTypeList[id] = type; }
    void SetEnemiesNum(int num) { numOfEnemies = num; }

    bool Paused() { return isPaused; }
    int NumOfEnemies() { return numOfEnemies; }
    int AliveEnemies() { return enemyList.size(); }
    int NextSpawnX();

    Enemy* SingleEnemy(int id);
    list<Enemy*> EnemiesList() { return enemyList; }

private:
    bool isPaused;
    double currentPauseTime;
    double pauseTime;

    list <Enemy*> enemyList;
    vector <EnemyType> enemyTypeList;
    int numOfEnemies;
};

#endif // ENEMIES_H
