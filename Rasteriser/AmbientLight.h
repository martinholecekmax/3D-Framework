#pragma once
#include "Color.h"

class AmbientLight
{
public:
	AmbientLight();
	AmbientLight(int intensityRed, int intensityGreen, int intensityBlue);
	AmbientLight(Color lightIntensity);
	~AmbientLight();

	// Accessors
	Color GetLightIntensity() const;

	// Mutators
	void SetLightIntensity(const Color lightIntensity);

	// Operator overload - copy the light
	AmbientLight& operator= (const AmbientLight& ambientLight);

private:
	Color _lightIntensity;
};

