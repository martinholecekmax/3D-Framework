#pragma once
#include <windows.h>

class Color
{
public:
	Color();
	Color(int red, int green, int blue);
	Color(COLORREF color);
	~Color();

	int GetRed() const;
	int GetGreen() const;
	int GetBlue() const;
	COLORREF GetColor() const;

private:
	COLORREF _color;
};

