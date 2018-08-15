#pragma once
class ReflectionCoefficients
{
public:
	ReflectionCoefficients();
	~ReflectionCoefficients();

	void SetAmbientReflectanceCoefficients(const float kd_red, const float kd_green, const float kd_blue);
	void SetDiffuseReflectanceCoefficients(const float kd_red, const float kd_green, const float kd_blue);
	void SetSpecularReflectanceCoefficients(const float kd_red, const float kd_green, const float kd_blue);

	const float* GetAmbientReflectanceCoefficients() const;
	const float* GetDiffuseReflectanceCoefficients() const;
	const float* GetSpecularReflectanceCoefficients() const;
private:
	float ka[3];
	float kd[3];
	float ks[3];
};

