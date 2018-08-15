#pragma once

#include "Vertex.h"
#include <initializer_list>
#include <math.h>

class Matrix4x4
{
public:
	Matrix4x4();

	Matrix4x4(std::initializer_list<float> list);
	Matrix4x4(const Matrix4x4& rhs);

	~Matrix4x4();

	// Retrieve value in matrix at specified row and column
	float GetM(const int row, const int column) const;

	// Set value in matrix at specified row and column
	void SetM(const int row, const int column, const float value);

	// Copy assignment operator
	Matrix4x4& operator= (const Matrix4x4 &rhs);

	// Multiply two matrices together
	const Matrix4x4 operator* (const Matrix4x4 &rhs) const;

	// Multiply a matrix by a vertex, returning a vertex
	const Vertex operator* (const Vertex &other) const;

	static Matrix4x4 Translate(float x, float y, float z);
	static Matrix4x4 Scaling(float factorX, float factorY, float factorZ);
	static Matrix4x4 RotationX(float angle);
	static Matrix4x4 RotationY(float angle);
	static Matrix4x4 RotationZ(float angle);
	static Matrix4x4 RotationXYZ(float angleX, float angleY, float angleZ);
	static Matrix4x4 Identity();
	static Matrix4x4 ProjectionMatrix(float d, float aspectRatio);
	static Matrix4x4 ScreenTransformationMatrix(float width, float height, float d);

public:
	// [ row ][ col ]
	float _elements[4][4];
};