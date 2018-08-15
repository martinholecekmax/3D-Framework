#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
	_lightIntensity = RGB(0, 0, 0);
	_direction = Vector3D(0.0f, 0.0f, 0.0f);
}

DirectionalLight::DirectionalLight(int intensityRed, int intensityGreen, int intensityBlue, Vector3D direction)
{
	_lightIntensity = RGB(intensityRed, intensityGreen, intensityBlue);
	_direction = direction.Normalize(); 
}

DirectionalLight::DirectionalLight(Color lightIntensity, Vector3D direction)
{
	_lightIntensity = lightIntensity;
	_direction = direction.Normalize();
}


DirectionalLight::~DirectionalLight()
{
}

Vector3D DirectionalLight::GetDirection() const
{
	return _direction;
}

Color DirectionalLight::GetLightIntensity() const
{
	return _lightIntensity;
}

void DirectionalLight::SetLightIntensity(const Color lightIntensity)
{
	_lightIntensity = lightIntensity;
}

void DirectionalLight::SetDirection(const Vector3D direction)
{
	_direction = direction.GetNormalizeVec();
}

DirectionalLight & DirectionalLight::operator=(const DirectionalLight & directionalLight)
{
	_direction = directionalLight._direction;
	_lightIntensity = directionalLight._lightIntensity;

	return *this;
}
