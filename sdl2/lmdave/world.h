// world/level state
#ifndef __WORLD_H
#define __WORLD_H

void W_StartLevel();
void W_ResetLevel();
uint8_t W_IsClear( int16_t px, int16_t py, uint8_t is_player );
uint8_t W_IsVisible( int16_t px );
void W_Update();
void W_ScrollView();

#endif
