// monster state and behavior
#ifndef __MONSTER_H
#define __MONSTER_H

typedef struct {
	uint8_t type; // type and tile index
	uint8_t dead_timer;
	uint8_t fire_timer;
	uint8_t path_index; // current point in path
	uint8_t tx, ty; // tile pos
	int16_t px, py; // pixel pos
	int8_t npx, npy; // next relative pixel path movement
} monster_state_t;

void M_Move();
void M_Fire();
void M_UpdateBullet();

#endif

