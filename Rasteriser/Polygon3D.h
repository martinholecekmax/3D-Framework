#pragma once
#include <windows.h>
#include "Vector3D.h"

class Polygon3D
{
public:
	Polygon3D();
	Polygon3D(int index0, int index1, int index2);
	Polygon3D(int index0, int index1, int index2, int uvIndex0, int uvIndex1, int uvIndex2);
	Polygon3D(const Polygon3D& rhs);
	~Polygon3D();
	
	// Accessors
	int GetIndex(int value) const;
	int GetUVIndex(int value) const;
	bool GetCullFlag() const;
	float GetDepthValue() const;
	COLORREF GetColor() const;
	Vector3D GetNormal() const;

	// Mutators
	void SetCullFlag(const bool value);
	void SetDepthValue(const float depthAverage);
	void SetColor(const int R, const int G, const int B);
	void SetColor(const COLORREF color);
	void SetNormal(const Vector3D normal);

	Polygon3D& operator= (const Polygon3D &rhs);

	// Used for std::sort function in model class
	bool operator<(Polygon3D& rhs) {
		return _depthAverage > rhs._depthAverage;
	}

private:
	int _indices[3];
	int _uvIndices[3];
	float _depthAverage;
	bool _cullFlag;
	COLORREF _color;
	Vector3D _normal;
};

