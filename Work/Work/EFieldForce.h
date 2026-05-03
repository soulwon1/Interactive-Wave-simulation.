#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include "Classes.h"
using namespace std;

//Need to calculate force on a point charge. This needs magnitude and direction. 

//maginitude= KQq/r^2

vector<double> R(vector<Charge> Q, double x, double y) { // calculate the distance between charges, producing a vector of distances
	size_t n = Q.size();
	double tol = 1;
	vector<double> R(n);
	for (int i = 0; i < n; ++i) {
		R[i] = sqrt((x - Q[i].getX()) * (x - Q[i].getX()) + (y - Q[i].getY()) * (y - Q[i].getY()));
		if (R[i] < tol) { R[i] = tol; }
	}
	return R;
}
vector<double> EFmagnitude(vector<Charge> Q, double x, double y, Charge q) {
	vector<double> dist = R(Q, x, y);
	double K = 1.0;
	size_t n = Q.size();
	vector<double> EF(n);

	for (int i = 0; i < n; ++i) {
		EF[i] = (K * Q[i].getQ() * q.getQ()) / (dist[i] * dist[i]);
	}
	return EF;
}

// FOr direction I need just x component and y component in tan(). For multuple charges just have a vector of angles corresponding to each.

vector<double> FDir(vector<Charge> Q, double x, double y) {

	size_t n = Q.size();
	vector<double> theta(n);
	for (int i = 0; i < n; ++i) {
		theta[i] = atan2((y - Q[i].getY()), (x - Q[i].getX()));
	}
	return theta;
}

// Function that puts it all together

Vec2D ForceCalc(vector<Charge> Q, double x, double y, Charge q) {
	Vec2D f;
	size_t n = Q.size();
	vector<double> EF = EFmagnitude(Q, x, y, q);
	vector<double> dir = FDir(Q, x, y);

	//To change coords I need to scale force so it is reasonable per frame
	double Fxtot = 0;
	double Fytot = 0;
	double scale = 0.05;

	for (int i = 0; i < n; ++i) {
		Fxtot += EF[i] * cos(dir[i]);
		Fytot += EF[i] * sin(dir[i]);
	}
	f.setX(Fxtot * scale);
	f.setY(Fytot * scale);
	return f;
}

//Update coords function. Need to get the dx and dy to add in tick function in forms.
Vec2D dqCords(Charge& q, Vec2D f, double dt) {

	Vec2D c;
	q.addVx((f.getX() / q.getM()) * dt);
	q.addVy((f.getY() / q.getM()) * dt);

	c.setX(q.getVx() * dt);
	c.setY(q.getVy() * dt);
	return c;
}

double totFDir( Vec2D f) {
	double theta = atan2(f.getY(), f.getX());
	return theta;
}
double testVDir(Charge q) {
	return atan2(q.getVy(), q.getVx());
}

Vec2D forceTwoQ(Charge Q1, Charge Q2) {
	Vec2D force;
	double dx = Q1.getX() - Q2.getX();
	double dy = Q1.getY() - Q2.getY();
	double r = sqrt(dx * dx + dy * dy);
	//if (r < Q1.getDiameter() / 2) r = Q1.getDiameter() / 2;
	double eps = Q1.getDiameter() / 2;
	r = sqrt(r * r + eps * eps);
	//if (r < 1) r = 1;
	double k = 1.0;
	double mag = k * (Q1.getQ() * Q2.getQ()) / (r * r);
	double dir = atan2(Q1.getY() - Q2.getY(), Q1.getX() - Q2.getX());

	double scale = 5e6;
	double Fx = mag * (dx/r); //having to use unit veector since angle was breaking when neg q and pos q got too close
	double Fy = mag * (dy/r);

	force = Vec2D(Fx*scale, Fy*scale);
	return force;
}

void allQscreenCollision(vector<Charge>& Q, int screenWidth, int screenHeight) {

	for (int i = 0; i < Q.size(); ++i) {
		if (Q[i].getY() >= screenHeight - Q[i].getDiameter()/2) Q[i].setY(screenHeight-Q[i].getDiameter() / 2);
		if (Q[i].getX() >= screenWidth - Q[i].getDiameter()/2) Q[i].setX(screenWidth - Q[i].getDiameter()/2);
		
		if (Q[i].getX() <= Q[i].getDiameter()/2) Q[i].setX( Q[i].getDiameter()/2);
		if (Q[i].getY() <= Q[i].getDiameter()/2) Q[i].setY(Q[i].getDiameter()/2);
	}
}

