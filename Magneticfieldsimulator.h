#pragma once

#include "Magnetics.h" 
#include<vector> 
#include <cmath>
#include<tuple>

class MagneticfieldSimulator
{
public:
	std::vector<Magnet> magnets;

	//field strength scaling constant(tuneable for stability/visuals)
	double kField = 10.0;

	void addMagnet(const Magnet& m)
	{
		magnets.push_back(m);
	}

	//compute the B field at point (px, py) due to all magnets. 
	void computeField(double px, double py, double& Bx, double& By, int ignoreIndex) const //intakes position and magentic field. 
	{
		Bx = 0.0;
		By = 0.0;

		for (int i = 0; i < (int)magnets.size(); i++)
		{
			if (i == ignoreIndex) continue; //simple if statement that if true it skips forward to the next one without breaking the loop. 

			const auto& m = magnets[i]; 

			double mx, my;
			std::tie(mx, my) = m.dipole();

			double rx = px - m.x;
			double ry = py - m.y;
			double r2 = rx * rx + ry * ry;
			double r = std::sqrt(r2);

			const double eps = 1e-3; //avoid singularity
			if (r < eps) continue;

			double inv_r = 1.0 / r;
			double inv_r3 = inv_r * inv_r * inv_r;

			double rhatx = rx * inv_r; //represents the unit vector from magnet to point
			double rhaty = ry * inv_r;

			double mdotrhat = mx * rhatx + my * rhaty; //dot product M . rhat

			double Bx_i = kField * (3.0 * mdotrhat * rhatx - mx) * inv_r3; //field contribution from this magnet
			double By_i = kField * (3.0 * mdotrhat * rhaty - my) * inv_r3; //field contribution from this magnet

			Bx += Bx_i;
			By += By_i;
		}
	}


	//compute net forces and torques on each magnet due to the others, and update their velocities and positions.

	void computeForceAndTorqueOn(int i, double& Fx, double& Fy, double& tau) const
	{
		Fx = 0.0;
		Fy = 0.0;
		tau = 0.0;

		if (i < 0 || i >= (int)magnets.size()) return;

		const Magnet& mi = magnets[i];

		//Exctract dipole moment.
		double mx, my;
		std::tie(mx, my) = mi.dipole();   

		// Sample field at magnet center
		double Bx, By;
		computeField(mi.x, mi.y, Bx, By, i);

		// Torque = M × B
		tau = mx * By - my * Bx;

		// Approximately work out the force over finite distnaces. 
		const double h = 1e-1;

		double Bx_px, By_px, Bx_mx, By_mx;
		double Bx_py, By_py, Bx_my, By_my;

		computeField(mi.x + h, mi.y, Bx_px, By_px, i);
		computeField(mi.x - h, mi.y, Bx_mx, By_mx, i);
		computeField(mi.x, mi.y + h, Bx_py, By_py, i);
		computeField(mi.x, mi.y - h, Bx_my, By_my, i);

		double U_px = -(mx * Bx_px + my * By_px);
		double U_mx = -(mx * Bx_mx + my * By_mx);
		double U_py = -(mx * Bx_py + my * By_py);
		double U_my = -(mx * Bx_my + my * By_my);

		double dUdx = (U_px - U_mx) / (2.0 * h);
		double dUdy = (U_py - U_my) / (2.0 * h);

		Fx = -dUdx;
		Fy = -dUdy;
	}
	//Main simulation step. (time step) 
	void step(double dt)
	{
		const int n = (int)magnets.size();
		std::vector<double> Fx(n), Fy(n), Tau(n);

		for (int i = 0; i < n; ++i) //compute forces and torques on each magnet
		{
			computeForceAndTorqueOn(i, Fx[i], Fy[i], Tau[i]);
		}

		for (int i = 0; i < n; ++i) //update each magnet's velocity and position based on computed forces and torques
		{
			magnets[i].applyForce(Fx[i], Fy[i], dt);
			magnets[i].applyTorque(Tau[i], dt);
			magnets[i].integrate(dt);
		}
	}

	void removeMagnet(int index) //simple magnet removal function. 
	{
		if (index < 0 || index >= (int)magnets.size())
		{
			return; 
		}
		magnets.erase(magnets.begin() + index); 
	}
};
