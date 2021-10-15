#pragma once

#include <string>
#include <map>
#include "BaseCreator.h"

class GameObjectFactory
{
private:
	GameObjectFactory();
	std::map<std::string, BaseCreator*> m_creators;
	static GameObjectFactory* s_pInstance;

public:
	//Get singleton instance of GameObjectFactory
	static GameObjectFactory* Instance();

	//Register type to spawn classes after reading XML file
	bool registerType(std::string typeID, BaseCreator* pCreator);

	//Create gameobject if the typeID is a registered type
	GameObject* create(std::string typeID);
};
