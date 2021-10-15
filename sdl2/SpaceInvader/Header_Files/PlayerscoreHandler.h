#pragma once

#include <string>
#include "SDLGameObject.h"
#include "TextDisplay.h"
#include "Game.h"
#include "SpriteManager.h"
#include "tinyxml.h"

class PlayerScoreHandler :public SDLGameObject
{
private:
	SDL_Texture* tex = nullptr;

public:

	//Set player score position to x,y
	PlayerScoreHandler(int posX,int posY)
	{
		m_vPosition = Vector2D(posX, posY);
		Game::Instance()->getGameStateMachine()->getCurrentState()->addGameObject(this);
	}

	//Do render
	virtual void draw()
	{
		if (tex != nullptr)
		{
			int iW, iH;
			SDL_QueryTexture(tex, NULL, NULL, &iW, &iH);
			SpriteManager::instance()->draw("score", m_vPosition.getX(), m_vPosition.getY(), iW, iH, Game::Instance()->getRenderer());
		}
	}

	//Called each frame
	virtual void update()
	{

	}

	//Clean resources and close files
	virtual void clean()
	{

	}

	virtual void load(const LoaderParams* pParams)
	{

	}

	virtual void onCollision()
	{

	}

	//Update and display score
	void updateScore(std::string str)
	{
		SpriteManager::instance()->clearFromMemory("score");
		SDL_Color color = { 255,255,255,255 };
		tex = TextDisplay::Instance()->renderText("SCORE: "+str, "Assets/ca.ttf", color, 15, Game::Instance()->getRenderer());
		SpriteManager::instance()->addTexture("score", tex);
	}

	//Store score in persistent file if score greater than previous high score
	void storeScore(std::string stateID,int newHighScore)
	{
		TiXmlDocument xmlDoc;
		if (!xmlDoc.LoadFile("Assets/test.xml"))
		{
			std::cerr << xmlDoc.ErrorDesc() << std::endl;
			return;
		}

		TiXmlElement* pRoot = xmlDoc.RootElement();

		TiXmlElement* pStateRoot = 0;
		for (TiXmlElement* e = pRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
		{
			if (e->Value() == stateID)
			{
				pStateRoot = e;
			}
		}

		TiXmlElement* pObjectRoot = 0;
		for (TiXmlElement* e = pStateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
		{
			if (e->Value() == std::string("OBJECTS"))
			{
				pObjectRoot = e;
				TiXmlElement* ele = e->FirstChildElement("object")->ToElement();
				std::string prevScore = ele->Attribute("score");
				int score = std::stoi(prevScore);
				if (newHighScore > score)
				{
					ele->SetAttribute("score", newHighScore);
					xmlDoc.SaveFile();
				}
			}
		}
	}
};
