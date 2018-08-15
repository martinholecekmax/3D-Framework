#pragma once

#include "Vertex.h"
#include "Vector3D.h"
#include "Polygon3D.h"
#include "Matrix4x4.h"
#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "ReflectionCoefficients.h"
#include "UVCoordinates.h"
#include "Texture.h"

#include <vector>
#include <algorithm>
#include<iostream>

class Model
{
public:
	Model();
	~Model();

	// Accessors
	std::vector<Polygon3D>& GetPolygons();
	std::vector<Vertex>& GetVertices();
	std::vector<UVCoordinates>& GetUVCoordinates();
	size_t GetPolygonCount() const;
	size_t GetVertexCount() const;
	Texture& GetTexture();

	void AddVertex(float x, float y, float z);
	void AddPolygon(int i0, int i1, int i2, int uvIndex0, int uvIndex1, int uvIndex2);
	void AddTextureUV(float u, float v);

	// Apply Transformation on the model loaded from file
	void ApplyTransformToLocalVertices(const Matrix4x4 &transform);
	void ApplyTransformToTransformedVertices(const Matrix4x4 &transform);

	// Backface Culling and depth sorting
	void DehomogenizeModel();
	void CalculateBackFaces(Vertex cameraPosition);
	void Sort();

	// Lights Calculations
	void CalculateLightAmbient(AmbientLight ambientLight);
	void CalculateLightDirectional(const std::vector<DirectionalLight> directionalLight);
	void CalculateLightDirectionalSmooth(const std::vector<DirectionalLight> directionalLight);
	void CalculateLightingPoint(const std::vector<PointLight> pointLight);

	// Calculate normal of each vertex
	void CalculateNormalVertices();

private:
	std::vector<Vertex> _vertices;
	std::vector<Vertex> _transformedVertices;
	std::vector<Polygon3D> _polygons;
	std::vector<UVCoordinates> _uvCoordinates;
	ReflectionCoefficients _reflectanceCoefficients;
	Texture _texture;
};

