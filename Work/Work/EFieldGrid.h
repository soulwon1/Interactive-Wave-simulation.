#pragma once
#include "Classes.h"
#include "EFieldForce.h"
#include "ArrowThings.h"

class EFieldGrid {
	int horSpacing;
	int vertSpacing;
	int row;
	int coll;
	vector<vector<Vec2D>> field;
	vector<double> ax;
	vector<vector<double>> ay;
public: EFieldGrid(int Hspacing, int Vspacing, int Rows, int Colls) : horSpacing(Hspacing), vertSpacing(Vspacing), row(Rows), coll(Colls) {}


	void generate(vector<Charge> Q, Charge testCharge) {
		ax.clear();
		ay.clear();
		field.clear();

		for (int i = 0; i < coll; ++i) {
			ax.push_back(horSpacing * i);
			ay.push_back(vector<double>());
			field.push_back(vector<Vec2D>());
			for (int b = 0; b < row; ++b) {
				ay[i].push_back(vertSpacing * b);
				field[i].push_back(ForceCalc(Q, ax[i], ay[i][b], testCharge));
			}
		}
	}
	void update(vector<Charge> Q, Charge testCharge) {
		for (int i = 0; i < coll; ++i) {
			for (int b = 0; b < row; ++b) {
				field[i][b]= (ForceCalc(Q, ax[i], ay[i][b], testCharge));
			}
		}
	}

	void deleteEF() {
		ax.clear();
		ay.clear();
		field.clear();
	}

	void draw(Graphics^ g, double EFmaxSize, double EFScale) {
		Pen^ pen = gcnew Pen(Color::Black);
		if (ax.empty() || ay.empty()) return; //just in case

		for (int i = 0; i < coll; ++i) {
			for (int b = 0; b < row; ++b) {
				DrawArrow(g, ax[i], ay[i][b], totFDir(field[i][b]), 0, arSize(field[i][b], EFmaxSize, EFScale), pen);
			}
		}
	}
};
