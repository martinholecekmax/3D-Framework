#include "Matrix4x4.h"

Matrix4x4::Matrix4x4()
{
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t y = 0; y < 4; y++)
		{
			_elements[i][y] = 0.0f;
		}
	}
}

Matrix4x4::Matrix4x4(std::initializer_list<float> list)
{
	if (list.size() == 16)
	{
		int row = 0;
		int col = 0;
		for each (auto var in list)
		{
			if (row == 4)
			{
				col = 0;
				row++;
			}
			_elements[row][col++] = var;
		}
	}
}

Matrix4x4::Matrix4x4(const Matrix4x4 & rhs)
{
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t y = 0; y < 4; y++)
		{
			_elements[i][y] = rhs._elements[i][y];
		}
	}
}


Matrix4x4::~Matrix4x4()
{
}

float Matrix4x4::GetM(const int row, const int column) const
{
	return _elements[row][column];
}

void Matrix4x4::SetM(const int row, const int column, const float value)
{
	_elements[row][column] = value;
}

Matrix4x4 & Matrix4x4::operator=(const Matrix4x4 & rhs)
{
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t y = 0; y < 4; y++)
		{
			_elements[i][y] = rhs._elements[i][y];
		}
	}

	return *this;
}

const Matrix4x4 Matrix4x4::operator*(const Matrix4x4 & rhs) const
{
	Matrix4x4 result;
	for (size_t j = 0; j < 4; j++)
	{
		for (size_t k = 0; k < 4; k++)
		{
			float sum = 0.0f;
			for (size_t i = 0; i < 4; i++)
			{
				sum += _elements[j][i] * rhs._elements[i][k];
			}
			result._elements[j][k] = sum;
		}
	}
	return result;
}

const Vertex Matrix4x4::operator*(const Vertex & other) const
{
	Vertex vertex = other;

	vertex.SetX(_elements[0][0] * other.GetX()
		+ _elements[0][1] * other.GetY()
		+ _elements[0][2] * other.GetZ()
		+ _elements[0][3] * other.GetW());

	vertex.SetY(_elements[1][0] * other.GetX()
		+ _elements[1][1] * other.GetY()
		+ _elements[1][2] * other.GetZ()
		+ _elements[1][3] * other.GetW());

	vertex.SetZ(_elements[2][0] * other.GetX()
		+ _elements[2][1] * other.GetY()
		+ _elements[2][2] * other.GetZ()
		+ _elements[2][3] * other.GetW());

	vertex.SetW(_elements[3][0] * other.GetX()
		+ _elements[3][1] * other.GetY()
		+ _elements[3][2] * other.GetZ()
		+ _elements[3][3] * other.GetW());

	return vertex;
}

Matrix4x4 Matrix4x4::Translate(float x, float y, float z)
{
	return {
		1.0f, 0.0f, 0.0f, x,
		0.0f, 1.0f, 0.0f, y,
		0.0f, 0.0f, 1.0f, z,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
}

Matrix4x4 Matrix4x4::Scaling(float factorX, float factorY, float factorZ)
{
	return{
		factorX, 0.0f, 0.0f, 0.0f,
		0.0f, factorY, 0.0f, 0.0f,
		0.0f, 0.0f, factorZ, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
}

Matrix4x4 Matrix4x4::RotationX(float angle)
{
	const float sinAngle = (float)sin(angle);
	const float cosAngle = (float)cos(angle);

	return{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cosAngle, -sinAngle, 0.0f,
		0.0f, sinAngle, cosAngle, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
}

Matrix4x4 Matrix4x4::RotationY(float angle)
{
	const float sinAngle = (float)sin(angle);
	const float cosAngle = (float)cos(angle);

	return{
		cosAngle, 0.0f, sinAngle, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sinAngle, 0.0f, cosAngle, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
}

Matrix4x4 Matrix4x4::RotationZ(float angle)
{
	const float sinAngle = (float)sin(angle);
	const float cosAngle = (float)cos(angle);

	return{
		cosAngle, -sinAngle, 0.0f, 0.0f,
		sinAngle, cosAngle, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
}

Matrix4x4 Matrix4x4::RotationXYZ(float angleX, float angleY, float angleZ)
{
	return RotationX(angleX) * RotationY(angleY) * RotationZ(angleZ);
}

Matrix4x4 Matrix4x4::Identity()
{
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
}

Matrix4x4 Matrix4x4::ProjectionMatrix(float d, float aspectRatio)
{
	return {
		(d / aspectRatio), 0.0f, 0.0f, 0.0f,
		0.0f, d, 0.0f, 0.0f,
		0.0f, 0.0f, d, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
	};
}

Matrix4x4 Matrix4x4::ScreenTransformationMatrix(float width, float height, float d)
{
	return {
		(width / 2.0f), 0.0f, 0.0f, (width / 2.0f),
		0.0f, (-height / 2.0f), 0.0f, (height / 2.0f),
		0.0f, 0.0f, (d / 2.0f), (d / 2.0f),
		0.0f, 0.0f, 0.0f, 1.0f,
	};
}
