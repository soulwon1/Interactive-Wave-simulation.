#pragma once
#include "EFieldForce.h"
#include "Classes.h"
#include "ArrowThings.h"
#include <random>
#include "EFieldGrid.h"

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
			this->MouseUp += gcnew MouseEventHandler(this, &Form1::Mup); //For dragging (when clicking)
			this->MouseDown += gcnew MouseEventHandler(this, &Form1::Mdown); //For dragging (when releasing)

			this->DoubleBuffered = true; //better rendering, otherwise it looks trash
			timer = gcnew Timer(); // simulation
			timer->Interval = 15;
			timer->Tick += gcnew EventHandler(this, &Form1::Tick);
			timer->Start();
			//Charges
			Q = new vector<Charge>();
			maxQN = 10;
			//Test Charge
			F = new Vec2D(0, 0);
			startX = screenWidth / 10;
			startY = screenHeight / 2;
			x = startX;
			y = startY;
			testCharge = new Charge(2.0, 1e-7, testD, Vec2D(x, y), Vec2D(vx, vy), 0);

			//Walls
			wallSegHeight = 100;
			wallWidth = 10;
			wallN = 4;
			Walls = new walls(wallWidth, wallSegHeight, wallN);
			
			//Efield
			fieldColl = screenWidth/(EFMaxSize +20);
			fieldRow = screenHeight / (EFMaxSize +20);
			arrowHSpacing = screenWidth / fieldColl;
			bump = screenHeight / fieldRow;
			EField = new EFieldGrid(arrowHSpacing, bump, fieldRow, fieldColl);
			
			
		}
	private: System::Windows::Forms::Timer^ timer;
		   
		   
		   //Charges
		   double vx = 0;
		   double vy = 0;
		   vector<Charge>* Q;
		   float Qd = 40.0;
		   int maxQN;
		   int Qleft;
		   int QUsed;
		   int prevQUsed;
		
		   //Test Charge
		   Charge* testCharge;
		   float testD = 10.0;
		   double x;
		   double y;
		   double startX;
		   double startY;
		   //Force (adding it here to be able to update the force in paint (for arrow)
		   Vec2D* F;
		   
		   //Screen
		   int screenHeight;
		   int screenWidth;

		   //test charge arrows scales for visual purposes
		   double qVarMaxSize = 200;
		   double qVarScale = 0.5;
		   double qFarMaxSize = 200;
		   double qFarScale = 1e7;
		   //modes and mouse
		   enum class Mode {
			   none,
			   addCharge,
			   deleteCharges,
			   drag,
			   reversePolarity,
		   };
		   Mode mode;
		   double mouseX;
		   double mouseY;
		   int dragIndex;

		   //simulation checkbox
		   bool simulate = false; 

		   //Wall coords
		   vector<double>* wx;
		   vector<vector<double>>* wy;
		   //Difficulty
		   enum class Diff {
			  easy,
			  medium,
			  hard,
		   };
		   Diff diff;

		   //Wall related variables
		   walls* Walls;
		   int wallN;
		   double wallSegHeight;
		   int wallWidth;
		   bool doWalls = false;
		   bool collision = false;

		   //Arrow EField
		   double EFMaxSize = 30;
		   double EFScale = 2e7;
		   int fieldColl;
		   int fieldRow;
		   int arrowHSpacing;
		   int bump;
		   bool doEField = false;
		   EFieldGrid* EField;

	private: System::Windows::Forms::CheckBox^ checkBox1;
	private: System::Windows::Forms::Button^ button2;
	private: System::Windows::Forms::Button^ button3;
	private: System::Windows::Forms::Button^ button4;
	private: System::Windows::Forms::CheckBox^ checkBox2;
	private: System::Windows::Forms::CheckBox^ checkBox3;
	private: System::Windows::Forms::CheckBox^ checkBox4;
private: System::Windows::Forms::Label^ chargesLeftLabel;
private: System::Windows::Forms::Button^ ResetCharges;
private: System::Windows::Forms::Button^ TempGoal;
private: System::Windows::Forms::CheckBox^ EVectorField;
private: System::Windows::Forms::Button^ ReversePolarity;
private: System::Windows::Forms::Button^ ResetLevel;




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
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->checkBox2 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox3 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox4 = (gcnew System::Windows::Forms::CheckBox());
			this->chargesLeftLabel = (gcnew System::Windows::Forms::Label());
			this->ResetCharges = (gcnew System::Windows::Forms::Button());
			this->TempGoal = (gcnew System::Windows::Forms::Button());
			this->EVectorField = (gcnew System::Windows::Forms::CheckBox());
			this->ReversePolarity = (gcnew System::Windows::Forms::Button());
			this->ResetLevel = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(1758, 12);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(66, 17);
			this->checkBox1->TabIndex = 0;
			this->checkBox1->Text = L"Simulate";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(1692, 867);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(113, 48);
			this->button1->TabIndex = 1;
			this->button1->Text = L"Add Charge";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::button1_MouseClick);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(1553, 867);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(109, 48);
			this->button2->TabIndex = 2;
			this->button2->Text = L"Drag Charges";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(1426, 867);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(109, 48);
			this->button3->TabIndex = 3;
			this->button3->Text = L"Delete Charges";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(1730, 35);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(94, 34);
			this->button4->TabIndex = 4;
			this->button4->Text = L"Do walls";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &Form1::button4_Click);
			// 
			// checkBox2
			// 
			this->checkBox2->AutoSize = true;
			this->checkBox2->Checked = true;
			this->checkBox2->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox2->Location = System::Drawing::Point(1758, 75);
			this->checkBox2->Name = L"checkBox2";
			this->checkBox2->Size = System::Drawing::Size(49, 17);
			this->checkBox2->TabIndex = 5;
			this->checkBox2->Text = L"Easy";
			this->checkBox2->UseVisualStyleBackColor = true;
			this->checkBox2->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox2_CheckedChanged);
			// 
			// checkBox3
			// 
			this->checkBox3->AutoSize = true;
			this->checkBox3->Location = System::Drawing::Point(1758, 98);
			this->checkBox3->Name = L"checkBox3";
			this->checkBox3->Size = System::Drawing::Size(63, 17);
			this->checkBox3->TabIndex = 6;
			this->checkBox3->Text = L"Medium";
			this->checkBox3->UseVisualStyleBackColor = true;
			this->checkBox3->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox3_CheckedChanged);
			// 
			// checkBox4
			// 
			this->checkBox4->AutoSize = true;
			this->checkBox4->Location = System::Drawing::Point(1758, 121);
			this->checkBox4->Name = L"checkBox4";
			this->checkBox4->Size = System::Drawing::Size(49, 17);
			this->checkBox4->TabIndex = 7;
			this->checkBox4->Text = L"Hard";
			this->checkBox4->UseVisualStyleBackColor = true;
			this->checkBox4->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox4_CheckedChanged);
			// 
			// chargesLeftLabel
			// 
			this->chargesLeftLabel->AutoSize = true;
			this->chargesLeftLabel->Location = System::Drawing::Point(1689, 841);
			this->chargesLeftLabel->Name = L"chargesLeftLabel";
			this->chargesLeftLabel->Size = System::Drawing::Size(67, 13);
			this->chargesLeftLabel->TabIndex = 8;
			this->chargesLeftLabel->Text = L"Charges Left";
			// 
			// ResetCharges
			// 
			this->ResetCharges->Location = System::Drawing::Point(1426, 930);
			this->ResetCharges->Name = L"ResetCharges";
			this->ResetCharges->Size = System::Drawing::Size(109, 48);
			this->ResetCharges->TabIndex = 9;
			this->ResetCharges->Text = L"Reset Charges";
			this->ResetCharges->UseVisualStyleBackColor = true;
			this->ResetCharges->Click += gcnew System::EventHandler(this, &Form1::ResetCharges_Click);
			// 
			// TempGoal
			// 
			this->TempGoal->Location = System::Drawing::Point(1730, 345);
			this->TempGoal->Name = L"TempGoal";
			this->TempGoal->Size = System::Drawing::Size(94, 69);
			this->TempGoal->TabIndex = 10;
			this->TempGoal->Text = L"Temporary Goal";
			this->TempGoal->UseVisualStyleBackColor = true;
			this->TempGoal->Click += gcnew System::EventHandler(this, &Form1::TempGoal_Click);
			// 
			// EVectorField
			// 
			this->EVectorField->AutoSize = true;
			this->EVectorField->Location = System::Drawing::Point(1758, 144);
			this->EVectorField->Name = L"EVectorField";
			this->EVectorField->Size = System::Drawing::Size(82, 17);
			this->EVectorField->TabIndex = 11;
			this->EVectorField->Text = L"Vector Field";
			this->EVectorField->UseVisualStyleBackColor = true;
			this->EVectorField->CheckedChanged += gcnew System::EventHandler(this, &Form1::EVectorField_CheckedChanged);
			// 
			// ReversePolarity
			// 
			this->ReversePolarity->Location = System::Drawing::Point(1692, 930);
			this->ReversePolarity->Name = L"ReversePolarity";
			this->ReversePolarity->Size = System::Drawing::Size(113, 48);
			this->ReversePolarity->TabIndex = 12;
			this->ReversePolarity->Text = L"Reverse Polarity";
			this->ReversePolarity->UseVisualStyleBackColor = true;
			this->ReversePolarity->Click += gcnew System::EventHandler(this, &Form1::ReversePolarity_Click);
			// 
			// ResetLevel
			// 
			this->ResetLevel->Location = System::Drawing::Point(1730, 459);
			this->ResetLevel->Name = L"ResetLevel";
			this->ResetLevel->Size = System::Drawing::Size(91, 71);
			this->ResetLevel->TabIndex = 13;
			this->ResetLevel->Text = L"Reset Level";
			this->ResetLevel->UseVisualStyleBackColor = true;
			this->ResetLevel->Click += gcnew System::EventHandler(this, &Form1::ResetLevel_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1836, 1000);
			this->Controls->Add(this->ResetLevel);
			this->Controls->Add(this->ReversePolarity);
			this->Controls->Add(this->EVectorField);
			this->Controls->Add(this->TempGoal);
			this->Controls->Add(this->ResetCharges);
			this->Controls->Add(this->chargesLeftLabel);
			this->Controls->Add(this->checkBox4);
			this->Controls->Add(this->checkBox3);
			this->Controls->Add(this->checkBox2);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->button3);
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
	
		void ChargesUsed() {
			QUsed = prevQUsed + Q->size();
		}
		void QleftLableUpdate() {
			ChargesUsed();
			Qleft = maxQN - QUsed; //only for now. When doing the next level the Qs should be deleted but the Qleft should not restart. Do another variable of Qused. SoQ left = max - Qused. Q used is Q used += Q.size(), but if reset, tempQused = Qused, and after delete Qused = tempQused.
			chargesLeftLabel->Text = "Charges left : " + Qleft.ToString();
		}
		void deleteQ() {
			for (int i = Q->size() - 1; i >= 0; --i) {
				Q->erase(Q->begin() + i);
			}
		}
		
		void wallCollisionCheck(){
			for (int ix = 0; ix < Walls->getWx().size(); ++ix) {
				for (int iy = 0; iy < Walls->getWy()[ix].size(); ++iy) {
					if ((((x - testD / 2 <= Walls->getWx()[ix]+wallWidth) && (x+ testD/2 >= Walls->getWx()[ix] + wallWidth)) || ((x + testD / 2 >= Walls->getWx()[ix]) && (x - testD / 2 <= Walls->getWx()[ix]))) && (((y + testD / 2 >= Walls->getWy()[ix][iy]) && (y - testD / 2 <= Walls->getWy()[ix][iy]+wallSegHeight)))) {
						collision = true;
						return;
					}
				}
			}
		}
		void restartLevel() {
			checkBox1->Checked = false;
			deleteQ();
			x = startX;
			y = startY;
			testCharge->setPos(Vec2D(x, y));
			collision = false;
			testCharge->setVx(0);
			testCharge->setVy(0);
			
		}

	private: System::Void Form1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
		Graphics^ g = e->Graphics;
		Pen^ pen = gcnew Pen(Color::Black, 1);
		Pen^ dragPen = gcnew Pen(Color::Gray, 1);

		Pen^ vArrowPen = gcnew Pen(Color::FromArgb(0, 255, 0), 1);
		Pen^ fArrowPen = gcnew Pen(Color::FromArgb(255,1-intensity(*F), 0), 1);
		//Brush^ wallBrush = gcnew SolidBrush(Color::FromArgb(200, 100, 0));

		if (diff == Diff::easy) Walls->setHoleSize(5);
		if (diff == Diff::medium) Walls->setHoleSize(3);
		if (diff == Diff::hard) Walls->setHoleSize(1);
		
		//test charge draw
		testCharge->fill(g);
		//E field draw (before stationary charges to hide arrows over them)
		if (doEField) {
			EField->draw(g, EFMaxSize, EFScale);
		}
		//All stationary charges
		for (int i = 0; i < Q->size(); ++i) {
			double cx = (*Q)[i].getX();
			double cy = (*Q)[i].getY();
			(*Q)[i].fill(g);
		}
	
		//Draw force arrow if there is a force acting on test charge. THESE CONDITIONS CAN BE CHANGED TO A BUTTON CLIKC (to say if you want thea arrows to show or not)
		if (F->getX() != 0 || F->getY() != 0) {
			DrawArrow(g, x-testD/2, y-testD/2, totFDir(*F), testD, arSize(*F,qFarMaxSize, qFarScale), fArrowPen);
		}
		//Draw vel arrow if the test charge is moving
		if (testCharge->getVx() != 0 || testCharge->getVy() != 0) {
			DrawArrow(g, x-testD/2, y-testD/2, testVDir(*testCharge), testD, arSize(testCharge->getV(), qVarMaxSize, qVarScale), vArrowPen);
		}

		//Draw a circle following the mouse
		if (mode == Mode::addCharge) {
			g->DrawEllipse(dragPen, (float)mouseX -Qd/2, (float)mouseY -Qd/2, Qd, Qd);
		}
		//drag
		if (mode == Mode::drag) {
		(*Q)[dragIndex].setPos(Vec2D(mouseX, mouseY));
		}
		//walls
		if (doWalls) {
			Walls->draw(g);
		}	
		
	}

	private: void Tick(Object^ sender, EventArgs^ e)
	{
		QleftLableUpdate();
		double dt = 0.05;
		Vec2D force = ForceCalc(*Q, x, y, *testCharge);
		F->setX(force.getX()); // updating global F and V, for test charge arrow purposes
		F->setY(force.getY());
		if (simulate) {
			Vec2D c = dCords(*testCharge, force, dt);
			x += c.getX();
			y += c.getY();
			testCharge->setPos(Vec2D(x, y));
			if (y >= screenHeight - testD) y = screenHeight - testD;
			if (x >= screenWidth - testD) x = screenWidth - testD;
			if (x <= 0) x = 0;
			if (y <= 0) y = 0;
			wallCollisionCheck();
			if (collision) { restartLevel(); }
		}
		if (doEField) EField->update(*Q, *testCharge);
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
	   
		if (mode == Mode::addCharge && Qleft > 0) {
			Q->push_back(Charge(1, 1,Qd, Vec2D(mx, my), Vec2D(0, 0), Q->size() - 1));
		}
		if (mode == Mode::deleteCharges) {
			for (int i = Q->size() - 1; i >= 0; --i) {
				if ((mx >= (*Q)[i].getX() -Qd/2 && mx <= (*Q)[i].getX() + Qd/2) && (my >= (*Q)[i].getY() -Qd/2 && my <= (*Q)[i].getY() + Qd/2)) {
					Q->erase(Q->begin() + i);
				}
			}
		}
		   Invalidate();
	}

	void Mdown(Object^ sender, MouseEventArgs^ e) {
		double mx = e->X;
		double my = e->Y;
		if (mode == Mode::none) {
			for (int i = Q->size() - 1; i >= 0; --i) {
				if ((mx >= (*Q)[i].getX() - Qd/2 && mx <= (*Q)[i].getX() +Qd/2) && (my >= (*Q)[i].getY()- Qd/2 && my <= (*Q)[i].getY() + Qd/2)) { //if mouse on top of charge
					mode = Mode::drag; //now actually drag
					dragIndex = i; //to know which charge to drag
				}
			}
		}
		if (mode == Mode::reversePolarity) {
			for (int i = Q->size() - 1; i >= 0; --i) {
				if ((mx >= (*Q)[i].getX() - Qd / 2 && mx <= (*Q)[i].getX() + Qd / 2) && (my >= (*Q)[i].getY() - Qd / 2 && my <= (*Q)[i].getY() + Qd / 2)) { //if mose on top of charge
					(*Q)[i].setQ((*Q)[i].getQ() * -1); //multiplying the charge's q by -1
				}
			}
		}
	}
	void Mup(Object^ sender, MouseEventArgs^ e) {
		if(mode == Mode::drag) mode = Mode::none;
	}

	private: System::Void button1_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) { //Add acharges button
		mode = Mode::addCharge;
	}

	private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) { //Drag charges button (although mode = none for logic purposes)
		mode = Mode::none;
	}

	private: System::Void button3_Click(System::Object^ sender, System::EventArgs^ e) { //Delete Charges button
		mode = Mode::deleteCharges;
	}

	private: System::Void button4_Click(System::Object^ sender, System::EventArgs^ e) { //Walls
		Walls->generate(screenWidth, screenHeight);
		doWalls = true;
	}
	private: System::Void checkBox2_CheckedChanged(System::Object^ sender, System::EventArgs^ e) { //easy
		if (checkBox2->Checked) {
			diff = Diff::easy;
			checkBox3->Checked = false;
			checkBox4->Checked = false;
		}	
	}

	private: System::Void checkBox3_CheckedChanged(System::Object^ sender, System::EventArgs^ e) { //medium
		if (checkBox3->Checked) {
			diff = Diff::medium;
			checkBox2->Checked = false;
			checkBox4->Checked = false;
		}
	}
	private: System::Void checkBox4_CheckedChanged(System::Object^ sender, System::EventArgs^ e) { //hard
		if (checkBox4->Checked) {
			diff = Diff::hard;
			checkBox2->Checked = false;
			checkBox3->Checked = false;
		}
	}

	private: System::Void ResetCharges_Click(System::Object^ sender, System::EventArgs^ e) {
		deleteQ();
	}
	private: System::Void TempGoal_Click(System::Object^ sender, System::EventArgs^ e) {
		prevQUsed = QUsed;
		deleteQ();
		Walls->generate(screenWidth, screenHeight);
		x = startX;
		y = startY;
		testCharge->setPos(Vec2D(x, y));
		checkBox1->Checked = false;
	}
	private: System::Void EVectorField_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		doEField = EVectorField->Checked;
		if (EVectorField->Checked) { EField->generate(*Q, *testCharge); }
		if (EVectorField->Checked == false) { EField ->deleteEF(); }
	}
	private: System::Void ReversePolarity_Click(System::Object^ sender, System::EventArgs^ e) {
		mode = Mode::reversePolarity;
	}
private: System::Void ResetLevel_Click(System::Object^ sender, System::EventArgs^ e) {
	restartLevel();
}
};
}



