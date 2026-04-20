#pragma once
#include "EFieldForce.h"
#include "Classes.h"
#include "ArrowThings.h"

using namespace std;
namespace CppCLRWinFormsProject {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			screenWidth = this->ClientSize.Width; //set width and height of screen
			screenHeight = this->ClientSize.Height;

			this->MouseClick += gcnew MouseEventHandler(this, &Form1::MClick); //to access mouse coords when click
			this->MouseMove += gcnew MouseEventHandler(this, &Form1::OnMouseMove); //To access mouse cords at all times

			this->DoubleBuffered = true; //better rendering, otherwise it looks trash
			timer = gcnew Timer(); // simulation
			timer->Interval = 15;
			timer->Tick += gcnew EventHandler(this, &Form1::Tick);
			timer->Start();

			Q = new vector<Charge>();
			
			//Q->push_back(Charge(1, 0.01, Vec2D(500, 800), Vec2D(0, 0)));

			testCharge = new Charge(2.0, 1e-7, Vec2D(x, y), Vec2D(vx, vy),0);

			F = new Vec2D(0, 0);
			V = new Vec2D(0, 0);

		}
	private: System::Windows::Forms::Timer^ timer;
		   //Screen
		   int screenHeight = 1000;
		   int screenWidth = 1500;
		   double x = screenWidth / 2;
		   double y = screenHeight / 2;
		   //Charges
		   double vx = 0;
		   double vy = 0;
		   vector<Charge>* Q;
		   //Test Charge
		   Charge* testCharge;
		   float testD = 10.0;
		   //Force (adding it here to be able to update the force in paint (for arrow)
		   Vec2D* F;
		   Vec2D* V;
		   

		   int mode = 0; // four modes for mouse actions, assigned to each button, ex ad charge, delete charge, add parallel plates, inspect, etc..
		   double mouseX;
		   double mouseY;
		  
		   bool simulate = false; //For simulation checkbox

	private: System::Windows::Forms::CheckBox^ checkBox1;
		  
	private: System::Windows::Forms::Button^ button2;

	private: System::Windows::Forms::Button^ button1;

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>

		~Form1()
		{
			delete Q;
			delete testCharge;
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(1400, 12);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(66, 17);
			this->checkBox1->TabIndex = 0;
			this->checkBox1->Text = L"Simulate";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(1203, 758);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(141, 73);
			this->button1->TabIndex = 1;
			this->button1->Text = L"Add Charge";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::button1_MouseClick);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(969, 752);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(145, 79);
			this->button2->TabIndex = 2;
			this->button2->Text = L"Stop Adding Charges";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1500, 1000);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->checkBox1);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->TopMost = true;
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::Form1_Paint);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		void DrawArrow(Graphics^ g, double x, double y, double theta, double d, double size, Pen^ pen) {
			
			double hyp = size*1.2;
			double arrowSize = size * 0.5;
			double x1 = x + d/2; //to have the arrow starting at center of circle
			double y1 = y + d/2;
			double x2 = x1 + hyp*cos(theta);
			double y2 = y1 + hyp*sin(theta);

			double arrowAngle = 3.14159 / 6;
			double xar1 = x2 - arrowSize * cos(theta - arrowAngle);
			double yar1 = y2 - arrowSize * sin(theta - arrowAngle);
			double xar2 = x2 - arrowSize * cos(theta + arrowAngle);
			double yar2 = y2 - arrowSize * sin(theta + arrowAngle);

			g->DrawLine(pen,(float)x1, (float)y1, (float)x2, (float)y2);
			g->DrawLine(pen, (float)x2, (float)y2, (float)xar1, (float)yar1);
			g->DrawLine(pen, (float)x2, (float)y2, (float)xar2, (float)yar2);
		}

	private: System::Void Form1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
		Graphics^ g = e->Graphics;
		Pen^ pen = gcnew Pen(Color::Black, 1);
		Pen^ dragPen = gcnew Pen(Color::Gray, 1);
		Brush^ brush = gcnew SolidBrush(Color::Blue);
		Pen^ vArrowPen = gcnew Pen(Color::FromArgb(0, 255, 0), 1);
		Pen^ fArrowPen = gcnew Pen(Color::FromArgb(255,1-intensity(*F), 0), 1);

		float d = 40.0;
		//test charge
		g->FillEllipse(brush, (float)x, (float)y, testD, testD);

		//All stationary charges
		for (int i = 0; i < Q->size(); ++i) {
			double cx = (*Q)[i].getX();
			double cy = (*Q)[i].getY();
			g->DrawEllipse(pen, (float)cx, (float)cy, d, d);
		}
	
		//Draw force arrow if there is a force acting on test charge. THESE CONDITIONS CAN BE CHANGED TO A BUTTON CLIKC (to say if you want thea arrows to show or not)
		if (F->getX() != 0 || F->getY() != 0) {
			DrawArrow(g, x, y, testFDir(*testCharge, *F), testD, fArSize(*F), fArrowPen);
		}
		//Draw vel arrow if the test charge is moving
		if (testCharge->getVx() != 0 || testCharge->getVy() != 0) {
			DrawArrow(g, x, y, testVDir(*testCharge), testD, vArSize(*V), vArrowPen);
		}

		//Draw a circle following the mouse
		if (mode == 1) {
			g->DrawEllipse(dragPen, (float)mouseX, (float)mouseY, d, d);
		}
	}

	private: void Tick(Object^ sender, EventArgs^ e)
	{
		double dt = 0.05;
		Vec2D force = ForceCalc(*Q, *testCharge);
		F->setX(force.getX()); // updating global F and V, for arrow purposes
		F->setY(force.getY());
		V->setX(testCharge->getVx());
		V->setY(testCharge->getVy());

		if (simulate) {
			Vec2D c = dCords(*testCharge, force, dt);
			x += c.getX();
			y += c.getY();
			testCharge->setPos(Vec2D(x, y));
			if (y >= screenHeight - testD) y = screenHeight - testD;
			if (x >= screenWidth - testD) x = screenWidth - testD;
			if (x <= 0) x = 0;
			if (y <= 0) y = 0;
		}
		Invalidate();
	}

	private: System::Void checkBox1_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		simulate = checkBox1->Checked;

	}
		   void OnMouseMove(Object^ sender, MouseEventArgs^ e) {
			   mouseX = e->X;
			   mouseY = e->Y;
		   }

	void MClick(Object^ sender, MouseEventArgs^ e) {
	   double mx = e->X;
	   double my = e->Y;
			if (mode == 1) {
				Q->push_back(Charge(1, 1, Vec2D(mx, my), Vec2D(0, 0),Q->size()-1));
			}
			   Invalidate();
		   }

	private: System::Void button1_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		mode = 1;
	}

	private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) {
		mode = 0;
	}// To delete we might need to add charge index as member data of which gets set with push back with global member in form. Or Hash table
	};
}



