/*
 * png2bmp converts a png file into a bmp file.
 * author: Mateusz Viste
 */

#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>


int main(int argc, char **argv) {
  char *pngfile, imgfile[1024];
  SDL_Surface *surface;

  /* check for correct number of params */
  if (argc != 2) {
    puts("Usage: png2img file.png");
    return(1);
  }
  pngfile = argv[1];
  sprintf(imgfile, "%s", pngfile);
  imgfile[strlen(imgfile) - 4] = 0;
  strcat(imgfile, ".bmp");

  /* load the png file into a surface */
  surface = IMG_Load(pngfile);
  if (surface == NULL) {
    puts("ERROR: Failed to load the png file");
    return(1);
  }
  printf("Loaded %s image into a %dx%d surface with %d bytes per pixel\n", pngfile, surface->w, surface->h, surface->format->BytesPerPixel);

  /* save the surface into BMP */
  SDL_SaveBMP(surface, imgfile);

  return(0);
}
