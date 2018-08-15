#pragma once
#include <math.h>

class Vector3D
{
public:
	Vector3D();
	Vector3D(float x, float y, float z);
	Vector3D(const Vector3D& vec);
	~Vector3D();

	// Length (Magnitude)
	float Length();

	// Normalize
	Vector3D& Normalize();
	Vector3D GetNormalizeVec() const;

	// Cross Product
	static Vector3D Cross(const Vector3D & lhs, const Vector3D& rhs);
	
	// Dot Product
	static float DotProduct(const Vector3D& lhs, const Vector3D& rhs);

	// Accessors
	float GetX() const;
	float GetY() const;
	float GetZ() const;

	// Mutators
	void SetX(const float x);
	void SetY(const float y);
	void SetZ(const float z);

	// Equals operator
	Vector3D& operator= (const Vector3D& rhs);

	// Subtract two vectors
	const Vector3D operator- (const Vector3D& rhs) const;

	const Vector3D operator+ (const Vector3D& rhs) const;
	Vector3D& operator+= (const Vector3D& rhs);

	const Vector3D operator/ (const float rhs) const;
	Vector3D& operator/= (const float rhs);

private:
	float _x;
	float _y;
	float _z;
};

