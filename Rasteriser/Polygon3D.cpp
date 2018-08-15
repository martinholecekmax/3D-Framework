#include "Polygon3D.h"

Polygon3D::Polygon3D()
{
}

Polygon3D::Polygon3D(int index0, int index1, int index2)
{
	_indices[0] = index0;
	_indices[1] = index1;
	_indices[2] = index2;

	_uvIndices[0] = 0;
	_uvIndices[1] = 0;
	_uvIndices[2] = 0;

	_depthAverage = 0.0f;
	_cullFlag = false;
	_color = RGB(0, 0, 0);
	_normal = Vector3D(0.0f, 0.0f, 0.0f);
}

Polygon3D::Polygon3D(int index0, int index1, int index2, int uvIndex0, int uvIndex1, int uvIndex2)
{
	_indices[0] = index0;
	_indices[1] = index1;
	_indices[2] = index2;

	_uvIndices[0] = uvIndex0;
	_uvIndices[1] = uvIndex1;
	_uvIndices[2] = uvIndex2;

	_depthAverage = 0.0f;
	_cullFlag = false;
	_color = RGB(0, 0, 0);
	_normal = Vector3D(0.0f, 0.0f, 0.0f);
}


Polygon3D::Polygon3D(const Polygon3D & rhs)
{
	_indices[0] = rhs._indices[0];
	_indices[1] = rhs._indices[1];
	_indices[2] = rhs._indices[2];

	_uvIndices[0] = rhs._uvIndices[0];
	_uvIndices[1] = rhs._uvIndices[1];
	_uvIndices[2] = rhs._uvIndices[2];

	_depthAverage = rhs._depthAverage;
	_cullFlag = rhs._cullFlag;
	_color = rhs._color;
	_normal = rhs._normal;
}

Polygon3D::~Polygon3D()
{
}

int Polygon3D::GetIndex(int value) const
{
	return _indices[value];
}

int Polygon3D::GetUVIndex(int value) const
{
	return _uvIndices[value];
}

bool Polygon3D::GetCullFlag() const
{
	return _cullFlag;
}

float Polygon3D::GetDepthValue() const
{
	return _depthAverage;
}

COLORREF Polygon3D::GetColor() const
{
	return _color;
}

Vector3D Polygon3D::GetNormal() const
{
	return _normal;
}

void Polygon3D::SetCullFlag(const bool value)
{
	_cullFlag = value;
}

void Polygon3D::SetDepthValue(const float depth)
{
	_depthAverage = depth;
}

void Polygon3D::SetColor(const int Red, const int Green, const int Blue)
{
	_color = RGB(Red, Green, Blue);
}

void Polygon3D::SetColor(const COLORREF color)
{
	_color = color;
}

void Polygon3D::SetNormal(const Vector3D normal)
{
	_normal = normal;
}

Polygon3D & Polygon3D::operator=(const Polygon3D & rhs)
{
	_indices[0] = rhs._indices[0];
	_indices[1] = rhs._indices[1];
	_indices[2] = rhs._indices[2];

	_uvIndices[0] = rhs._uvIndices[0];
	_uvIndices[1] = rhs._uvIndices[1];
	_uvIndices[2] = rhs._uvIndices[2];

	_depthAverage = rhs._depthAverage;
	_cullFlag = rhs._cullFlag;
	_color = rhs._color;
	_normal = rhs._normal;

	return *this;
}
