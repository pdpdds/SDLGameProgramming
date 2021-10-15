#pragma once

#include <string>
#include <iostream>

class LoaderParams
{
private:
	int m_iX;
	int m_iY;
	int m_iWidth;
	int m_iHeight;
	int m_iNumOfFrames;
	int m_iCallbackID;
	int m_iAnimSpeed;
	std::string m_strSpriteID;

public:
	LoaderParams(int x, int y, int width, int height, std::string spriteID, int numOfFrames, int callbackID = 0, int animSpeed = 0) :
		m_iX(x), m_iY(y), m_iWidth(width), m_iHeight(height), m_strSpriteID(spriteID), m_iNumOfFrames(numOfFrames),m_iCallbackID(callbackID),m_iAnimSpeed(animSpeed)
	{

	}

	int getX() const
	{
		return m_iX;
	}

	int getY() const
	{
		return m_iY;
	}

	int getWidth() const
	{
		return m_iWidth;
	}

	int getHeight() const
	{
		return m_iHeight;
	}

	int getNumberOfFrames() const
	{
		return m_iNumOfFrames;
	}

	int getCallbackID() const
	{
		return m_iCallbackID;
	}

	int getAnimSpeed() const
	{
		return m_iAnimSpeed;
	}

	std::string getSpriteID() const
	{
		return m_strSpriteID;
	}

};
