#pragma once
#include "Vector3D.h"
#include "Color.h"

struct Attenuation
{
	float a;
	float b; 
	float c;
};

class PointLight
{
public:
	PointLight();
	PointLight(int intensityRed, int intensityGreen, int intensityBlue, Vector3D position, Attenuation attenuation);
	PointLight(Color lightIntensity, Vector3D position, Attenuation attenuation);
	~PointLight();

	// Accessors
	Vector3D GetPosition() const;
	Color GetLightIntensity() const;
	Attenuation GetAttenuation() const;

	// Mutators
	void SetLightIntensity(const Color lightIntensity);
	void SetPosition(const Vector3D position);
	void SetAttenuation(const Attenuation attenuation);

	PointLight& operator= (const PointLight& pointLight);

private:
	Color _lightIntensity;
	Vector3D _position;
	Attenuation _attenuation;
};

