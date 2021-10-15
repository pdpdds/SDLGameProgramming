#include "Vector2D.h"

Vector2D::Vector2D(float x, float y) :m_fX(x), m_fY(y)
{

}

Vector2D::Vector2D() : m_fX(0), m_fY(0)
{

}

Vector2D::~Vector2D()
{

}

float Vector2D::getX()
{
	return m_fX;
}

float Vector2D::getY()
{
	return m_fY;
}

void Vector2D::setX(float x)
{
	m_fX = x;
}

void Vector2D::setY(float y)
{
	m_fY = y;
}

float Vector2D::length()
{
	return sqrt((m_fX*m_fX) + (m_fY*m_fY));
}
