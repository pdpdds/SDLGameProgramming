/*
 * file2c transforms a data file into C code.
 * Copyright (C) Mateusz Viste 2014
 *
 * file2c adds a NULL character to the end of the file.
 * this is not a bug (you will notice that this extra
 * character is ont counted in the declared length anyway).
 * This allows to use file2c on text files, and read them
 * until \0.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* needed for strdup() */

/* creates a suitable variable name from a filename (replacing all invalid chars by underscores) */
char *filename2varname(char *filename) {
  char *result;
  int x;
  result = strdup(filename);
  for (x = 0; result[x] != 0; x++) {
    if ((result[x] >= '0') && (result[x] <= '9') && (x > 0)) continue;
    if ((result[x] >= 'a') && (result[x] <= 'z')) continue;
    if ((result[x] >= 'A') && (result[x] <= 'Z')) continue;
    result[x] = '_';
  }
  return(result);
}

int main(int argc, char **argv) {
  FILE *fd;
  int bytebuff, column = 1;
  long bytecount = 0;
  char *varname, *filename;
  if ((argc != 2) || (argv[1][0] == '-')) {
    puts("file2c transforms a data file into C code. Copyright (C) Mateusz Viste 2014");
    puts("Usage: file2c file.dat");
    return(1);
  }
  filename = argv[1];
  varname = filename2varname(filename);
  if (varname == NULL) {
    puts("Error: unable to parse filename.");
    return(2);
  }
  fd = fopen(filename, "rb");
  if (fd == NULL) {
    printf("Error: failed to open '%s'.\n", argv[1]);
    return(3);
  }
  printf("unsigned char %s[] = {\n", varname);
  for (;;) {
    bytebuff = getc(fd);
    if (bytebuff < 0) break;
    if (bytecount > 0) printf(",");
    if (column > 15) {
      column = 1;
      printf("\n");
    }
    column += 1;
    bytecount += 1;
    printf("0x%02X", bytebuff);
  }
  if (bytecount > 0) printf(",");
  printf("0x00};\n");
  printf("long %s_len = %ldl;\n", varname, bytecount);
  fclose(fd);
  free(varname);
  return(0);
}
