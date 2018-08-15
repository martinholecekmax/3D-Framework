#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(float xRotation, float yRotation, float zRotation, const Vertex & position)
{
	_rotateX = Matrix4x4::RotationX(-xRotation);
	_rotateY = Matrix4x4::RotationY(-yRotation);
	_rotateZ = Matrix4x4::RotationZ(-zRotation);
	_cameraPosition = Matrix4x4::Translate(-position.GetX(), -position.GetY(), -position.GetZ());
	_position = position;
	_cameraView =  _rotateX * _rotateY * _rotateZ * _cameraPosition;
}

Camera::~Camera()
{
}

Matrix4x4 Camera::GetView()
{
	_cameraPosition = Matrix4x4::Translate(-_position.GetX(), -_position.GetY(), -_position.GetZ());
	_cameraView = _rotateX * _rotateY * _rotateZ * _cameraPosition;

	return _cameraView;
}

Vertex Camera::GetPosition()
{
	return _position;
}

void Camera::RotateX(float xRotation)
{
	_rotateX = Matrix4x4::RotationX(xRotation) * _rotateX;
}

void Camera::RotateY(float yRotation)
{
	_rotateY = Matrix4x4::RotationY(yRotation) * _rotateY;
}

void Camera::RotateZ(float zRotation)
{
	_rotateZ = Matrix4x4::RotationZ(zRotation) * _rotateZ;
}

void Camera::SetCameraPosition(Vertex position)
{
	_position = position;
}

Camera & Camera::operator=(const Camera & camera)
{
	_rotateX = camera._rotateX;
	_rotateY = camera._rotateY;
	_rotateZ = camera._rotateZ;
	_cameraPosition = camera._cameraPosition;
	_cameraView = camera._cameraView;
	_position = camera._position;

	return *this;
}
