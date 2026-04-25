#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <random>
using namespace std;
using namespace System::Drawing;

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
	float diameter;
	Vec2D pos;
	Vec2D vel;
	int chargeIndex;
public:Charge(double iq, double im, double d, Vec2D ipos, Vec2D ivel, int ichargeIndex) : q(iq), m(im), diameter(d), pos(ipos), vel(ivel), chargeIndex(ichargeIndex) {}
	  Charge() : q(1), m(1e-7) {}
	  double getQ() {
		  return q;
	  }
	  double getM() {
		  return m;
	  }
	  double getDiameter() {
		  return diameter;
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
	  Vec2D getV() {
		  return vel;
	  }
	  void setQ(double Q) {
		  q = Q;
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
	  void setDiameter(double d) {
		  diameter = d;
	  }
	  void setIndex(int i) {
		  chargeIndex = i;
	  }
	  int getIndex() {
		  return chargeIndex;
	  }
	  //Drawin charge
	  void draw(Graphics^ g) {
		  Color c = (q < 0) ? Color::DarkBlue : Color::DarkRed;
		  Pen^ pen = gcnew Pen(c);
		  g->DrawEllipse(pen, pos.getX()- diameter/2, pos.getY()- diameter/2, diameter, diameter);
	  }
	  void fill(Graphics^ g) {
		  Color c;
		  if (q ==2) c = Color::SeaGreen;
		  else { c = (q < 0) ? Color::DarkBlue : Color::DarkRed; }
		  Brush^ brush = gcnew SolidBrush(c);
		  g->FillEllipse(brush, pos.getX() - diameter / 2, pos.getY() - diameter / 2, diameter, diameter);
	  }
};

class walls {
	vector<double> wx;
	vector<vector<double>> wy;
	int width;
	int height;
	int holeSize;
	int wallN;
	vector<int> hole;
public: walls(int wallWidth, int wallHeight, int iwallN) : width(wallWidth), height(wallHeight), wallN(iwallN) {}

	int getWidth() { return width; }
	void setWidth(int w) { width = w; }

	int getHeight() { return height; }
	void setHeight(int h) { height = h; }

	int getHoleSize() { return holeSize; }
	void setHoleSize(int hs) { holeSize = hs; }

	vector<double> getWx() { return wx; }
	vector<vector<double>> getWy() { return wy; }

	void generate(int screenWidth, int screenHeight) {
		int spacing = screenWidth / (wallN + 1);
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> dist(holeSize, screenHeight / height);
		hole = vector<int>(wallN);
		for (int i = 0; i < hole.size(); ++i) { //set holes
			hole[i] = dist(gen);
		}
		//clear wall vectors for when new walls are generated
		wx.clear();
		wy.clear();

		for (int ix = 0; ix < wallN; ++ix) {
			wx.push_back(spacing * ix + 1.2 * spacing);
			wy.push_back(vector<double>());//need to create the x vector first, otherwise crash
			for (int iy = 0; iy < (screenHeight / height); ++iy) {
				if (iy < (hole[ix] - holeSize) || iy >= hole[ix]) { //if not in hole
					wy[ix].push_back(height * iy);
				}
			}
		}
	}

	void draw(Graphics^ g) {
		Brush^ wallBrush = gcnew SolidBrush(Color::FromArgb(200, 100, 0));
		for (int ix = 0; ix < wx.size(); ++ix) {
			for (int iy = 0; iy < wy[ix].size(); ++iy) {
				g->FillRectangle(wallBrush, wx[ix], wy[ix][iy], width, height);
			}
		}
	}
	

};


