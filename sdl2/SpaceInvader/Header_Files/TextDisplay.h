#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class TextDisplay
{
private:
	TextDisplay();
	static TextDisplay* s_pInstance;
	SDL_Texture* tex = NULL;
	int m_x, m_y;
public:
	~TextDisplay();
	
	//Get singleton instance of TextDisplay
	static TextDisplay* Instance();

	//Create texture from string message
	SDL_Texture* renderText(const std::string &message, const std::string &fontFile, SDL_Color color, int fontSize, SDL_Renderer *renderer);
};
