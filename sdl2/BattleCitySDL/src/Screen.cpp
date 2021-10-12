#include <iostream>
#include "Screen.h"
#include "Game.h"

using namespace std;

void Screen::Init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl;
    else
        cout << "- Initialize SDL." << endl;

    if (isFullscreen) {
        currentWindow = SDL_CreateWindow(windowTitle.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            screenWidth, screenHeight,
            SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
    }
    else {
        currentWindow = SDL_CreateWindow(windowTitle.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            screenWidth, screenHeight,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    }

    openglContext = SDL_GL_CreateContext(currentWindow);
    cout << "- Initialize OpenGL." << endl;

    glViewport(0, 0, screenWidth, screenHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 5);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Disable Vsync
    SDL_GL_SetSwapInterval(0);
    cout << "- Disable Vsync" << endl;

    // glClearColor(1.f, 1.f, 1.f, 1.f);
    glClearColor(0.455f, 0.455f, 0.455f, 1.f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    cout << "- Enable Depth Test" << endl;
    cout << "- Enable Alpha Test" << endl;

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 0.5);
}
