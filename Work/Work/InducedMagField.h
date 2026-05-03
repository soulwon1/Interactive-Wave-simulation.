#pragma once
#include "Classes.h"
#include "EFieldCalc.h"


class inducMag {
	Vec2D pos;
	double dbdt;
	int index;
	double diameter;
	int inOrOut; //-1 is in, 1 is out
public: inducMag(double idbdt, Vec2D ipos,  double idiameter, int IinOrOut, int Iindex) :dbdt(idbdt), pos(ipos), diameter(idiameter), inOrOut(IinOrOut), index(Iindex) {}

	double getX() { return pos.getX(); }
	void setX(double ix) { pos.setX(ix); }
	double getY() { return pos.getY(); }
	void setY(double iy) { pos.setY(iy); }
	void setPos(Vec2D ipos) { pos = ipos; }
	Vec2D getPos() { return pos; }
	double getRad() { return diameter / 2; }
	void setDiameter(double d) { diameter = d; }
	int getIndex() { return index; }
	void setIndex(int i) { index = i; }
	double getDbdt() { return dbdt; }
	int getInOrOut() { return inOrOut; }
	void setInOrOut(int i) { inOrOut = i;}
	void inverse() { inOrOut *= -1; }

	void drawX(Graphics^ g) {
		Pen^ pen = gcnew Pen(Color::White, 1.0);
		float x1 = pos.getX() - (getRad() / 6); //doing like from center, go this propotion of the rad to the left, then right
		float x2 = pos.getX() + (getRad() / 6);
		float y1 = pos.getY() - (getRad() / 6);
		float y2 = pos.getY()  + (getRad() / 6);

		//inverse y
		//draw lines
		g->DrawLine(pen, x1, y1, x2, y2);
		g->DrawLine(pen, x1, y2, x2, y1);

	}
	void draw(Graphics^ g) {
		Pen^ pen = gcnew Pen(Color::White, 2);
		Brush^ brush = gcnew SolidBrush(Color::White);
		g->DrawEllipse(pen, (float)pos.getX() - (float)getRad(), (float)pos.getY() - (float)getRad(), diameter, diameter);
		if (inOrOut == 1) {
			g->FillEllipse(brush, (float)pos.getX() -5.0f/2, (float)pos.getY()-5.0f/2, 5.0, 5.0);
		}
		else if (inOrOut == -1) {
			drawX(g);
		}
	}

};


Vec2D getEfromInducMag(vector<inducMag>& mag, double x, double y) { //in a circle symmetry, E(x,y) = 1/2 dB/dt *(y,-x)
	Vec2D E(0,0);
	for (int i = 0; i < mag.size(); ++i) {
		
		double dx = (x- mag[i].getX());
		double dy = (y- mag[i].getY() );
		double tol = 1;
		double r = sqrt(dx * dx + dy * dy);
		if (r <= tol) r = tol;

		double k = 0.5 * mag[i].getDbdt();
		float scale = 3e3;
		double Ex = k * dy*mag[i].getInOrOut()/(r*r); //still check if 1/r or 1/r*r physics correctness.
		double Ey = -(k * dx)*mag[i].getInOrOut()/(r*r);
		//double theta = atan2(Ey, Ex);
		E.addComp(Ex*scale, Ey*scale);
	}
	return E;
}
Vec2D totEat(vector<Charge>& Q, vector<inducMag>& mag, double x, double y) {

	Vec2D Etot;
	Vec2D Emag = getEfromInducMag(mag, x, y);
	Vec2D Eq = EFieldCalcAt(Q, x, y);
	Etot = Emag + Eq;
	return Etot;
}
void drawXinO(Graphics^ g, Pen^ pen, double x, double y, double rad) {
	
	float x1 = x  - (rad / 8); //doing like from center, go this propotion of the rad to the left, then right
	float x2 = x  + (rad / 8);
	float y1 = y  - (rad / 8);
	float y2 = y  + (rad/ 8);

	//inverse y
	//draw lines
	g->DrawLine(pen, x1, y1, x2, y2);
	g->DrawLine(pen, x1, y2, x2, y1);
}
void deleteMag(vector<inducMag>& mag) {
	mag.clear();
}

Vec2D forceFromMagOnQ( Charge Q, Vec2D E) {
	Vec2D f;
	f = E.vecTimesScalar(Q.getQ());
	return f;
}

vector<Vec2D> forceCalcAtAllQ(vector<Charge>& Q, vector<inducMag>& mag) {
	vector<Vec2D> force(Q.size(), Vec2D(0, 0));
	//if (Q.size() <= 1) return vector<Vec2D>(0);
	for (int i = 0; i < Q.size(); ++i) {
		Vec2D fe = forceFromMagOnQ(Q[i], getEfromInducMag(mag, Q[i].getX(), Q[i].getY()));
		force[i] += fe;
		for (int m = 0; m < Q.size(); ++m) {
			if (i != m) {
				Vec2D fq = forceTwoQ(Q[i], Q[m]);
				force[i] += fq;
			}
			else continue;
		}
	}
	return force;
}

void simulateAllQcord(vector<Charge>& Q, vector<inducMag>& mag, double dt) {
	vector<Vec2D> f = forceCalcAtAllQ(Q, mag);
	for (int i = 0; i < Q.size(); ++i) {
		Vec2D c = dqCords(Q[i], f[i], dt);
		Q[i].addPos(c);

	}
}

double Emag(Vec2D E) {
	double ex = E.getX();
	double ey = E.getY();
	return sqrt(ex * ex + ey * ey);
}
