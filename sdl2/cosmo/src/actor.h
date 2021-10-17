#ifndef COSMO_ENGINE_ACTOR_H
#define COSMO_ENGINE_ACTOR_H

#include "defines.h"
#include "tile.h"

#define MAX_ACTORS 410

typedef struct ActorData
{
    int actorInfoIndex;
    int frame_num;
    int x;
    int y;
    int update_while_off_screen_flag;
    int can_update_if_goes_off_screen_flag;
    int non_blocking_flag_maybe;
    int can_fall_down_flag;
    int has_moved_left_flag;
    int has_moved_right_flag;
    uint16 data_1;
    uint16 data_2;
    uint16 data_3;
    uint16 data_4;
    uint16 data_5;
    int is_deactivated_flag_maybe;
    int falling_counter;
    int count_down_timer;
    void (*update_function)(struct ActorData *);
} ActorData;

//Data
extern int total_num_actors;
extern uint16 word_2E17E;
extern uint16 word_2E22A;
extern uint16 word_2E236;
extern uint16 word_2E228;
extern uint16 word_2E220;
extern uint16 word_2E238;
extern uint16 word_2E21E;
extern uint16 word_2E24C;
extern uint16 word_2E23C;
extern uint16 word_2E232;
extern uint16 word_2E234;
extern uint16 word_2E246;

extern uint8 byte_32EB8;

extern uint16 word_32EC2;

extern uint8 energy_beam_enabled_flag;

extern uint16 word_2E1E4;

extern uint16 word_2E1E2;

extern uint16 brightness_effect_enabled_flag;
extern uint16 obj_switch_151_flag;


extern uint8 move_platform_flag;
extern uint16 actor_tile_display_func_index;

int is_sprite_on_screen(int actorInfoIndex, int frame_num, int x_pos, int y_pos);
BlockingType sprite_blocking_check(int blocking_dir, int actorInfoIndex, int frame_num, int x_pos, int y_pos);

void actor_add_new(int image_index, int x_pos, int y_pos);
int actor_init(int actor_num, int image_index, int x_pos1, int y_pos1);
void load_actor(int actor_num, int actorType, int x_pos, int y_pos);
void actor_load_tiles();

void display_actor_sprite_maybe(int actorInfoIndex, int frame_num, int x_pos, int y_pos, int tile_display_func_index);

uint16 sub_1106F();

void actor_update_all();

TileInfo *actor_get_tile_info(int actorInfoIndex, int frame_num);

ActorData *get_actor(uint16 actor_num);

#endif //COSMO_ENGINE_ACTOR_H