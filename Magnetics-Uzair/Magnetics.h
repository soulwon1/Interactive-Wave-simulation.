#pragma once

#include<cmath>
#include<utility>

class Magnet
{
public:
	double x, y;
	double vx, vy; //velocity components, for dynamic magnets
	double angle; //angle of the magnetic field in radians
	double omega; //angular velocity for dynamic magnets
	double strength; // dipole strength (|M|)
	double width, height; //for the physical size of the magnet
	double mass; //for translation
	double inertia; // moment of inertia for rotation

	Magnet(double x_, double y_, double angle_, double strength_, double width_, double height_, double mass_ = 1.0)
		: x(x_), y(y_), vx(0.0), vy(0.0), angle(angle_), omega(0.0), strength(strength_), width(width_), height(height_), mass(mass_)
	{
		//approx the inertia about the centre of mass for a rectangle
		inertia = (mass * (width * width + height * height)) / 12.0;
	}

	std::pair<double, double> dipole() const
	{
		return { strength * std::cos(angle), strength * std::sin(angle) };
	}

	void applyForce(double fx, double fy, double dt)
	{
		vx += fx / mass * dt;
		vy += fy / mass * dt;
	}

	void applyTorque(double tau, double dt)
	{
		omega += (tau / inertia) * dt;
	}

	void integrate(double dt, double linDamping = 0.98, double angDamping = 0.95)
	{
		//Note here we're adding dampening so the magnets don't accelerate indefinitely. In a real system, you might want to model this more accurately (e.g., with a drag force proportional to velocity).
		// Update position and angle
		x += vx * dt;
		y += vy * dt;
		angle += omega * dt;
		// Apply damping
		vx *= linDamping;
		vy *= linDamping;
		omega *= angDamping;
	}
};




