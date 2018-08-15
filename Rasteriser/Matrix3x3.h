#pragma once

#include <initializer_list>
#include <math.h>
#include "Vertex.h"

class Matrix3x3
{
public:
	Matrix3x3();

	// Constructor that initialises all elements. Note that an initialiser 
	// list is used to pass in the values. This is a new C++ feature.
	Matrix3x3(std::initializer_list<float> list);

	// Copy constructor
	Matrix3x3(const Matrix3x3& other);

	~Matrix3x3();

	// Retrieve value in matrix at specified row and column
	float GetM(const int row, const int column) const;
	
	// Set value in matrix at specified row and column
	void SetM(const int row, const int column, const float value);

	// Copy assignment operator
	Matrix3x3& operator= (const Matrix3x3 &rhs);

	// Multiply two matrices together
	const Matrix3x3 operator* (const Matrix3x3 &other) const;

	// Multiply a matrix by a vertex, returning a vertex
	//const Vertex operator* (const Vertex &other) const;

	// Translate Matrix
	static Matrix3x3 Translate(float x, float y);

	// Scaling Matrix
	static Matrix3x3 Scaling(float factorX, float factorY);

	// Rotate around X-axis
	static Matrix3x3 RotationX(float angle);

	// Rotate around Y-axis
	static Matrix3x3 RotationY(float angle);

	// Rotate around Z-axis (2d Rotation)
	static Matrix3x3 RotationZ(float angle);

	// Identity Matrix
	static Matrix3x3 Identity();

public:
	// [ row ][ col ]
	float _elements[3][3];
};
Vertex& operator*= (Vertex& lhs, const Matrix3x3& rhs);
Vertex operator* (const Vertex & other, const Matrix3x3& rhs);
