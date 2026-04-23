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
	double kField = 1.0;

	void addMagnet(const Magnet& m)
	{
		magnets.push_back(m);
	}

	//compute the B field at point (px, py) due to all magnets. 
	void computeField(double px, double py, double& Bx, double& By) const
	{
		Bx = 0.0;
		By = 0.0;

		for (const auto& m : magnets)
		{
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

		if (i < 0 || i >= (int)magnets.size()) return; //invalid index

		const Magnet& mi = magnets[i];
		double mx, my;

		//Sample field at magnet center
		double Bx, By;
		computeField(mi.x, mi.y, Bx, By);

		//Remove self-contribution (since we don't want a magnet to exert a force on itself)
		//This can be refined but I'm going to accept a small amount of self error. 

		//Torque: tau = M x B = Mx*By - My*Bx (scalar in 2D)
		tau = mx * By - my * Bx;

		//aproximate force via finite difference on potential U = -M . B
		const double h = 1e-1;

		double Bx_px, By_px, Bx_mx, By_mx;
		double Bx_py, By_py, Bx_my, By_my;

		computeField(mi.x + h, mi.y, Bx_px, By_px);
		computeField(mi.x - h, mi.y, Bx_mx, By_mx);
		computeField(mi.x, mi.y + h, Bx_py, By_py);
		computeField(mi.x, mi.y - h, Bx_my, By_my);

		double U_px = -(mx * Bx_px + my * By_px);
		double U_mx = -(mx * Bx_mx + my * By_mx);
		double U_py = -(mx * Bx_py + my * By_py);
		double U_my = -(mx * Bx_my + my * By_my);

		double dUdx = (U_px - U_mx) / (2.0 * h);
		double dUdy = (U_py - U_my) / (2.0 * h);

		Fx = -dUdx; //force is negative gradient of potential
		Fy = -dUdy; //Same here
	}

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
};
