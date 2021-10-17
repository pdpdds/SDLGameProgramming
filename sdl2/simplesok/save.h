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


#ifndef save_h_sentinel
#define save_h_sentinel

/* saves the solution for levcrc32 */
void solution_save(unsigned long levcrc32, char *solution, char *ext);

/* returns a malloc()'ed, null-terminated string with the solution to level levcrc32. if no solution available, returns NULL. */
char *solution_load(unsigned long levcrc32, char *ext);

#endif
