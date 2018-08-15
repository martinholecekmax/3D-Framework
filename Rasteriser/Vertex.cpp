#include "Vertex.h"

Vertex::Vertex()
{
	_x = 0.0f;
	_y = 0.0f;
	_z = 0.0f;
	_w = 1.0f;
	_normal = Vector3D();
	_color = RGB(0, 0, 0);
	_UVIndex = 0;
}

Vertex::Vertex(float x, float y, float z)
{
	_x = x;
	_y = y;
	_z = z;
	_w = 1.0f;
	_normal = Vector3D();
	_color = RGB(0, 0, 0);
	_UVIndex = 0;
}

Vertex::Vertex(float x, float y, float z, float w)
{
	_x = x;
	_y = y;
	_z = z;
	_w = w;
	_normal = Vector3D();
	_color = RGB(0, 0, 0);
	_UVIndex = 0;
}

Vertex::Vertex(float x, float y, float z, Vector3D normal, Color color)
{
	_x = x;
	_y = y;
	_z = z;
	_w = 1.0f;
	_normal = normal;
	_color = color;
	_UVIndex = 0;
}

Vertex::Vertex(float x, float y, float z, float w, Vector3D normal, Color color)
{
	_x = x;
	_y = y;
	_z = z;
	_w = w;
	_normal = normal;
	_color = color;
	_UVIndex = 0;
}

// Copy constructor implementation
// Note that the parameter v is passed by reference, rather than value (that is what
// '&' indicates).  Note also the use of 'const' to indicate that the parameter v cannot
// be changed.

Vertex::Vertex(const Vertex& rhs)
{
	_x = rhs.GetX();
	_y = rhs.GetY();
	_z = rhs.GetZ();
	_w = rhs.GetW();
	_normal = rhs.GetNormal();
	_color = rhs.GetColor();
	_UVIndex = rhs._UVIndex;
}

// Destructor does not need to do anything

Vertex::~Vertex()
{
}

float Vertex::GetX() const
{
	return _x;
}

float Vertex::GetY() const
{
	return _y;
}

float Vertex::GetZ() const
{
	return _z;
}

float Vertex::GetW() const
{
	return _w;
}

Vector3D Vertex::GetNormal() const
{
	return _normal;
}

Color Vertex::GetColor() const
{
	return _color;
}

int Vertex::GetCountRelatedPolygons() const
{
	return _counterRelatedPolygons;
}

int Vertex::GetUVIndex() const
{
	return _UVIndex;
}

void Vertex::SetX(const float x)
{
	_x = x;
}

void Vertex::SetY(const float y)
{
	_y = y;
}

void Vertex::SetZ(const float z)
{
	_z = z;
}

void Vertex::SetW(const float w)
{
	_w = w;
}

void Vertex::SetNormal(const Vector3D normal)
{
	_normal = normal;
}

void Vertex::AddNormal(const Vector3D normal)
{
	_normal += normal;
}

void Vertex::SetColor(const Color color)
{
	_color = color;
}

void Vertex::SetColor(const int red, const int green, const int blue)
{
	_color = RGB(red, green, blue);
}

void Vertex::SetCountRelatedPolygons(const int counter)
{
	_counterRelatedPolygons = counter;
}

void Vertex::AddPolygonCounter()
{
	_counterRelatedPolygons++;
}

void Vertex::SetUVIndex(const int value)
{
	_UVIndex = value;
}

Vertex& Vertex::operator=(const Vertex& rhs)
{
	// Only do the assignment if we are not assigning
	// to ourselves
	if (this != &rhs)
	{
		_x = rhs.GetX();
		_y = rhs.GetY();
		_z = rhs.GetZ();
		_w = rhs.GetW();
		_normal = rhs.GetNormal();
		_color = rhs.GetColor();
		_UVIndex = rhs._UVIndex;
	}
	return *this;
}

// The const at the end of the declaration for '==" indicates that this operation does not change
// any of the member variables in this class.

bool Vertex::operator==(const Vertex& rhs) const
{
	return (_x == rhs.GetX() && _y == rhs.GetY() && _z == rhs.GetZ() && _w == rhs.GetW());
}

Vertex & Vertex::operator+=(const Vertex & rhs)
{
	_x += rhs._x;
	_y += rhs._y;
	_z += rhs._z;
	_w += rhs._w;
	_UVIndex = rhs._UVIndex;

	return *this;
}

// You can see three different uses of 'const' here:
//
// The first const indicates that the method changes the return value, but it is not moved in memory
// The second const indicates that the parameter is passed by reference, but it is not modified
// The third const indicates that the operator does not change any of the memory variables in the class.

const Vertex Vertex::operator+(const Vertex& rhs) const
{
	return Vertex(_x + rhs.GetX(), _y + rhs.GetY(), _z + rhs.GetZ() , _w + rhs.GetW());
}

const Vector3D Vertex::operator-(const Vertex & rhs) const
{
	return Vector3D(_x - rhs.GetX(), _y - rhs.GetY(), _z - rhs.GetZ());
}

const Vertex Vertex::operator*(const float & rhs) const
{
	return Vertex(_x * rhs, _y * rhs, _z * rhs, _w * rhs);
}

const Vertex Vertex::operator/(const float rhs) const
{
	return Vertex(_x / rhs, _y / rhs, _z / rhs, _w / rhs);
}

void Vertex::Dehomogenize()
{
	if (_w == 0) 
	{
		return;
	}

	_x = _x / _w;
	_y = _y / _w;
	_z = _z / _w;
	_w = 1.0f;
}