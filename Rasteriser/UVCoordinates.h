#pragma once
class UVCoordinates
{
public:
	UVCoordinates();
	UVCoordinates(float u, float v);

	~UVCoordinates();

	float GetU() const;
	float GetV() const;
	
	const UVCoordinates operator- (const UVCoordinates& rhs) const;
	UVCoordinates& operator-= (const UVCoordinates& rhs);
	UVCoordinates& operator+= (const UVCoordinates& rhs);
	const UVCoordinates operator+ (const UVCoordinates& rhs) const;
	const UVCoordinates operator/ (const float rhs) const;
	UVCoordinates& operator= (const UVCoordinates& rhs);

private:
	float _u;
	float _v;
};

