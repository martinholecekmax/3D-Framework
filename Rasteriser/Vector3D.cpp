#include "Vector3D.h"

Vector3D::Vector3D()
{
	_x = 0.0f;
	_y = 0.0f;
	_z = 0.0f;
}

Vector3D::Vector3D(float x, float y, float z)
{
	_x = x;
	_y = y;
	_z = z;
}

Vector3D::Vector3D(const Vector3D & vec)
{
	_x = vec._x;
	_y = vec._y;
	_z = vec._z;
}

Vector3D::~Vector3D()
{
}

float Vector3D::Length()
{
	return (float)sqrt(_x * _x + _y * _y + _z * _z);
}

Vector3D & Vector3D::Normalize()
{
	const float len = this->Length();

	if (len == 0.0f)
	{
		return *this;
	}

	_x /= len;
	_y /= len;
	_z /= len;

	return *this;
}

Vector3D Vector3D::GetNormalizeVec() const
{
	Vector3D vec = *this;
	vec.Normalize();

	return vec;
}

Vector3D Vector3D::Cross(const Vector3D & lhs, const Vector3D & rhs)
{
	return Vector3D(
		lhs._y * rhs._z - lhs._z * rhs._y,
		lhs._z * rhs._x - lhs._x * rhs._z,
		lhs._x * rhs._y - lhs._y * rhs._x);
}

float Vector3D::DotProduct(const Vector3D & lhs, const Vector3D & rhs)
{
	return lhs._x * rhs._x + lhs._y * rhs._y + lhs._z * rhs._z;
}

float Vector3D::GetX() const
{
	return _x;
}

float Vector3D::GetY() const
{
	return _y;
}

float Vector3D::GetZ() const
{
	return _z;
}

void Vector3D::SetX(const float x)
{
	_x = x;
}

void Vector3D::SetY(const float y)
{
	_y = y;
}

void Vector3D::SetZ(const float z)
{
	_z = z;
}

Vector3D & Vector3D::operator=(const Vector3D & rhs)
{
	if (this != &rhs)
	{
		_x = rhs.GetX();
		_y = rhs.GetY();
		_z = rhs.GetZ();
	}
	return *this;
}

const Vector3D Vector3D::operator-(const Vector3D & rhs) const
{
	return Vector3D(_x - rhs.GetX(), _y - rhs.GetY(), _z - rhs.GetZ());
}

const Vector3D Vector3D::operator+(const Vector3D & rhs) const
{
	return Vector3D(_x + rhs.GetX(), _y + rhs.GetY(), _z + rhs.GetZ());
}

Vector3D & Vector3D::operator+=(const Vector3D & rhs)
{
	_x += rhs._x;
	_y += rhs._y;
	_z += rhs._z;

	return *this;
}

const Vector3D Vector3D::operator/(const float rhs) const
{
	return Vector3D(_x / rhs, _y / rhs, _z / rhs);
}

Vector3D & Vector3D::operator/=(const float rhs)
{
	_x /= rhs;
	_y /= rhs;
	_z /= rhs;

	return * this;
}

