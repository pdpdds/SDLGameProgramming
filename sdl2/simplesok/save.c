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

#include <errno.h>
#include <stdio.h>    /* fopen() */
#include <stdlib.h>   /* malloc(), realloc() */
#include <string.h>   /* strcpy(), strcat() */
#include <SDL2/SDL.h> /* SDL_GetPrefPath(), SDL_free() */

enum solmoves {
  solmove_u = 0,
  solmove_l = 1,
  solmove_d = 2,
  solmove_r = 3,
  solmove_U = 4,
  solmove_L = 5,
  solmove_D = 6,
  solmove_R = 7,
  solmove_ERR = 8
};

static int xsb2byte(char c) {
  switch (c) {
    case 'u':
      return(solmove_u);
    case 'l':
      return(solmove_l);
    case 'd':
      return(solmove_d);
    case 'r':
      return(solmove_r);
    case 'U':
      return(solmove_U);
    case 'L':
      return(solmove_L);
    case 'D':
      return(solmove_D);
    case 'R':
      return(solmove_R);
    default:
      return(solmove_ERR);
  }
}

static char byte2xsb(int b) {
  switch (b) {
    case solmove_u:
      return('u');
    case solmove_l:
      return('l');
    case solmove_d:
      return('d');
    case solmove_r:
      return('r');
    case solmove_U:
      return('U');
    case solmove_L:
      return('L');
    case solmove_D:
      return('D');
    case solmove_R:
      return('R');
    default:
      return('!');
  }
}


/* fills *savedir with the directory path where simplesok is supposed to keep its save files */
static void getsavedir(char *savedir, int maxlen) {
  char *prefpath;
  if (maxlen > 0) savedir[0] = 0;
  maxlen -= 16; /* to be sure we will always have enough room for the app's suffix and extra NULL terminator */
  if (maxlen < 1) return;
  prefpath = SDL_GetPrefPath("Mateusz Viste", "Simple Sokoban");
  if (prefpath == NULL) return;
  if (strlen(prefpath) > (unsigned) maxlen) return;
  strcpy(savedir, prefpath);
  SDL_free(prefpath);
}

/* returns a malloc()'ed, null-terminated string with the solution to level levcrc32. if no solution available, returns NULL. */
char *solution_load(unsigned long levcrc32, char *ext) {
  char rootdir[4096], crcstr[16], *solution, *solutionfinal;
  int bytebuff, rlecounter;
  long solutionpos = 0, solution_alloc = 16;
  FILE *fd;
  getsavedir(rootdir, sizeof(rootdir));
  if (rootdir[0] == 0) return(NULL);
  sprintf(crcstr, "%08lX.%s", levcrc32, ext);
  strcat(rootdir, crcstr);
  fd = fopen(rootdir, "rb");
  if (fd == NULL) return(NULL);

  solution = malloc(solution_alloc);
  if (solution == NULL) return(NULL);
  solution[0] = 0;

  for (;;) {
    bytebuff = getc(fd);
    if (bytebuff < 0) break; /* if end of file -> stop here */
    rlecounter = bytebuff >> 4; /* fetch the RLE counter */
    bytebuff &= 15; /* strip the rle counter, so we have only the actual value */
    while (rlecounter > 0) {
      /* check first if we are in need of reallocation */
      if (solutionpos + 1 >= solution_alloc) {
        solution_alloc *= 2;
        solution = realloc(solution, solution_alloc);
        if (solution == NULL) {
          printf("realloc() failed for %ld bytes: %s\n", solution_alloc, strerror(errno));
          solution = NULL;
          break;
        }
      }
      /* add one position to the solution and decrement the rle counter */
      solution[solutionpos] = byte2xsb(bytebuff);
      if (solution[solutionpos] == '!') { /* if corrupted solution, free it and return nothing */
        free(solution);
        solution = NULL;
        break;
      }
      solutionpos += 1;
      solution[solutionpos] = 0;
      rlecounter -= 1;
    }
    if (solution == NULL) break; /* break out on error */
  }

  /* close the file descriptor */
  fclose(fd);

  /* strdup() the solution to make sure we use the least possible amount of memory */
  solutionfinal = NULL;
  if (solution != NULL) {
    solutionfinal = strdup(solution);
    free(solution);
  }
  /* return the solution string */
  return(solutionfinal);
}

/* saves the solution for levcrc32 */
void solution_save(unsigned long levcrc32, char *solution, char *ext) {
  char rootdir[4096], crcstr[16];
  int curbyte, lastbyte = -1, lastbytecount = 0;
  FILE *fd;
  getsavedir(rootdir, sizeof(rootdir));
  if ((rootdir[0] == 0) || (solution == NULL)) return;
  sprintf(crcstr, "%08lX.%s", levcrc32, ext);
  strcat(rootdir, crcstr);
  fd = fopen(rootdir, "wb");
  if (fd == NULL) return;
  for (;;) {
    curbyte = xsb2byte(*solution);
    if ((curbyte == lastbyte) && (lastbytecount < 15)) {
        lastbytecount += 1; /* same pattern -> increment the RLE counter */
      } else {
        int bytebuff;
        /* dump the lastbyte chunk */
        if (lastbytecount > 0) {
          bytebuff = lastbytecount;
          bytebuff <<= 4;
          bytebuff |= lastbyte;
          fputc(bytebuff, fd);
        }
        /* save the new RLE counter */
        lastbyte = curbyte;
        lastbytecount = 1;
    }
    if (curbyte == solmove_ERR) break;
    solution += 1;
  }
  fclose(fd);
  return;
}
