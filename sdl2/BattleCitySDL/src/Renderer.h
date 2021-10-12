#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL_opengl.h>
#include <string>
#include <map>
#include "Sprites.h"

using namespace std;

struct Atlas {
    GLuint texture;
    int width, height;
};

class Renderer {
public:
    ~Renderer();

    void Init();
    void LoadAtlasFromFile(const string &fileName, const string &atlasName);

    void StartRendering();
    void StopRendering(SDL_Window *window);
    void DrawSprite(SpriteData &sprite_data, int frame, double scr_x, double scr_y, int width, int height, ColorType color = COLOR_NONE);

    int AtlasWidth(const string &name) { return m_atlasses[name].width; }
    int AtlasHeight(const string &name) { return m_atlasses[name].height; }

private:
    map<string, Atlas> m_atlasses;
    void InsertAtlas(const string &name, Atlas atlas) { m_atlasses.insert(make_pair(name, atlas)); }
};

#endif // RENDERER_H
