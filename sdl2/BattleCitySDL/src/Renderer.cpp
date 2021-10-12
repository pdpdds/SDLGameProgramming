#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <iostream>
#include "Game.h"
#include "Renderer.h"

using namespace std;

Renderer::~Renderer() {
    for (map<string, Atlas>::iterator iter = m_atlasses.begin(); iter != m_atlasses.end(); ++iter) {
        glDeleteTextures(1, &m_atlasses[iter->first].texture);
        cout << "Remove texture '" << iter->first << "'\n";
    }
}

void Renderer::Init() {
    LoadAtlasFromFile("data/sprite/sprite.bmp", "atlas");
}

void Renderer::LoadAtlasFromFile(const string &fileName, const string &atlasName) {
    Atlas NewAtlas;
    SDL_Surface* temp_surface = SDL_LoadBMP(fileName.c_str());
    if (!temp_surface) {
        cout << "Unable to load sprites file: '" << fileName << "'!\n";
        exit(1);
    }

    NewAtlas.width = temp_surface->w;
    NewAtlas.height = temp_surface->h;

    if (((NewAtlas.width & (NewAtlas.width - 1)) != 0) ||
        ((NewAtlas.height & (NewAtlas.height - 1)) != 0)) {
        cout << "Sprites file size error (" << fileName << ")!" << endl;
        exit(1);
    }

    GLenum format;
    switch (temp_surface->format->BytesPerPixel) {
    case 3:
        format = GL_BGR;
        break;
    case 4:
        format = GL_BGRA;
        break;
    default:
        cout << "File format not recognized (" << fileName << ")!" << endl;
        exit(1);
    }

    glGenTextures(1, &NewAtlas.texture);                                    // Create texture
    glBindTexture(GL_TEXTURE_2D, NewAtlas.texture);                         // Bind texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, temp_surface->format->BytesPerPixel,
        NewAtlas.width, NewAtlas.height, 0, format, GL_UNSIGNED_BYTE, temp_surface->pixels);

    if (temp_surface)
        SDL_FreeSurface(temp_surface);

    InsertAtlas(atlasName, NewAtlas);
    cout << "- Load Sprites file '" << fileName << "'." << endl;
}

void Renderer::StartRendering() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glPushMatrix();
    glBegin(GL_QUADS);
}

void Renderer::StopRendering(SDL_Window *window) {
    glEnd();
    glPopMatrix();

    SDL_GL_SwapWindow(window);
}

void Renderer::DrawSprite(SpriteData &sprite_data, int frame, double scr_x, double scr_y, int width, int height, ColorType color) {
    static int offX = Game::instance().GameScreenOffsetX();
    static int offY = Game::instance().GameScreenOffsetY();
    scr_x += offX;
    scr_y += offY;

    if (color != COLOR_NONE) {
        double r, g, b;
        switch (color) {
        case COLOR_BLACK:       r = 0; g = 0; b = 0; break;
        case COLOR_RED:         r = 1; g = 0; b = 0; break;
        case COLOR_GREEN:       r = 0; g = 1; b = 0; break;
        case COLOR_BLUE:        r = 0; g = 0; b = 1; break;
        }
        glColor3d(r, g, b);
    }

    double left = ((double)sprite_data.left + ((double)sprite_data.width * frame)) / m_atlasses[sprite_data.atlas].width;
    double right = left + ((double)sprite_data.width / (double)m_atlasses[sprite_data.atlas].width);
    double bottom = (double)sprite_data.bottom / m_atlasses[sprite_data.atlas].height;
    double top = bottom - ((double)sprite_data.height / m_atlasses[sprite_data.atlas].height);

    glTexCoord2f((GLfloat)right, (GLfloat)top);       glVertex2f(GLfloat(scr_x + width), GLfloat(scr_y + height));
    glTexCoord2f((GLfloat)left, (GLfloat)top);        glVertex2f(GLfloat(scr_x), GLfloat(scr_y + height));
    glTexCoord2f((GLfloat)left, (GLfloat)bottom);     glVertex2f(GLfloat(scr_x), GLfloat(scr_y));
    glTexCoord2f((GLfloat)right, (GLfloat)bottom);    glVertex2f(GLfloat(scr_x + width), GLfloat(scr_y));

    if (color != COLOR_NONE) {
        glColor3d(1, 1, 1);
    }
}
