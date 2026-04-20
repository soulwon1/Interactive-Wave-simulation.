#pragma once
#include "EFieldForce.h"

int intensity(Vec2D vec) {
	double mag = sqrt(vec.getX() * vec.getX() + vec.getY() * vec.getY()); 
	if (mag > 255) mag = 255;
	return mag;
}
double fArSize(Vec2D vec) {
	double mag = sqrt(vec.getX() * vec.getX() + vec.getY() * vec.getY());
	double length = log(1 + mag);
	double scale = 2e6;
	length *= scale;
	if (length > 200) length = 200;
	return length;
}
double vArSize(Vec2D vec) {
	double mag = sqrt(vec.getX() * vec.getX() + vec.getY() * vec.getY());
	double length = log(1 + mag);
	double scale = 6;
	length *= scale;
	if (length > 200) length = 200;
	return length;
}

