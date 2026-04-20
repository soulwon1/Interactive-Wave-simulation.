#pragma once
#include <iostream>
#include <cmath>
#include <vector>

class Vec2D {
	double x;
	double y;
public:Vec2D(double ix, double iy) : x(ix), y(iy) {}
	  Vec2D() : x(0), y(0) {}
	  double getX() {
		  return x;
	  }
	  double getY() {
		  return y;
	  }
	  void setX(double setx) {
		  x = setx;
	  }
	  void setY(double sety) {
		  y = sety;
	  }
	  void addX(double sx) {
		  x += sx;
	  }
	  void addY(double sy) {
		  y += sy;
	  }
	  void addComp(double sx, double sy) {
		  y += sy;
		  x += sx;
	  }
};

class Charge {
	double q;
	double m;
	Vec2D pos;
	Vec2D vel;
	int chargeIndex;
public:Charge(double iq, double im, Vec2D ipos, Vec2D ivel, int ichargeIndex) : q(iq), m(im), pos(ipos), vel(ivel), chargeIndex(ichargeIndex) {}
	  Charge() : q(1), m(1e-7) {}
	  double getQ() {
		  return q;
	  }
	  double getM() {
		  return m;
	  }
	  double getX() {
		  return pos.getX();
	  }
	  double getY() {
		  return pos.getY();
	  }
	  double getVx() {
		  return vel.getX();
	  }
	  double getVy() {
		  return vel.getY();
	  }
	  void setPos(Vec2D p) {
		  pos = p;
	  }
	  void setVx(double vx) {
		  vel.setX(vx);
	  }
	  void setVy(double vy) {
		  vel.setY(vy);
	  }
	  void addVx(double vx) {
		  vel.addX(vx);
	  }
	  void addVy(double vy) {
		  vel.addY(vy);
	  }
	  void setIndex(int i) {
		  chargeIndex = i;
	  }
	  int getIndex() {
		  return chargeIndex;
	  }
};


#pragma once
