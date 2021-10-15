#pragma once

#include <iostream>
#include <vector>
#include "tinyxml.h"

class GameObject;

class StateParser
{
private:
	void parseObjects(TiXmlElement* pStateRoot, std::vector<GameObject*> *pObjects);
	void parseSprites(TiXmlElement* pStateRoot, std::vector<std::string> *pSpriteIDs);

public :

	//Read and store spriteIDs and objects of state
	bool parseState(const char* stateFile, std::string stateID, std::vector<GameObject* > *pObjects, std::vector<std::string> *pSpriteIDs);

};
