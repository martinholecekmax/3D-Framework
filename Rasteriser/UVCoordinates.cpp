#include "UVCoordinates.h"

UVCoordinates::UVCoordinates()
{
	_u = 0.0f;
	_v = 0.0f;
}

UVCoordinates::UVCoordinates(float u, float v)
{
	_u = u;
	_v = v;
}


UVCoordinates::~UVCoordinates()
{
	
}

float UVCoordinates::GetU() const
{
	return _u;
}

float UVCoordinates::GetV() const
{
	return _v;
}

const UVCoordinates UVCoordinates::operator-(const UVCoordinates & rhs) const
{
	return UVCoordinates(_u - rhs._u, _v - rhs._v);
}

UVCoordinates & UVCoordinates::operator-=(const UVCoordinates & rhs)
{
	_u -= rhs._u;
	_v -= rhs._v;

	return *this;
}

UVCoordinates & UVCoordinates::operator+=(const UVCoordinates & rhs)
{
	_u += rhs._u;
	_v += rhs._v;

	return *this;
}

const UVCoordinates UVCoordinates::operator+(const UVCoordinates & rhs) const
{
	return UVCoordinates(_u + rhs._u, _v + rhs._v);
}

const UVCoordinates UVCoordinates::operator/(const float rhs) const
{
	return UVCoordinates(_u / rhs, _v / rhs);
}

UVCoordinates & UVCoordinates::operator=(const UVCoordinates & rhs)
{
	if (this != &rhs)
	{
		_u = rhs.GetU();
		_v = rhs.GetV();
	}
	return *this;
}
