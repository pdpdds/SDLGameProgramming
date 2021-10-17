/*
 * This file is part of the 'Simple Sokoban' project.
 *
 * Copyright (C) Mateusz Viste 2014
 *
 * ----------------------------------------------------------------------
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
 */

#ifndef sok_core_h_sentinel
#define sok_core_h_sentinel

  #define field_floor 1
  #define field_atom 2
  #define field_goal 4
  #define field_wall 8

  struct sokgame {
    int field_width;
    int field_height;
    unsigned char field[64][64];
    int positionx;
    int positiony;
    int level;
    unsigned long crc32;
    char *solution;
  };

  struct sokgamestates {
    int angle;
    char *history;
    long historyallocsize;
  };

  enum SOKMOVE {
    sokmoveUP = 1,
    sokmoveLEFT = 2,
    sokmoveDOWN = 3,
    sokmoveRIGHT = 4
  };

  #define sokmove_pushed 1
  #define sokmove_ongoal 2
  #define sokmove_solved 4

  /* loads a level file. returns the amount of levels loaded on success, a non-positive value otherwise. */
  int sok_loadfile(struct sokgame **game, int maxlevels, char *gamelevel, unsigned char *memptr, long filelen, char *comment, int maxcommentlen);

  void sok_freefile(struct sokgame **gamelist, int gamescount);

  /* checks if the game is solved. returns 0 if the game is not solved, non-zero otherwise. */
  int sok_checksolution(struct sokgame *game, struct sokgamestates *states);

  /* try to move the player in a direction. returns a negative value if move has been denied, or a sokmove bitfield otherwise. */
  int sok_move(struct sokgame *game, enum SOKMOVE dir, int validitycheck, struct sokgamestates *states);

  /* undo last move */
  void sok_undo(struct sokgame *game, struct sokgamestates *states);

  /* returns the number of moves in a history string */
  long sok_history_getlen(char *history);

  /* returns the number of pushes in a history string */
  long sok_history_getpushes(char *history);

  /* reset game's states */
  void sok_resetstates(struct sokgamestates *states);

  /* initialize a states structure, and return a pointer to it */
  struct sokgamestates *sok_newstates(void);

  /* free the memory occupied by a previously allocated states structure */
  void sok_freestates(struct sokgamestates *states);

  /* reloads solutions for all levels in a list */
  void sok_loadsolutions(struct sokgame **gamelist, int levelscount);

  /* returns a human string for error code */
  char *sok_strerr(int errid);

  /* plays a string of moves */
  void sok_play(struct sokgame *game, struct sokgamestates *states, char *playfile);

#endif
