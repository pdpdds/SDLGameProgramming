#ifndef TYPES_H
#define TYPES_H

enum Direction {
    DIR_UP = 0,
    DIR_RIGHT = 1,
    DIR_DOWN = 2,
    DIR_LEFT = 3
};

enum Group {
    GROUP_PLAYER_ONE = 0,
    GROUP_PLAYER_TWO = 1,
    GROUP_ENEMY = 2
};

enum PlayerState {
    PLAYER_STOP = 0,
    PLAYER_DRIVE = 1
};

enum GameStateType {
    GS_MENU,
    GS_STAGESELECT,
    GS_GAMEPLAY,
    GS_SCORES,
    GS_EDITOR
};

enum BrickType {
    BT_NONE = 0,
    BT_BRICK = 1,
    BT_BRICK_DAMAGED = 10,
    BT_WHITE = 2,
    BT_SLIDE = 3,
    BT_BUSH  = 4,
    BT_RIVER = 5,
    BT_EAGLE = 9
};

enum EnemyType {
    ENEMY_SLOW = 0,
    ENEMY_FAST = 1,
    ENEMY_MEDIUM = 2,
    ENEMY_BIG = 3,
    ENEMY_SLOW_BONUS = 4,
    ENEMY_FAST_BONUS = 5,
    ENEMY_MEDIUM_BONUS = 6,
    ENEMY_BIG_BONUS = 7
};

enum ItemType {
    ITEM_STAR = 0,
    ITEM_LIFE = 1,
    ITEM_INVINCIBILITY = 2,
    ITEM_PROTECT = 3,
    ITEM_TIME = 4,
    ITEM_GRANADE = 5
};

enum AnimationType {
    ANIMATION_SPAWN,
    ANIMATION_EXPLODE
};

enum SoundType {
    SOUND_ONMOVE = 0,
    SOUND_FIRE = 1,
    SOUND_EXPLOSION = 2,
    SOUND_BONUS = 3,
    SOUND_GAMESTART = 4,
    SOUND_GAMEOVER = 5,
    SOUND_DIE = 6
};

enum ColorType {
    COLOR_NONE,
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE
};

#endif // TYPES_H
