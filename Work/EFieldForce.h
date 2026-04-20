#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include "Classes.h"
using namespace std;

//Need to calculate force on a point charge. This needs magnitude and direction. 

//maginitude= KQq/r^2


vector<double> EFmagnitude(vector<Charge> Q, Charge q, vector<double> R) {
	double K = 1.0;
	size_t n = Q.size();
	vector<double> EF(n);


	for (int i = 0; i < n; ++i) {
		EF[i] = (K * Q[i].getQ() * q.getQ()) / (R[i] * R[i]);
	}
	return EF;
}

vector<double> R(vector<Charge> Q, Charge q) { // calculate the distance between charges, producing a vector of distances
	size_t n = Q.size();
	double tol = 1;
	vector<double> R(n);
	for (int i = 0; i < n; ++i) {
		R[i] = sqrt((q.getX() - Q[i].getX()) * (q.getX() - Q[i].getX()) + (q.getY() - Q[i].getY()) * (q.getY() - Q[i].getY()));
		if (R[i] < tol) { R[i] = tol; }
	}
	return R;
}

// FOr direction I need just x component and y component in tan(). For multuple charges just have a vector of angles corresponding to each.

vector<double> FDir(vector<Charge> Q, Charge q) {

	size_t n = Q.size();
	vector<double> theta(n);
	for (int i = 0; i < n; ++i) {
		theta[i] = atan2((q.getY() - Q[i].getY()), (q.getX() - Q[i].getX()));
	}
	return theta;
}

// Function that puts it all together

Vec2D ForceCalc(vector<Charge> Q, Charge q) {
	Vec2D f;
	size_t n = Q.size();
	vector<double> r = R(Q, q);
	vector<double> EF = EFmagnitude(Q, q, r);
	vector<double> dir = FDir(Q, q);

	//To change coords I need to scale force so it is reasonable per frame
	double Fxtot = 0;
	double Fytot = 0;
	double scale = 1;

	for (int i = 0; i < n; ++i) {
		Fxtot += EF[i] * cos(dir[i]);
		Fytot += EF[i] * sin(dir[i]);
	}
	f.setX(Fxtot * scale);
	f.setY(Fytot * scale);
	return f;
}

//Update coords function. Need to get the dx and dy to add in tick function in forms.
Vec2D dCords(Charge& q, Vec2D f, double dt) {

	Vec2D c;
	q.addVx((f.getX() / q.getM()) * dt);
	q.addVy((f.getY() / q.getM()) * dt);

	c.setX(q.getVx() * dt);
	c.setY(q.getVy() * dt);
	return c;
}

double testFDir(Charge q, Vec2D f) {
	double theta = atan2(f.getY(), f.getX());
	return theta;
}
double testVDir(Charge q) {
	return atan2(q.getVy(), q.getVx());
}

