#ifndef GAMETIMER_H
#define GAMETIMER_H

class GameTimer {
public:
    GameTimer() : selectedLevel(1), showedLevel(1), enemySpawnAnimation(false) { Init(); }
    void Init();
    void Update(double dt);
    void SetLevelProtect(bool protect);
    void SetSelectedLevel(int num) { selectedLevel = showedLevel = 1; }
    void ChooseLevel();

    void DrawStageSelect();
    void ProcessEvents();

    int SelectedLevel() { return selectedLevel; }
    int ShowedLevel() { return showedLevel; }

private:
    int     maxEnemiesInScreen;
    int     selectedLevel;
    int     showedLevel;

    bool    levelProtectState;
    bool    enemySpawnAnimation;

    double  currentEnemyTime;
    double  currentPlayerTime;
    double  currentNextmapTime;
    double  currentLevelProtectTime;
    double  currentPlayerOneRssurectionTime;
    double  currentPlayerTwoRssurectionTime;
    double  currentGameLostTime;

    double  enemySpawnTime;
    double  playerSpawnTime;
    double  nextmapChangeTime;
    double  levelProtectTime;
    double  playerRessurectionTime;
    double  gameLostTime;
};

#endif // GAMETIMER_H
