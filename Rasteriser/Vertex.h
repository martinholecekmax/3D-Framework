#pragma once
#include "Vector3D.h"
#include "Color.h"

class Vertex
{
public:
	Vertex();
	Vertex(float x, float y, float z);
	Vertex(float x, float y, float z, float w);
	Vertex(float x, float y, float z, Vector3D normal, Color color);
	Vertex(float x, float y, float z, float w, Vector3D normal, Color color);

	// Copy constructor.  
	Vertex(const Vertex& v);

	// Destructor
	~Vertex();

	// Accessors
	float GetX() const;
	float GetY() const;
	float GetZ() const;
	float GetW() const;
	Vector3D GetNormal() const;
	Color GetColor() const;
	int GetCountRelatedPolygons() const;
	int GetUVIndex() const;

	// Muttators
	void SetX(const float x);
	void SetY(const float y);
	void SetZ(const float z);
	void SetW(const float w);
	void SetNormal(const Vector3D normal);
	void AddNormal(const Vector3D normal);
	void SetColor(const Color color);
	void SetColor(const int red, const int green, const int blue);
	void SetCountRelatedPolygons(const int counter);
	void AddPolygonCounter();
	void SetUVIndex(const int value);
	
	// Operators Overloading
	Vertex& operator= (const Vertex& rhs);
	bool operator== (const Vertex& rhs) const;
	Vertex& operator+= (const Vertex& rhs);
	const Vertex operator+ (const Vertex& rhs) const;
	const Vector3D operator- (const Vertex& rhs) const;
	const Vertex operator* (const float& rhs) const;
	const Vertex operator/ (const float rhs) const;

	// Dehomogenize
	void Dehomogenize();

private:
	float _x;
	float _y;
	float _z;
	float _w;
	int _counterRelatedPolygons;
	Vector3D _normal;
	Color _color;
	int _UVIndex;
};

