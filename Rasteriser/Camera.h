#pragma once
#include "Vertex.h"
#include "Matrix4x4.h"

class Camera
{
public:
	Camera();
	Camera(float xRotation, float yRotation, float zRotation, const Vertex& position);
	~Camera();

	// Get camera view
	Matrix4x4 GetView();
	Vertex GetPosition();

	// Apply transformations on the camera
	void RotateX(float xRotation);
	void RotateY(float yRotation);
	void RotateZ(float zRotation);
	void SetCameraPosition(Vertex position);

	// Operator
	Camera& operator= (const Camera &camera);

private:
	Matrix4x4 _rotateX;
	Matrix4x4 _rotateY;
	Matrix4x4 _rotateZ;
	Matrix4x4 _cameraView;
	Matrix4x4 _cameraPosition;
	Vertex _position;
};

