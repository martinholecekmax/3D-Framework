#pragma once
#include "Vector3D.h"
#include "Color.h"

class DirectionalLight
{
public:
	DirectionalLight();
	DirectionalLight(int intensityRed, int intensityGreen, int intensityBlue, Vector3D direction);
	DirectionalLight(Color lightIntensity, Vector3D direction);
	~DirectionalLight();

	// Accessors
	Vector3D GetDirection() const;
	Color GetLightIntensity() const;

	// Mutators
	void SetLightIntensity(const Color lightIntensity);
	void SetDirection(const Vector3D);

	DirectionalLight& operator= (const DirectionalLight& directionalLight);

private:
	Vector3D _direction;
	Color _lightIntensity;
};

