#include "StateParser.h"
#include "SpriteManager.h"
#include "Game.h"
#include "GameObjectFactory.h"

bool StateParser::parseState(const char* stateFile, std::string stateID, std::vector<GameObject* > *pObjects,
							std::vector<std::string> *pSpriteIDs)
{
	std::cout << stateFile << " OPENED. POPULATE " << stateID << " STATE" << std::endl;
	TiXmlDocument xmlDoc;
	if (!xmlDoc.LoadFile(stateFile))
	{
		std::cerr << xmlDoc.ErrorDesc() << std::endl;
		return false;
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

	TiXmlElement* pSpriteRoot = 0;
	for (TiXmlElement* e = pStateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
	{
		if (e->Value() == std::string("TEXTURES"))
		{
			pSpriteRoot = e;
		}
	}
	parseSprites(pSpriteRoot, pSpriteIDs);


	TiXmlElement* pObjectRoot = 0;
	for (TiXmlElement* e = pStateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
	{
		if (e->Value() == std::string("OBJECTS"))
		{
			pObjectRoot = e;
		}
	}
	parseObjects(pObjectRoot, pObjects);

	return true;
}

void StateParser::parseSprites(TiXmlElement* pStateRoot, std::vector<std::string> *pSpriteIDs)
{
	for (TiXmlElement* e = pStateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
	{
		std::string filenameAttribute = e->Attribute("filename");
		std::string idAttribute = e->Attribute("ID");
		pSpriteIDs->push_back(idAttribute);

		SpriteManager::instance()->load("Assets/"+filenameAttribute, idAttribute, Game::Instance()->getRenderer());

	}
}

void StateParser::parseObjects(TiXmlElement* pStateRoot, std::vector<GameObject*> *pObjects)
{
	for (TiXmlElement* e = pStateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
	{
		int x, y, width, height, numFrames, callbackID, animSpeed;
		std::string spriteID;

		e->Attribute("x", &x);
		e->Attribute("y", &y);
		e->Attribute("width", &width);
		e->Attribute("height", &height);
		e->Attribute("numFrames", &numFrames);
		e->Attribute("callbackID", &callbackID);
		e->Attribute("animSpeed", &animSpeed);
		spriteID = e->Attribute("textureID");
		GameObject* pGameObject = GameObjectFactory::Instance()->create(e->Attribute("type"));
		if (pGameObject != NULL)
		{
			pGameObject->load(new LoaderParams(x, y, width, height, spriteID, numFrames, callbackID, animSpeed));
			pObjects->push_back(pGameObject);
		}
	}
}
