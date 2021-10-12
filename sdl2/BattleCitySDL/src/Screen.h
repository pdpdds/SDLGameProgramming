#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <string>

using namespace std;

class Screen {
public:
    void Init();
    void Set(int width, int height, bool fullscreen) {
        screenWidth = width;
        screenHeight = height;
        isFullscreen = fullscreen;
        windowTitle = "Battle City Remake";
    }
    int WindowWidth() { return screenWidth; }
    int WindowHeight() { return screenHeight; }
    string WindowTitle() { return windowTitle; }

private:
    int screenWidth;
    int screenHeight;
    bool isFullscreen;
    string windowTitle;

public:
    SDL_Window *currentWindow;
    SDL_GLContext openglContext;
};

#endif // WINDOW_H
