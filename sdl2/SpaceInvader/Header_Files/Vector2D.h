#pragma once

#include <math.h>

class Vector2D
{
private:
	float m_fX;
	float m_fY;

public:
	Vector2D();
	Vector2D(float x, float y);
	~Vector2D();
	float getX();
	float getY();
	void setX(float x);
	void setY(float y);
	float length();

	Vector2D operator+(const Vector2D& v2)
	{
		return Vector2D(m_fX + v2.m_fX, m_fY + v2.m_fY);
	}

	friend Vector2D& operator+=(Vector2D& v1, const Vector2D& v2)
	{
		v1.m_fX += v2.m_fX;
		v1.m_fY += v2.m_fY;

		return v1;
	}

	Vector2D operator*(float scalar)
	{
		return Vector2D(m_fX*scalar, m_fY*scalar);
	}

	Vector2D& operator*=(float scalar)
	{
		m_fX *= scalar;
		m_fY *= scalar;

		return *this;
	}

	Vector2D operator-(const Vector2D& v2)
	{
		return Vector2D(m_fX - v2.m_fX, m_fY - v2.m_fY);
	}

	friend Vector2D operator-=(Vector2D& v1, const Vector2D& v2)
	{
		v1.m_fX -= v2.m_fX;
		v1.m_fY -= v2.m_fY;

		return v1;
	}

	Vector2D operator/(float scalar)
	{
		return Vector2D(m_fX / scalar, m_fY / scalar);
	}

	Vector2D& operator/=(float scalar)
	{
		m_fX /= scalar;
		m_fY /= scalar;

		return *this;
	}

	void normalize()
	{
		float l = length();
		if (l > 0)
		{
			*this *= 1 / l;
		}
	}

};
