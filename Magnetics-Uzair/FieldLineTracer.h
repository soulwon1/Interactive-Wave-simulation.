#pragma once
#include<vector>
#include<cmath>
#include "MagneticfieldSimulator.h"
double M_PI = 3.14159265358979323846; //M_PI was not defined in cmath so I'm defining it here for convenience.
struct Vec2 //simple 2D vector struct for convenience
{
	double x;
	double y;
};

class FieldLineTracer
{
public:
	const MagneticfieldSimulator* sim; //pointer to the simulator to query the field from

	//Intergration parameters
	double stepSize = 5.0; //pixel units per step. 
	int maxSteps = 1000; //maximum number of steps to trace (to prevent infinite loops)
	double minB = 1e-4; //stop when |B| is too small 

	FieldLineTracer(const MagneticfieldSimulator* simulator) : sim(simulator) {}

	//trace a single field line starting at 'start', forward and backwards
	std::vector<Vec2> traceLine(const Vec2& start, bool forward = true) const
	{
		std::vector<Vec2> pts;
		pts.reserve(maxSteps); //reserve space to avoid reallocations
		Vec2 p = start;

		for (int i = 0; i < maxSteps; ++i)
		{
			double Bx, By;
			sim->computeField(p.x, p.y, Bx, By);

			double Bmag = std::sqrt(Bx * Bx + By * By);
			if (Bmag < minB) break; //stop if field is too weak

			//Normalize; 
			Bx /= Bmag;
			By /= Bmag;

			double dir = forward ? 1.0 : -1.0; //Direction including reverse direction if tracing backwards
			p.x += dir * Bx * stepSize;
			p.y += dir * By * stepSize;

			pts.push_back(p);
		}

		return pts;
	}

	//Generate field lines for all magnets, 16 per maget (might change this) 
	// returns: outer vecor = all lines, inner vecot = points along one line; 

	std::vector<std::vector<Vec2>> generateFieldLinesPerMagnet() const
	{
		std::vector<std::vector<Vec2>> allLines;

		const int linesPerMagnet = 8;
		const double radiusFactor = 0.6; //seed radius ratlative to magnet size; 

		for (const auto& m : sim->magnets)
		{
			double mx, my;
			std::tie(mx, my) = m.dipole();

			//seeds points aroudn the magnet center with it being biased along the dipole axis;

			for (int k = 0; k < linesPerMagnet; ++k)
			{
				double t = (2.0 * M_PI * k) / linesPerMagnet;

				//base cirle around magnet
				double rx = std::cos(t);
				double ry = std::sin(t);

				double rSeed = radiusFactor * 0.5 * std::max(m.width, m.height); //seed radius based on magnet size

				Vec2 seed;
				seed.x = m.x + rx * rSeed;
				seed.y = m.y + ry * rSeed;

				//Trace both directions from the seed; 
				auto lineForward = traceLine(seed, true);
				auto lineBackward = traceLine(seed, false);

				//Combine: backward (reversed) + seed + forward because we want the line to be continuous from backward to forward
				std::vector<Vec2> fullLine;
				fullLine.reserve(lineBackward.size() + 1 + lineForward.size());

				for (auto it = lineBackward.rbegin(); it != lineBackward.rend(); ++it) //add backward points in reverse order
				{
					fullLine.push_back(*it);
				}

				fullLine.push_back(seed); //add the seed point in the middle

				for (const auto& p : lineForward) //add forward points
				{
					fullLine.push_back(p);
				}

				if (fullLine.size() > 1)
				{
					allLines.push_back(std::move(fullLine)); //add this line to the collection of all lines
				}
			}
		}
		return allLines;
	}
};