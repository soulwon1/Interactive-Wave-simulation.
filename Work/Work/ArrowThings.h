#pragma once
#include "EFieldForce.h"
#include "Classes.h"
#include <cmath>
using namespace System::Drawing;

int intensity(Vec2D vec) {
	double mag = sqrt(vec.getX() * vec.getX() + vec.getY() * vec.getY()); 
	if (mag > 255) mag = 255;
	return mag;
}
double arSize(Vec2D vec, double maxSize, double scale) {
	double mag = sqrt(vec.getX() * vec.getX() + vec.getY() * vec.getY());
	double length;
	if (mag > 1000) { length = log(1 + mag); }
	else length = mag;
	length *= scale;
	if (length > maxSize) length = maxSize;
	return length;
}
void DrawArrow(Graphics^ g, double x, double y, double theta, double d, double size, Pen^ pen) {

	double hyp = size * 1.2;
	double arrowSize = size * 0.5;
	double x1 = x + d / 2; //to have the arrow starting at center of circle
	double y1 = y + d / 2;
	double x2 = x1 + hyp * cos(theta);
	double y2 = y1 + hyp * sin(theta);

	double arrowAngle = 3.14159 / 6;
	double xar1 = x2 - arrowSize * cos(theta - arrowAngle);
	double yar1 = y2 - arrowSize * sin(theta - arrowAngle);
	double xar2 = x2 - arrowSize * cos(theta + arrowAngle);
	double yar2 = y2 - arrowSize * sin(theta + arrowAngle);

	g->DrawLine(pen, (float)x1, (float)y1, (float)x2, (float)y2);
	g->DrawLine(pen, (float)x2, (float)y2, (float)xar1, (float)yar1);
	g->DrawLine(pen, (float)x2, (float)y2, (float)xar2, (float)yar2);
}

