#pragma once

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <map>

class SpriteManager
{
private:
	SpriteManager();
	std::map<std::string, SDL_Texture*> m_SpriteMap;
	static SpriteManager* s_pSpriteManager;

public:
	//Get singleton instance
	static SpriteManager* instance();

	~SpriteManager();

	//Load texture into memeory and save in map with 'id' as key
	bool load(std::string fileName, std::string id, SDL_Renderer* renderer);

	//Draw frame at position x,y
	void draw(std::string id, int x, int y, int width, int height, SDL_Renderer* renderer);

	//Draw 'currentFrame' at position x,y
	void drawFrame(std::string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* renderer);
	
	//Unload texture with 'id' key from memory
	void clearFromMemory(std::string id);

	//Add texture to map
	void addTexture(std::string id, SDL_Texture* tex)
	{
		m_SpriteMap[id] = tex;
	}
};
