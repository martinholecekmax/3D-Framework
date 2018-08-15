#include "PointLight.h"

PointLight::PointLight()
{
	_lightIntensity = RGB(0, 0, 0);
	_position = Vector3D(0.0f, 0.0f, 0.0f);
	_attenuation.a = 0.0f;
	_attenuation.b = 0.0f;
	_attenuation.c = 0.0f;
}

PointLight::PointLight(int intensityRed, int intensityGreen, int intensityBlue, Vector3D position, Attenuation attenuation)
{
	_lightIntensity = RGB(intensityRed, intensityGreen, intensityBlue);
	_position = position;
	_attenuation.a = attenuation.a;
	_attenuation.b = attenuation.b;
	_attenuation.c = attenuation.c;
}

PointLight::PointLight(Color lightIntensity, Vector3D position, Attenuation attenuation)
{
	_lightIntensity = lightIntensity;
	_position = position;
	_attenuation.a = attenuation.a;
	_attenuation.b = attenuation.b;
	_attenuation.c = attenuation.c;
}


PointLight::~PointLight()
{
}

Vector3D PointLight::GetPosition() const
{
	return _position;
}

Color PointLight::GetLightIntensity() const
{
	return _lightIntensity;
}

Attenuation PointLight::GetAttenuation() const
{
	return _attenuation;
}

void PointLight::SetLightIntensity(const Color lightIntensity)
{
	_lightIntensity = lightIntensity;
}

void PointLight::SetPosition(const Vector3D position)
{
	_position = position;
}

void PointLight::SetAttenuation(const Attenuation attenuation)
{
	_attenuation.a = attenuation.a;
	_attenuation.b = attenuation.b;
	_attenuation.c = attenuation.c;
}

PointLight & PointLight::operator=(const PointLight & pointLight)
{
	_lightIntensity = pointLight._lightIntensity;
	_position = pointLight._position;
	_attenuation.a = pointLight._attenuation.a;
	_attenuation.b = pointLight._attenuation.b;
	_attenuation.c = pointLight._attenuation.c;

	return *this;
}
