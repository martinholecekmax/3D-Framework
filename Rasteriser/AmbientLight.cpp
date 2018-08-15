#include "AmbientLight.h"

AmbientLight::AmbientLight()
{
	_lightIntensity = RGB(0, 0, 0);
}

AmbientLight::AmbientLight(int intensityRed, int intensityGreen, int intensityBlue)
{
	_lightIntensity = RGB(intensityRed, intensityGreen, intensityBlue);
}

AmbientLight::AmbientLight(Color lightIntensity)
{
	_lightIntensity = lightIntensity;
}

AmbientLight::~AmbientLight()
{
}

Color AmbientLight::GetLightIntensity() const
{
	return _lightIntensity;
}

void AmbientLight::SetLightIntensity(const Color lightIntensity)
{
	_lightIntensity = lightIntensity;
}

AmbientLight & AmbientLight::operator=(const AmbientLight & ambientLight)
{
	_lightIntensity = ambientLight._lightIntensity;

	return *this;
}
