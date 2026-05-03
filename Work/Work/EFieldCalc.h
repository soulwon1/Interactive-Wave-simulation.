#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include "Classes.h"
#include "EfieldForce.h"
using namespace std;

//Use R from EFieldForce.h
vector<double> Emagnitude(vector<Charge> Q, double x, double y) {
	vector<double> dist = R(Q, x, y);
	double K = 2.0e6;
	size_t n = Q.size();
	vector<double> EF(n);

	for (int i = 0; i < n; ++i) {
		EF[i] = (K * Q[i].getQ()) / (dist[i] * dist[i]);
	}
	return EF;
}

//Use FDir from EFieldForce, its same

// Function that puts it all together

Vec2D EFieldCalcAt(vector<Charge> Q, double x, double y) {
	Vec2D E;
	size_t n = Q.size();
	vector<double> EF = Emagnitude(Q, x, y);
	vector<double> dir = FDir(Q, x, y);

	//To change coords I need to scale field so it is reasonable per frame
	double Extot = 0;
	double Eytot = 0;
	double scale = 1;

	for (int i = 0; i < n; ++i) {
		Extot += EF[i] * cos(dir[i]);
		Eytot += EF[i] * sin(dir[i]);
	}
	E.setX(Extot * scale);
	E.setY(Eytot * scale);
	return E;
}

//Update coords function. Need to get the dx and dy to add in tick function in forms.
Vec2D dCords(Vec2D& pos, Vec2D vel, Vec2D f, double dt) {

	Vec2D c;
	//normal addition of pos depending on vel : xf = xi + v*t
	c.setX(pos.getX() + vel.getX() * dt);
	c.setY(pos.getY() + vel.getY() * dt);
	return c;
}
Color FieldColor(Vec2D E)
{
	double mag = sqrt(E.getX() * E.getX() + E.getY() * E.getY());

	// VISUAL gain 
	double visualGain = 1;
	mag *= visualGain;

	double s = 1e1; // softness of gradient

	double t = mag / (mag + s);

	// invert so strong field = red
	t = 1.0 - t;

	if (t < 0.0) t = 0.0;
	if (t > 1.0) t = 1.0;

	int r, g, b;

	if (t < 0.5)
	{
		double k = t / 0.5;
		r = (int)(200 * (1.0 - k));
		g = (int)(200 * k);
		b = 0;
	}
	else
	{
		double k = (t - 0.5) / 0.5;
		r = 0;
		g = (int)(200 * (1.0 - k));
		b = (int)(200 * k);
	}

	return Color::FromArgb(r, g, b);
}


