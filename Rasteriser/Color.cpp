#include "Color.h"

Color::Color()
{
}

Color::Color(int red, int green, int blue)
{
	_color = RGB(red, green, blue);
}

Color::Color(COLORREF color)
{
	_color = color;
}


Color::~Color()
{
}

int Color::GetRed() const
{
	return (_color & 0xFF);
}

int Color::GetGreen() const
{
	return ((_color >> 8) & 0xFF);
}

int Color::GetBlue() const
{
	return ((_color >> 16) & 0xFF);
}

COLORREF Color::GetColor() const
{
	return _color;
}
