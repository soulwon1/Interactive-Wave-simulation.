#pragma once
#include "Classes.h"
#include "EFieldCalc.h"
#include "ArrowThings.h"
#include "inducedMagField.h"


class EFieldGrid {
	int horSpacing;
	int vertSpacing;
	int row;
	int coll;
	vector<vector<Vec2D>> field;
	vector<double> ax;
	vector<vector<double>> ay;
public: EFieldGrid( int Rows, int Colls) :  row(Rows), coll(Colls) {}


	void generate(vector<Charge> Q, vector<inducMag> mag, int screenWidth, int screenHeight) {
		ax.clear();
		ay.clear();
		field.clear();
		float hSpacing = screenWidth / coll;
		float vSpacing = screenHeight / row;

		for (int i = 0; i <= coll; ++i) {
			ax.push_back(hSpacing * i);
			ay.push_back(vector<double>());
			field.push_back(vector<Vec2D>());
			for (int b = 0; b <= row; ++b) {
				ay[i].push_back(vSpacing * b);
				field[i].push_back(totEat(Q,mag,ax[i],ay[i][b]));
				
			}
		}
	}
	void update(vector<Charge> Q) {
		for (int i = 0; i <= coll; ++i) {
			for (int b = 0; b <= row; ++b) {
				field[i][b]= (EFieldCalcAt(Q, ax[i], ay[i][b]));
			}
		}
	}
	void updateEfield(vector<Charge> Q, vector<inducMag> mag) {
		//update from Q first
		if (!Q.empty()) {
			for (int i = 0; i <= coll; ++i) {
				for (int b = 0; b <= row; ++b) {
					field[i][b] = (EFieldCalcAt(Q, ax[i], ay[i][b]));
				}
			}
		} // update from mag 
		if (!mag.empty()) {
			for (int i = 0; i <= coll; ++i) {
				for (int b = 0; b <= row; ++b) {
					if (!Q.empty()) field[i][b] += (getEfromInducMag(mag, ax[i], ay[i][b])); // if there are charges then add to the field
					else field[i][b] = (getEfromInducMag(mag, ax[i], ay[i][b])); //else the field is just this
				}
			}
		}
		//if both are empty 
		if (mag.empty() && Q.empty()) {
			for (int i = 0; i <= coll; ++i) {
				for (int b = 0; b <= row; ++b) {
					field[i][b] = Vec2D(0, 0);
				}
			}
		}
	}

	void deleteEF() {
		ax.clear();
		ay.clear();
		field.clear();
	}

	void draw(Graphics^ g, double EFmaxSize, double minSize, double EFScale) {
	
		if (ax.empty() || ay.empty()) return; //just in case

		for (int i = 0; i <= coll; ++i) {
			for (int b = 0; b <= row; ++b) {
				Pen^ pen = gcnew Pen(FieldColor(field[i][b]), 3);
				DrawArrow(g, ax[i], ay[i][b], totFDir(field[i][b]), 0, arSize(field[i][b], EFmaxSize, minSize,EFScale), pen);
			}
		}
	}
};
