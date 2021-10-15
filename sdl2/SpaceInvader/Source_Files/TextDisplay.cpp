#include "TextDisplay.h"
#include "Game.h"

TextDisplay* TextDisplay::s_pInstance = 0;

TextDisplay::TextDisplay()
{
	TTF_Init();
}

TextDisplay::~TextDisplay()
{

}

TextDisplay* TextDisplay::Instance()
{
	if (s_pInstance == 0)
	{
		s_pInstance = new TextDisplay();
		return s_pInstance;
	}
	return s_pInstance;
}

SDL_Texture* TextDisplay::renderText(const std::string &message, const std::string &fontFile,
	SDL_Color color, int fontSize, SDL_Renderer *renderer)
{
	TTF_Font *font = TTF_OpenFont(fontFile.c_str(), fontSize);
	if (font == nullptr) {
		return nullptr;
	}

	SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
	if (surf == nullptr) {
		TTF_CloseFont(font);
		return nullptr;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == nullptr) {
	}

	SDL_FreeSurface(surf);
	TTF_CloseFont(font);
	return texture;
}
