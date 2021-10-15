// player state and movement
#ifndef __PLAYER_H
#define __PLAYER_H

// player state
typedef struct {
	uint8_t tx, ty; // tile pos
	int16_t px, py; // pixel pos
	uint16_t score;
	uint8_t lives;
	// on ground flag
	uint8_t on_ground;
	// last facing direction
	int8_t last_dir;
	uint16_t tick;

	// input flags
	uint8_t try_right;
	uint8_t try_left;
	uint8_t try_jump;
	uint8_t try_fire;
	uint8_t try_jetpack;
	uint8_t try_up, try_down; // jetpack/climbing

	uint8_t do_right;
	uint8_t do_left;
	uint8_t do_jump;
	uint8_t do_fire;
	uint8_t do_jetpack;
	uint8_t do_up, do_down;

	uint8_t jump_timer;
	uint8_t dead_timer;
	// pickup tile pos
	uint8_t check_pickup_x;
	uint8_t check_pickup_y;
	// door was hit, check if passable
	uint8_t check_door;
	// item flags; jetpack is also fuel count
	uint8_t trophy, gun, jetpack;
	// bullet
	uint16_t bullet_px, bullet_py;
	int8_t bullet_dir;

	// collision point clear flags; 1 = clear
	uint8_t col_point[8];
} player_state_t;

void P_Spawn();
void P_PickupItem();
void P_UpdateCollision();
void P_UpdateBullet();
void P_VerifyInput();
void P_Move();
void P_ApplyGravity();

#endif
