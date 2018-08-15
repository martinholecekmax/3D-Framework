#include "Matrix3x3.h"

Matrix3x3::Matrix3x3()
{
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t y = 0; y < 3; y++)
		{
			_elements[i][y] = 0.0f;
		}
	}
}

Matrix3x3::Matrix3x3(std::initializer_list<float> list)
{
	if (list.size() == 9)
	{
		int row = 0;
		int col = 0;
		for each (auto var in list)
		{
			if (row == 3)
			{
				row = 0;
				col++;
			}
			_elements[row++][col] = var;
		}
	}
}

Matrix3x3::Matrix3x3(const Matrix3x3 & other)
{
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			_elements[i][j] = other._elements[i][j];
		}
	}
}

Matrix3x3::~Matrix3x3()
{
}

float Matrix3x3::GetM(const int row, const int column) const
{
	return _elements[row][column];
}

void Matrix3x3::SetM(const int row, const int column, const float value)
{
	_elements[row][column] = value;
}

Matrix3x3 & Matrix3x3::operator=(const Matrix3x3 & rhs)
{
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			_elements[i][j] = rhs._elements[i][j];
		}
	}

	return *this;
}

const Matrix3x3 Matrix3x3::operator*(const Matrix3x3 & rhs) const
{
	Matrix3x3 result;
	for (size_t j = 0; j < 3; j++)
	{
		for (size_t k = 0; k < 3; k++)
		{
			float sum = 0.0f;
			for (size_t i = 0; i < 3; i++)
			{
				sum += _elements[j][i] * rhs._elements[i][k];
			}
			result._elements[j][k] = sum;
		}
	}
	return result;
}

Matrix3x3 Matrix3x3::Translate(float x, float y)
{
	return {
		1.0f, 0.0f,0.0f,
		0.0f, 1.0f, 0.0f,
		x, y, 1.0f
	};
}

Matrix3x3 Matrix3x3::Scaling(float factorX, float factorY)
{
	return {
		factorX, 0.0f, 0.0f,
		0.0f, factorY, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

Matrix3x3 Matrix3x3::RotationX(float angle)
{
	const float sinAngle = (float)sin(angle);
	const float cosAngle = (float)cos(angle);

	return{
		1.0f, 0.0f, 0.0f,
		0.0f, cosAngle, -sinAngle,
		0.0f, sinAngle, cosAngle
	};
}

Matrix3x3 Matrix3x3::RotationY(float angle)
{
	const float sinAngle = (float)sin(angle);
	const float cosAngle = (float)cos(angle);

	return{
		cosAngle, 0.0f, sinAngle,
		0.0f, 1.0f, 0.0f,
		-sinAngle, 0.0f, cosAngle
	};
}

// 2D Rotation
Matrix3x3 Matrix3x3::RotationZ(float angle)
{
	const float sinAngle = (float)sin(angle);
	const float cosAngle = (float)cos(angle);

	return{
		cosAngle, -sinAngle, 0.0f,
		sinAngle, cosAngle, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

Matrix3x3 Matrix3x3::Identity()
{
	return {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
}

Vertex & operator*=(Vertex & lhs, const Matrix3x3 & rhs)
{
	return lhs = lhs * rhs;
}

Vertex operator*(const Vertex & other, const Matrix3x3 & rhs)
{
	return {
		other.GetX() * rhs._elements[0][0] + other.GetY() * rhs._elements[0][1] + other.GetW() * rhs._elements[0][2],
		other.GetX() * rhs._elements[1][0] + other.GetY() * rhs._elements[1][1] + other.GetW() * rhs._elements[1][2],
		other.GetX() * rhs._elements[2][0] + other.GetY() * rhs._elements[2][1] + other.GetW() * rhs._elements[2][2]
	};
}
