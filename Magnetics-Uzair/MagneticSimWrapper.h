#pragma once

#include "MagneticfieldSimulator.h"
#include "FieldLineTracer.h" 

//This header file was a suggestion from a friend to neaten up our commands for the UI. This will allow for
//simpler UI commands that dont require us to use raw pointers and prevents accidental memory leaks. There are other
//positive benefits such as being able to change the simuilation without having to make massive changed to the UI code. 
//There may be a better way to do this but this is what I came up with.

using namespace System; //for String^
using namespace System::Collections::Generic; //for List<>

public ref class MagneticSimWrapper
{

private:
	MagneticfieldSimulator* sim; //pointer to the native simulator
	FieldLineTracer* tracer; //pointer to the native field line tracer
public:
	MagneticSimWrapper()
	{
		sim = new MagneticfieldSimulator();
		tracer = new FieldLineTracer(sim);
	}

	~MagneticSimWrapper() //Destructor to clean up native resources
	{
		delete sim;
		delete tracer;
	}

	//Add a magnet from WinForms
	void AddMagnet(double x, double y, double angle, double strength, double width, double height)
	{
		sim->addMagnet(Magnet(x, y, angle, strength, width, height)); //Create a Magnet object and add it to the simulator
	}

	//step the physics simulation 

	void Step(double dt)
	{
		sim->step(dt);
	}

	//get magnetic count
	int GetMagnetCount()
	{
		return(int)sim->magnets.size();
	}

	//Get magnet position
	void GetMagnetPosition(int i, double% x, double% y)
	{
		if (i < 0 || i >= (int)sim->magnets.size()) return; //invalid index
		x = sim->magnets[i].x;
		y = sim->magnets[i].y;	
	}

	//Get magnet angle
	double GetMagnetAngle(int i)
	{
		if (i < 0 || i >= (int)sim->magnets.size()) return 0.0; //invalid index
		return sim->magnets[i].angle;
	}

	//Sample magnetic field at a pixel ( for heatmap) 
	void GetField(double px, double py, double& Bx, double% By) //Note: using double& for Bx and double% for By to demonstrate both ways of passing by reference in C++/CLI
	{
		double bx, by; 
		sim->computeField(px, py, bx, by);
		Bx = bx;
		By = by; 
	}

	//Get all field lines (each is a list of points) 
	List<List<System::Drawing::PointF>^>^ GetFieldLines()
	{
		auto nativeLines = tracer->generateFieldLinesPerMagnet();
		//get field lines from the native tracer
		auto result = gcnew List<List<System::Drawing::PointF>^>();
		//Convert native field lines to managed List<List<PointF>>
		for (const auto& line : nativeLines)
		{
			auto managedLine = gcnew List<System::Drawing::PointF>();

			for (const auto& p : line)
			{
				//Convert each native point (Vec2) to a managed PointF and add it to the managed line
				managedLine->Add(System::Drawing::PointF(
					(float)p.x,
					(float)p.y
				));
			}
			result->Add(managedLine); //Add the managed line to the result list
		}
		return result; //Return the collection of field lines to WinForms
	}
public: //here we're going to add some interactive functions, this is to turn the simulation more interactive. Do note you dont need this for barebones simulation.
	void SetMagnetPosition(int i, double x, double y) //This was edited to allow us to set the position of a magnet, this is useful for dragging magnets around with the mouse. Note that we also set the velocity to 0 when we do this to prevent the magnet from jumping after dragging.
	{
		if (i < 0 || i >= (int)sim->magnets.size()) return;

		sim->magnets[i].x = x;
		sim->magnets[i].y = y;

		// Zero velocity so it doesn't jump after dragging
		sim->magnets[i].vx = 0;
		sim->magnets[i].vy = 0;
	}

	void SetMagnetAngle(int i, double angle) //same thing for the angle 
	{
		if (i < 0 || i >= (int)sim->magnets.size()) return; 
		sim->magnets[i].angle = angle; 
		sim->magnets[i].omega = 0.0; 
	}

	void SetMagnetVelocity(int i, double vx, double vy) //same thing for velocity
	{
		if (i < 0 || i >= (int)sim->magnets.size()) return;
		sim->magnets[i].vx = vx;
		sim->magnets[i].vy = vy; 
	}

	void FlipMagnet(int i) //allows us to flip the magnet by changing the angle by a value of pi 
	{
		if (i < 0 || i >= (int)sim->magnets.size()) return; 
		sim->magnets[i].angle += M_PI; 
	}

	void AddMagnetAt(double x, double y) //adds a magnet 
	{
		//default size/strenght; you can tweak this as see fit
		sim->addMagnet(Magnet(x, y, 0.0, 150.0, 80.0, 20.0)); 
	}
};
