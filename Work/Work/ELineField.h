#pragma once
#include "InducedMagField.h"
#include "Classes.h"

class Eline {
	Vec2D pos1;
	Vec2D pos2;
	float length;
	Vec2D vel; //velocity
	int life;
	int state;//spawning = 1, just hanging = 0, dying = -1
public: Eline(Vec2D ipos1, Vec2D ipos2, int ilife, int istate) : pos1(ipos1), pos2(ipos2), life(ilife), state(istate) {}
	  float getX1() { return pos1.getX(); }
	  void setX1(float ix1) { pos1.setX(ix1); }
	  float getY1() { return pos1.getY(); }
	  void setY1(float iy1) { pos1.setY(iy1); }

	  float getX2() { return pos2.getX(); }
	  void setX2(float ix2) { pos2.setX(ix2); }
	  float getY2() { return pos2.getY(); }
	  void setY2(float iy2) { pos2.setY(iy2); }

	  Vec2D getPos1() { return pos1; }
	  void setPos1(Vec2D ipos1) { pos1 = ipos1; }
	  Vec2D getPos2() { return pos2; }
	  void setPos2(Vec2D ipos2) { pos2 = ipos2; }
	 void setVel(Vec2D ivel) {  vel = ivel; }

	  int getLife() { return life; }
	  void setLife(int ilife) { life = ilife; }
	  double getLength() { return length; }
	  void setLength(double l) { length = l; }
	  void setLengthFromPos() {
		  double dx = pos2.getX() - pos1.getX();
		  double dy = pos2.getY() - pos1.getY();
		  length = sqrt(dx * dx + dy * dy);
	  }
	  int getState() { return state; }
	  void setState(int s) { state = s; }
	  void decreaseLife() { --life; }
	  double getAngle() { return atan2(vel.getY(), vel.getX()); }

	  void lengthFromE(Vec2D E, float maxLength, float minLength) {
		  double mag = sqrt(E.getX() * E.getX() + E.getY() * E.getY());

		  float k = 1e-2; //tuning parameter

		  float norm = 1.0 - exp(-k * mag);
		  length = minLength + (maxLength - minLength) * norm;
	  }
	  double getLengthFromE(Vec2D E, float maxLength, float minLength) {
		  double mag = sqrt(E.getX() * E.getX() + E.getY() * E.getY());

		  float k = 1e-2; //tuning parameter

		  float norm = 1.0 - exp(-k * mag);
		  return( minLength + (maxLength - minLength) * norm);
	  }

	  void lengthInQ(float maxLength, float minLength, float r, float rad) { //for inside charges so that the spauning looks smooth
		  float norm = r / rad;
		  if (norm > 1.0) norm = 1.0;

		  float smooth = norm * norm * (3.0 - 2.0 * norm);

		  length = minLength + (maxLength - minLength) * smooth;
	  }

	  float birthLength(Vec2D E, float maxLength, float minLength) {
		  double mag = sqrt(E.getX() * E.getX() + E.getY() * E.getY());
		  float k = 7e4; //tuning parameter
		  float norm = 1.0 - exp(-k * mag);
		  length = minLength + (maxLength - minLength) * norm;
		  return minLength;
	  }

	  void VelFromE(Vec2D E, float baseSpeed) {
		  double maxField = 10;
		  float maxSpeed = 3;
		  double mag = sqrt(E.getX() * E.getX() + E.getY() * E.getY());
		  mag = min(mag, maxField);

		  if (mag < 1e-6) {
			  mag = 1e-6;
		  }
		  float k = 6e-4;
		  float norm = 1.0f - exp(-k * mag);
		  float speed = baseSpeed + 200.0 * norm;
		  if (speed > maxSpeed) {
			  speed = maxSpeed;
		  }

		  float vx = (E.getX() / mag) * speed;
		  float vy = (E.getY() / mag) * speed;

		  vel.setX(vx);
		  vel.setY(vy);
	  }

	  void VelinQ(Vec2D E, float r, float rad, float acc) {
		  double maxField = 10;
		  double mag = sqrt(E.getX() * E.getX() + E.getY() * E.getY());
		  mag = min(mag, maxField);
		 // if (mag < 1e-10) {
		//	  life = 0;
		//	  vel = Vec2D(0, 0);
		//	  return;
		 // }

		  float vx = (E.getX() / mag) * acc;
		  float vy = (E.getY() / mag) * acc;
		  vel.addX(vx*6e-4);
		  vel.addY(vy*5e-4);
	  }

	  void newPos2fromV(Vec2D V, float dt) {
		  pos2 += V.vecTimesScalar(dt);
	  }

	  void pos1Move(float dt) {
		  pos1 += (vel.vecTimesScalar(dt));
	  }
	  void pos2Move(float dt) {
		  pos2 += (vel.vecTimesScalar(dt));
	  }
	  void pos2MoveTowardsPos1(Vec2D E,float dt) { //so that when dying in a circulating mag field the pos2 actually reaches pos1 and gets eliminated
		  double dx = pos1.getX() - pos2.getX();
		  double dy = pos1.getY() -pos2.getY();
		  double dir = atan2(dy, dx);
		  
		  double magV = sqrt(vel.getX() * vel.getX() + vel.getY() * vel.getY()); //do magnitude of V times the actual direction;
		  Vec2D newV(magV * cos(dir), magV * sin(dir));
		  pos2 += newV.vecTimesScalar(dt);
		  
	  }
	

	  Vec2D pos2fromAngle(double theta, float ilength) {
		  length = ilength;
		  return Vec2D(pos1.getX() + cos(theta) * length, pos1.getY() + sin(theta) * length);
	  }

	  void drawLine(Graphics^ g, Pen^ pen) {
		  
		  g->DrawLine(pen, (float)pos1.getX(), (float)pos1.getY(), (float)pos2.getX(), (float)pos2.getY());
	  }

	  void StateCheck(double lengthFromE, int maxLife) {

		  if (state == 1) {
			  life = maxLife;
			  double dx = pos1.getX() - pos2.getX();
			  double dy = pos1.getY() - pos2.getY();
			  double r = sqrt(dx * dx + dy * dy);
			  if (r >= lengthFromE) {
				  state = 0;
			  }
		  }
		  
	  }
	  void SmoothVelFromE(Vec2D E, float dt) {
		  float maxSpeed = 50;
		  float responseK = 6e-4;
		  float tau = 0.01;
		  double mag = sqrt(E.getX() * E.getX() + E.getY() * E.getY());
		  if (mag < 1e-8) mag = 1e-8;

		  // --- direction ---
		  Vec2D dir(E.getX() / mag, E.getY() / mag);

		  // --- soft clamp (adjustable curve) ---
		  // responseK controls how fast it approaches maxSpeed
		  float speed = maxSpeed * (1.0f - exp(-responseK * mag));

		  Vec2D targetVel = dir.vecTimesScalar(speed);

		  // --- smooth velocity transition ---
		  float alpha = dt / (tau + dt);

		  vel += (targetVel - vel).vecTimesScalar(alpha);
	  }
	  void setPos1ForDrag(Vec2D E) {
		  double theta = atan2(E.getY(), E.getX());
		  double x = pos2.getX() + length * cos(theta);
		  double y = pos2.getY() + length * sin(theta);
		  pos1 = Vec2D(x, y);
	  }
};
Vec2D getVelFromE(Vec2D E, float baseSpeed) {// i need this function to update pos2 when state = 0 such that it follows pos1 at the vel it should, and not at vel of pos1 which would cause length = const
	double maxField = 10;
	float maxSpeed = 3;
	double mag = sqrt(E.getX() * E.getX() + E.getY() * E.getY());
	mag = min(mag, maxField);

	if (mag < 1e-6) {
		mag = 1e-6;
	}
	float k = 6e-4;
	float norm = 1.0f - exp(-k * mag);
	float speed = baseSpeed + 200.0 * norm;
	if (speed > maxSpeed) {
		speed = maxSpeed;
	}

	float vx = (E.getX() / mag) * speed;
	float vy = (E.getY() / mag) * speed;
	return(Vec2D(vx, vy));
}
double wantedLengthfromE(Vec2D E, float maxLength, float minLength) {
	double mag = sqrt(E.getX() * E.getX() + E.getY() * E.getY());

	float k = 1e-2; //tuning parameter

	float norm = 1.0 - exp(-k * mag);
	return minLength + (maxLength - minLength) * norm;
}
vector<Vec2D> getQCenters(vector<Charge> Q) {
	vector<Vec2D> birthPos(Q.size());
	for (int i = 0; i < Q.size(); ++i) {
		birthPos[i] = Q[i].getPos();
	}
	return birthPos;
}
vector<Vec2D> getEatQ(vector<Charge> Q) {
	int r = 10;
	vector<Vec2D> E(Q.size());
	for (int i = 0; i < Q.size(); ++i) {
		E[i] = EFieldCalcAt(Q, Q[i].getX()+r, Q[i].getY()+r);
	}
	return E;
}
//need E at every Q to generate the lines. Then function of update coords of lines will take only E at its pos.
//firweorks version
/*
void generateLines1(vector<Eline>& line, vector<Vec2D> birthPos, vector<Vec2D> E, int lgenerated, int maxLife, float maxLength, float minLength) {
	int twoPi = 2 * 3.1415;
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<double> randAngle(0, twoPi);

	for (int iq = 0; iq < birthPos.size(); ++iq) {
		for (int i = 0; i < lgenerated; ++i) {
			double theta = (2 * 3.1415 * i) / lgenerated;

			float r = 0;
			Vec2D pos1 = Vec2D(birthPos[iq].getX() + r * cos(theta), birthPos[iq].getY() + r * sin(theta));

			Eline newLine(pos1, pos1, maxLife);

			float len = newLine.birthLength(E[iq], maxLength, minLength);
			Vec2D pos2 = newLine.pos2fromAngle(theta, len);
			newLine.setPos2(pos2);

			line.push_back(newLine);
		}
	}
}
*/
//smooth version of gen field lines

void generateLines2( vector<Eline>& line,vector<Vec2D> birthPos,int lgenerated,int maxLife,float maxLength,float minLength, vector<Charge>& Q) {  
	int twoPi = 2 * 3.1415;

	for (int iq = 0; iq < birthPos.size(); ++iq) {

		if (iq >= Q.size()) { continue; }

		if (Q[iq].getQ() < 0) { continue; }

		for (int i = 0; i < lgenerated; ++i) {

			double theta = (2 * 3.1415 * i) / lgenerated;

			float r = Q[iq].getRad();

			Vec2D pos1(
				birthPos[iq].getX() + r * cos(theta),
				birthPos[iq].getY() + r * sin(theta)
			);

			Eline newLine(pos1, pos1, maxLife,1);

			Vec2D pos2 = newLine.pos2fromAngle(theta, minLength);
			newLine.setPos2(pos2);

			line.push_back(newLine);
		}
	}
}
void lineAtEdgeGen(vector<Eline>& line, int screenWidth, int screenHeight, int Hspacing, int Vspacing, int maxLife, float minLength) {

	int vertGen = screenHeight / Vspacing;
	int horGen = screenWidth / Hspacing;
	for (int iy = 0; iy < vertGen; ++iy) { //gen lines at the far right of scren
		Vec2D pos1(screenWidth, Vspacing * iy);
		Vec2D pos2(screenWidth - minLength, Vspacing * iy);
		Eline newLine(pos1, pos2, maxLife,1);
		line.push_back(newLine);
	}
	for (int ix = 0; ix < horGen; ++ix) {// Bottom
		Vec2D pos1(Hspacing * ix, screenHeight);
		Vec2D pos2(Hspacing * ix, screenHeight - minLength);
		Eline newLine(pos1, pos2, maxLife,1);
		line.push_back(newLine);
	}
	for (int ix = 0; ix < horGen; ++ix) {// Top
		Vec2D pos1(Hspacing * ix, 0);
		Vec2D pos2(Hspacing * ix, 0 + minLength);
		Eline newLine(pos1, pos2, maxLife,1);
		line.push_back(newLine);
	}
	for (int iy = 0; iy < vertGen; ++iy) { //left
		Vec2D pos1(0, Vspacing * iy);
		Vec2D pos2(0 + minLength, Vspacing * iy);
		Eline newLine(pos1, pos2, maxLife,1);
		line.push_back(newLine);
	}
}
//rotating angles version
void RotgenerateLines2(vector<Eline>& line, vector<Vec2D> birthPos, vector<Vec2D> E, int lRotgenerated, int maxLife, float maxLength, float minLength, vector<Charge>& Q, int cycle) {  // FIX: const ref

	double twoPi =2.0 * 3.1415926535;
	random_device rd;
	static mt19937 gen(random_device{}()); //static cuz otherwise it was missing always some angles
	uniform_real_distribution<double> randAngle(0.0, twoPi);

	for (int iq = 0; iq < birthPos.size(); ++iq) {

	//	if (iq >= Q.size()) { continue; } //just in case

		if (Q[iq].getQ() < 0) {
			continue;
			//lineAtEdgeGen(line, 1800, 100, 50, 50, maxLife, minLength); 
			}

		for (int i = 0; i < lRotgenerated; ++i) {

		//	double theta = ((2 * 3.1415 * i) / lRotgenerated)+cycle;
			double theta = randAngle(gen);
			float r = Q[iq].getRad();

			Vec2D pos2(
				birthPos[iq].getX() + r * cos(theta),
				birthPos[iq].getY() + r * sin(theta)
			);

			Eline newLine(pos2, pos2, maxLife,1);

			Vec2D pos1 = newLine.pos2fromAngle(theta, minLength);
			newLine.setPos2(pos1);

			line.push_back(newLine);
		}
	}
}

void drawElines(Graphics^ g, vector<Eline>& line, vector<Charge>& Q, vector<inducMag>& mag)
{
	for (int i = 0; i < line.size(); ++i) {

		//Vec2D E = EFieldCalcAt(Q, line[i].getX1(), line[i].getY1()); //with this one it looks so cool, although inacurate
		Vec2D E = totEat(Q, mag, line[i].getX1(), line[i].getY1());
		Pen^ pen = gcnew Pen(FieldColor(E), 2);

		line[i].drawLine(g, pen);
	}
}
bool inCircle(float xC, float yC, float rad, float x, float y) {
	float r = sqrt((x - xC) * (x - xC) + (y - yC) * (y - yC));
	if (r < rad) {
		return true;
	}
	else return false;

}
void updateLineLife(
	vector<Eline>& line, vector<Charge>& Q, vector<inducMag>% mag, int screenWidth,int screenHeight) {
	for (int i = line.size() - 1; i >= 0; --i) {

		line[i].decreaseLife(); //every tick

		if (line[i].getX2() > screenWidth || line[i].getX2() < 0) line[i].setState(-2); //delete if out of screen
		if (line[i].getY2() > screenHeight || line[i].getY2() < 0) line[i].setState(-2);

		for (int iq = 0; iq < Q.size(); ++iq) {
			if (Q[iq].getQ() < 0) { //delete if going into neg charge (check if pos 2 at r 5 from charge or pos 1 at r < 5 (if generated by that charge)

				if (inCircle(Q[iq].getX(), Q[iq].getY(), Q[iq].getRad()-10, line[i].getX2(), line[i].getY2())) {
					line[i].setState(-2);
					break;
				}
			}
		}

		if (line[i].getState()== -2) {
			line.erase(line.begin() + i);
		}
	}

	if (Q.size() == 0 && mag.size() == 0) { //if you delete all charges, field should also evaporate
		line.clear();
	}
}

float getVelAt(Vec2D E, float baseSpeed) { //to see the target vel we need at radius of circle
	double maxField = 10;
	float maxSpeed = 2;
	double mag = sqrt(E.getX() * E.getX() + E.getY() * E.getY());
	mag = min(mag, maxField);

	float k = 6e-4;
	float norm = 1.0f - exp(-k * mag);
	float speed = baseSpeed + 200.0 * norm;
	float vmax = 1;
	if (speed > vmax) speed = vmax;
	if (mag < 1e-6) mag= 1e-6;
	float vx = (E.getX() / mag) * speed;
	float vy = (E.getY() / mag) * speed;

	return sqrt(vx * vx + vy * vy);
}

//do the whole moevemnt and scaling after birth
void fieldLineUpdateGridVersion(vector<Eline>& line, vector<Charge>& Q, vector<inducMag> mag,float Qd, float baseSpeed, float dt,float maxLength, float minLength, int maxLife, bool dragQ, bool dragMag) {
	for (int i = line.size() - 1; i >= 0; --i) {

		float x = line[i].getX1();
		float y = line[i].getY1();

		Vec2D E = totEat(Q, mag, x, y);
		Vec2D E2 = totEat(Q, mag, line[i].getX2(), line[i].getY2());

		float ex = E.getX();
		float ey = E.getY();
		float ex2 = E2.getX();
		float ey2 = E2.getY();

		float magE = sqrt(ex * ex + ey * ey);
		if (magE < 1e-8) magE = 1e-8;
		float magE2 = sqrt(ex2 * ex2 + ey2 * ey2);
		if (magE2 < 1e-8) magE2 = 1e-8;
		bool inNegCharge = false;
		// DELETE INSIDE NEGATIVE CHARGES first!


		for (int jq = 0; jq < Q.size(); ++jq) {

			if (Q[jq].getQ() < 0) {//need angle for simple trig to set the pos1 at radius 5
				if (inCircle(Q[jq].getX(), Q[jq].getY(), Q[jq].getRad()-10, line[i].getX1(), line[i].getY1())) { //using 8 and then 4 so it doesnt "miss" it
					
					//line[i].setPos1(Vec2D(Q[jq].getX(), Q[jq].getY()));
					line[i].setState(-1); //this fixes pos1 and pos2 goes towards it
					
					inNegCharge = true;
				}
				//kill if pos2 got to Q
				if (inCircle(Q[jq].getX(), Q[jq].getY(), Q[jq].getRad(), line[i].getX2(), line[i].getY2())) {
					line[i].setState(-2);
					break;
				}

			}

		}
		if (line[i].getState() == -2) continue;
		//delete lines inside magnetic inductor
		if (mag.size() > 0) {
			for (int im = 0; im < mag.size(); im++) {
				if (inCircle(mag[im].getX(), mag[im].getY(), mag[im].getRad() + 20, line[i].getX1(), line[i].getY1())) {
					line[i].setState(-2);
				}
			}
		}
		if (!inNegCharge) { //skip alive motion if in charge, still move state -1
			//if spawning
			if (line[i].getState() == 1) {
				if (dragQ || dragMag) {
					double target = wantedLengthfromE(E, maxLength, minLength);
					Vec2D dir(E2.getX() / magE2, E2.getY() / magE2);
					line[i].VelFromE(E2, baseSpeed);
					line[i].pos1Move(dt);
					line[i].StateCheck(target, maxLife);//check state here so that if it sets statete to 0, it doesnt get set back to 1 again next frame if length increases before checking
				}
				else {
					
					double target = wantedLengthfromE(E, maxLength, minLength);
					Vec2D dir(E2.getX() / magE2, E2.getY() / magE2);
					line[i].VelFromE(E, baseSpeed);
					line[i].pos1Move(dt);
					line[i].StateCheck(target, maxLife);//check state here so that if it sets statete to 0, it doesnt get set back to 1 again next frame if length increases before checking

					/*
					double target = wantedLengthfromE(E, maxLength, minLength);
					Vec2D dir(E2.getX() / magE2, E2.getY() / magE2);
					line[i].VelFromE(E2, baseSpeed);
					line[i].pos1Move(dt);
					line[i].StateCheck(target, maxLife);//check state here so that if it sets statete to 0, it doesnt get set back to 1 again next frame if length increases before checking
					*/
				}

			}
			//there needs to be two different logics for dragging and for when not dragging, to keep visuals
			
			if (line[i].getState() == 0) { //if not spawning, just normal lifespan, move pos2 from geometry and velocity
				if (dragQ || dragMag) {
					//get E at pos2
				
					line[i].lengthFromE(E, maxLength, minLength);
					Vec2D vel = getVelFromE(E2, baseSpeed);
					line[i].newPos2fromV(vel, dt);
					line[i].setPos1ForDrag(E2);
				}
				else {
					/*
					line[i].lengthFromE(E, maxLength, minLength);
					Vec2D vel = getVelFromE(E2, baseSpeed);
					line[i].newPos2fromV(vel, dt);
					line[i].setPos1ForDrag(E2);
					*/
					line[i].lengthFromE(E, maxLength, minLength);
					line[i].VelFromE(E, baseSpeed);
					// move AFTER velocity is set 
					line[i].pos1Move(dt);
					//get vel at pos2
					Vec2D vel = getVelFromE(E2, baseSpeed);
					line[i].newPos2fromV(vel, dt);
					
				}
			}
		}
			 if (line[i].getState() == -1) {// dying, pos2 remains fixed, only pos1 moves
				line[i].VelFromE(E2, baseSpeed);
				// move AFTER velocity is set 
				line[i].pos2MoveTowardsPos1(E2, dt);
				double dx = line[i].getX1() - line[i].getX2();
				double dy = line[i].getY1() - line[i].getY2();
				double r = sqrt(dx * dx + dy * dy);
				if (inNegCharge) {
					if (r <= 10) line[i].setState(-2);
				}
				else if(!inNegCharge) {
					if (r <= 3) {
						line[i].setState(-2);
					}
				}
			}

		

		
		
		
		
		//state update depending on life
			if (line[i].getLife() <= 0 && line[i].getState() !=-2) {
				line[i].setState(-1);
			}
		
		
	}
}

//generate initial field lines
void initialElineGen(vector<Eline>& line, int fieldColl, int fieldRow, int Hspacing, int Vspacing,  int maxLife, float minLength) {  

	for (int ix = 0; ix < fieldColl; ++ix) {

		for (int iy = 0; iy < fieldRow; ++iy) {

			Vec2D pos1(Hspacing*ix, Vspacing*iy );
			Vec2D pos2(Hspacing * ix + minLength, Vspacing * iy);

			Eline newLine(pos1, pos2, maxLife,1);
			line.push_back(newLine);
		}
	}
}



/*
void fieldLineUpdate(vector<Eline>& line, vector<Charge>& Q, vector<inducMag> magnet,float Qd, float baseSpeed, float dt,float maxLength, float minLength) {
	for (int i = line.size() - 1; i >= 0; --i) {

		float x = line[i].getX1();
		float y = line[i].getY1();

		Vec2D E = totEat(Q, magnet, x, y);

		float ex = E.getX();
		float ey = E.getY();

		float mag = sqrt(ex * ex + ey * ey);

		bool inCharge = false;

		//if spawning
		if (line[i].isSpawning(wantedLengthfromE(E, maxLength, minLength))) {

			line[i].VelFromE(E, baseSpeed);
			line[i].newPos2fromV();
		}
		else { //if not spawning do nromal
			line[i].lengthFromE(E, maxLength, minLength);
			line[i].VelFromE(E, baseSpeed);
			// move AFTER velocity is set
			line[i].pos1Move(dt);
		}


		// CHARGE REGION
		
		for (int iq = 0; iq < Q.size(); ++iq) {

			float cx = Q[iq].getX();
			float cy = Q[iq].getY();
			float rad = Q[iq].getRad();

			if (inCircle(cx, cy, rad, x, y)) {

				float r = sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy));

				// LENGTH inside charge
				line[i].lengthInQ(maxLength, minLength, r, rad);

				// velocity inside charge (IMPORTANT)
				float t = 0.015f;
				float acc = getVelAt(totEat(Q, magnet, x, y), baseSpeed) * t;

				line[i].VelinQ(E, r, rad, acc);

				inCharge = true;
				break;
			}
		}


		// OUTSIDE CHARGE
		if (!inCharge) {

			line[i].lengthFromE(E, maxLength, minLength);
			line[i].VelFromE(E, baseSpeed);
		}
		// move AFTER velocity is set
		line[i].pos1Move(dt);
		


		// DELETE INSIDE NEGATIVE CHARGES

bool pos2AtQ = false;

for (int jq = 0; jq < Q.size(); ++jq) {

	if (Q[jq].getQ() < 0) {//need angle for simple trig to set the pos2 at radius 5
		if (inCircle(Q[jq].getX(), Q[jq].getY(), 8, line[i].getX2(), line[i].getY2())) { //using 8 and then 4 so it doesnt "miss" it
			double lineAngle = atan2(line[i].getY1() - Q[jq].getY(), line[i].getX1() - Q[jq].getX());
			double fixX = Q[jq].getX() + 4 * cos(lineAngle);
			double fixY = Q[jq].getY() + 4 * sin(lineAngle);
			line[i].setPos2(Vec2D(fixX, fixY));
			pos2AtQ = true;
			break;
		}
		//kill if pos1 got to Q
		if (inCircle(Q[jq].getX(), Q[jq].getY(), 5, line[i].getX1(), line[i].getY1())) {
			line[i].setLife(0);
			break;
		}

	}

}
if (!pos2AtQ) { line[i].newPos2fromV(); }
	}
}
*/

//generating a field grid thats more natural
void niceFieldGen(vector<Eline>& line, vector<Charge>& Q, vector<inducMag>& mag, int fieldColl, int fieldRow, int Hspacing, int Vspacing, int maxLife, float minLength, int cycle) {
	//eachcycle lines get started at a slightly different position, and when cycles finish it repeats. This creates the illusion of the whole field once its all generated
	for (int ix = 0; ix < fieldColl; ++ix) {

		for (int iy = 0; iy < fieldRow; ++iy) {
			if (cycle == 1) {//at orign
				Vec2D pos1(Hspacing * ix, Vspacing * iy);
				Eline newLine(pos1, pos1, maxLife, 1);
				Vec2D E = totEat(Q, mag, pos1.getX(), pos1.getY());

				float magE = sqrt(E.getX() * E.getX() + E.getY() * E.getY());
				if (magE < 1e-6) magE = 1e-6;

				Vec2D dir(E.getX() / magE, E.getY() / magE);

				// small initial length
				float initLength = minLength;

				// set pos2 properly
				newLine.setPos2(pos1 - dir.vecTimesScalar(initLength));
				
				// IMPORTANT: initialize velocity
				newLine.setVel(dir.vecTimesScalar(0.1f)); // small starting vel

				// also set length explicitly
				newLine.setLength(initLength);

				line.push_back(newLine);
			}
			if (cycle == 2) {//bottom right
				Vec2D pos1(Hspacing * ix + Hspacing / 2, Vspacing * iy + Vspacing / 2);
				Eline newLine(pos1, pos1, maxLife, 1);
				Vec2D E = totEat(Q, mag, pos1.getX(), pos1.getY());

				float magE = sqrt(E.getX() * E.getX() + E.getY() * E.getY());
				if (magE < 1e-6) magE = 1e-6;

				Vec2D dir(E.getX() / magE, E.getY() / magE);

				// small initial length
				float initLength = minLength;

				// set pos2 properly
				newLine.setPos2(pos1 - dir.vecTimesScalar(initLength));

				// IMPORTANT: initialize velocity
				newLine.setVel(dir.vecTimesScalar(0.1f)); // small starting vel

				// also set length explicitly
				newLine.setLength(initLength);
				line.push_back(newLine);
			}
			if (cycle == 3) {//bottom left
				Vec2D pos1(Hspacing * ix, Vspacing * iy + Vspacing / 2);
				Eline newLine(pos1, pos1, maxLife, 1);
				Vec2D E = totEat(Q, mag, pos1.getX(), pos1.getY());

				float magE = sqrt(E.getX() * E.getX() + E.getY() * E.getY());
				if (magE < 1e-6) magE = 1e-6;

				Vec2D dir(E.getX() / magE, E.getY() / magE);

				// small initial length
				float initLength = minLength;

				// set pos2 properly
				newLine.setPos2(pos1 - dir.vecTimesScalar(initLength));

				// IMPORTANT: initialize velocity
				newLine.setVel(dir.vecTimesScalar(0.1f)); // small starting vel

				// also set length explicitly
				newLine.setLength(initLength);
				line.push_back(newLine);
			}
			if (cycle == 4) {//top right
				Vec2D pos1(Hspacing * ix + Hspacing/2, Vspacing * iy);
				Eline newLine(pos1, pos1, maxLife, 1);
				Vec2D E = totEat(Q, mag, pos1.getX(), pos1.getY());

				float magE = sqrt(E.getX() * E.getX() + E.getY() * E.getY());
				if (magE < 1e-6) magE = 1e-6;

				Vec2D dir(E.getX() / magE, E.getY() / magE);

				// small initial length
				float initLength = minLength;

				// set pos2 properly
				newLine.setPos2(pos1 - dir.vecTimesScalar(initLength));

				// IMPORTANT: initialize velocity
				newLine.setVel(dir.vecTimesScalar(0.1f)); // small starting vel

				// also set length explicitly
				newLine.setLength(initLength);
				line.push_back(newLine);
			}
			
			
			if (cycle == 5) {//center
				Vec2D pos1(Hspacing * ix + Hspacing/4, Vspacing * iy+ Vspacing/4);
				Eline newLine(pos1, pos1, maxLife, 1);
				Vec2D E = totEat(Q, mag, pos1.getX(), pos1.getY());

				float magE = sqrt(E.getX() * E.getX() + E.getY() * E.getY());
				if (magE < 1e-6) magE = 1e-6;

				Vec2D dir(E.getX() / magE, E.getY() / magE);

				// small initial length
				float initLength = 2;

				// set pos2 properly
				newLine.setPos2(pos1 - dir.vecTimesScalar(initLength));

				// IMPORTANT: initialize velocity
				newLine.setVel(dir.vecTimesScalar(0.1f)); // small starting vel

				// also set length explicitly
				newLine.setLength(initLength);
				line.push_back(newLine);
			}
			
		}
	}
	
}

void randFieldGen(vector<Eline>& line, vector<Charge>& Q, vector<inducMag>& mag, int lGenerated, int screenWidth, int screenHeight, int maxLife) {
	double twoPi = 2.0 * 3.1415926535;
	random_device rd;
	static mt19937 gen(random_device{}()); //static cuz otherwise it was missing always some angles
	uniform_int_distribution<int> randX(0,screenWidth);
	uniform_int_distribution<int> randY(0, screenHeight);

	for (int i = 0; i < lGenerated; ++i) {
		bool accept = false;
		int x;
		int y;
		while (!accept) { //check if a line spawnied in the center, or very near the center of a Q (it breaks if it does)
			x = randX(gen); //spawin random coords
			y = randY(gen);
			accept = true;
			for (int iq = 0; iq < Q.size(); ++iq) { // so they dont spawn in charges
				double dx = Q[iq].getX() - x;
				double dy = Q[iq].getY() - y;
				double r = sqrt(dx * dx + dy * dy);
				if (r <= 20) { accept =false;break; }
			}	
			for (int im = 0; im < mag.size(); ++im) {
				double dx = mag[im].getX() - x;
				double dy = mag[im].getY() - y;
				double r = sqrt(dx * dx + dy * dy);
				if (r <= mag[im].getRad()+30) { accept = false; break; }
			}
			
		}
		Vec2D pos1(x, y);
		Vec2D E = totEat(Q, mag, pos1.getX(), pos1.getY());

		float magE = sqrt(E.getX() * E.getX() + E.getY() * E.getY());
		if (magE < 1e-6) magE = 1e-6;

		Vec2D dir(E.getX() / magE, E.getY() / magE);
	// small initial length
	float initLength = 2;
	Eline newLine(pos1, pos1, maxLife, 1);

	// set pos2 properly
	newLine.setPos2(pos1 - dir.vecTimesScalar(initLength));

	// IMPORTANT: initialize velocity
	newLine.setVel(dir.vecTimesScalar(0.1f)); // small starting vel

	// also set length explicitly
	newLine.setLength(initLength);
	line.push_back(newLine);
	}
}
