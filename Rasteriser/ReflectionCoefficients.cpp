#include "ReflectionCoefficients.h"

ReflectionCoefficients::ReflectionCoefficients()
{
	for (int i = 0; i < 3; i++)
	{
		ka[i] = 0.50f;
		kd[i] = 0.50f;
		ks[i] = 0.50f;
	}
}

ReflectionCoefficients::~ReflectionCoefficients()
{
}

void ReflectionCoefficients::SetAmbientReflectanceCoefficients(const float ka_red, const float ka_green, const float ka_blue)
{
	ka[0] = ka_red;
	ka[1] = ka_green;
	ka[2] = ka_blue;
}

void ReflectionCoefficients::SetDiffuseReflectanceCoefficients(const float kd_red, const float kd_green, const float kd_blue)
{
	kd[0] = kd_red;
	kd[1] = kd_green;
	kd[2] = kd_blue;
}

void ReflectionCoefficients::SetSpecularReflectanceCoefficients(const float ks_red, const float ks_green, const float ks_blue)
{
	ks[0] = ks_red;
	ks[1] = ks_green;
	ks[2] = ks_blue;
}

const float * ReflectionCoefficients::GetAmbientReflectanceCoefficients() const
{
	return ka;
}

const float * ReflectionCoefficients::GetDiffuseReflectanceCoefficients() const
{
	return kd;
}

const float * ReflectionCoefficients::GetSpecularReflectanceCoefficients() const
{
	return ks;
}
