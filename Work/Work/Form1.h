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
			//panel
			wallGamePanel1->Visible = false;
			System::Reflection::PropertyInfo^ p =
				wallGamePanel1->GetType()->GetProperty("DoubleBuffered", //otherwise it was flickering, double buffering apparently is not acessible easily, had to do this
					System::Reflection::BindingFlags::Instance |
					System::Reflection::BindingFlags::NonPublic);

			p->SetValue(wallGamePanel1, true, nullptr);

			screenWidth = this->ClientSize.Width; //set width and height of screen
			screenHeight = this->ClientSize.Height;

			gameWidth = wallGamePanel1 ->Width; //set width and height of game panel
			gameHeight = wallGamePanel1->Height;

			this->MouseClick += gcnew MouseEventHandler(this, &Form1::MClick); //to access mouse coords when click
			this->MouseMove += gcnew MouseEventHandler(this, &Form1::OnMouseMove); //To access mouse cords at all times
			this->MouseUp += gcnew MouseEventHandler(this, &Form1::Mup); //For dragging (when clicking)
			this->MouseDown += gcnew MouseEventHandler(this, &Form1::Mdown); //For dragging (when releasing)

			wallGamePanel1->MouseClick += gcnew MouseEventHandler(this, &Form1::MClick);
			wallGamePanel1->MouseDown += gcnew MouseEventHandler(this, &Form1::Mdown);
			wallGamePanel1->MouseUp += gcnew MouseEventHandler(this, &Form1::Mup);
			wallGamePanel1->MouseMove += gcnew MouseEventHandler(this, &Form1::OnMouseMove);

			//timer and rendering
			this->DoubleBuffered = true; //better rendering, otherwise it looks trash
			timer = gcnew Timer(); // simulation
			timer->Interval = 15;
			timer->Tick += gcnew EventHandler(this, &Form1::Tick);
			timer->Stop();

			//Charges
			Q = new vector<Charge>();
			//Test Charge
			F = new Vec2D(0, 0);
			startX = gameWidth / 10;
			startY = gameHeight / 2;
			x = startX;
			y = startY;
			testCharge = new Charge(2.0, 1e-7, testD, Vec2D(x, y), Vec2D(vx, vy), 0);

			//Max number of Q
			maxQN = 30;

			//Walls
			wallSegHeight = 100;
			wallWidth = 10;
			wallN = 4;
			Walls = new walls(wallWidth, wallSegHeight, wallN);
			
			//Efield
			fieldColl = gameWidth/(EFMaxSize +20);
			fieldRow = gameHeight / (EFMaxSize +20);
			arrowHSpacing = gameWidth / fieldColl;
			bump = gameHeight / fieldRow;
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
		   int maxQleft;
		
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
		   int gameWidth;
		   int gameHeight;

		   //test charge arrows scales for visual purposes
		   double qVarMaxSize = 200;
		   double qVarScale = 0.6;
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
		   Mode prevMode; //for when releasing drag you can still what you were doing
		   double mouseX;
		   double mouseY;
		   int dragIndex;
		   bool drag = true;

		   //simulation checkbox
		   bool simulate = false; 
		   //level
		   int level= 1;

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
private: System::Windows::Forms::Panel^ wallGamePanel1;
private: System::Windows::Forms::Button^ playWallGame;
private: System::Windows::Forms::Panel^ homePanel;
private: System::Windows::Forms::Button^ ExitWallGame;
private: System::Windows::Forms::Label^ Level;
private: System::Windows::Forms::CheckBox^ dragCheckBox;
private: System::Windows::Forms::Button^ noneButton;









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
			this->wallGamePanel1 = (gcnew System::Windows::Forms::Panel());
			this->dragCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->Level = (gcnew System::Windows::Forms::Label());
			this->ExitWallGame = (gcnew System::Windows::Forms::Button());
			this->playWallGame = (gcnew System::Windows::Forms::Button());
			this->homePanel = (gcnew System::Windows::Forms::Panel());
			this->noneButton = (gcnew System::Windows::Forms::Button());
			this->wallGamePanel1->SuspendLayout();
			this->homePanel->SuspendLayout();
			this->SuspendLayout();
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(31, 88);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(46, 17);
			this->checkBox1->TabIndex = 0;
			this->checkBox1->Text = L"Play";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(13, 841);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(113, 48);
			this->button1->TabIndex = 1;
			this->button1->Text = L"Add Charge";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::button1_MouseClick);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(13, 919);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(109, 48);
			this->button3->TabIndex = 3;
			this->button3->Text = L"Delete Charges";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(139, 116);
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
			this->checkBox2->Location = System::Drawing::Point(31, 150);
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
			this->checkBox3->Location = System::Drawing::Point(31, 173);
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
			this->checkBox4->Location = System::Drawing::Point(31, 196);
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
			this->chargesLeftLabel->Location = System::Drawing::Point(28, 809);
			this->chargesLeftLabel->Name = L"chargesLeftLabel";
			this->chargesLeftLabel->Size = System::Drawing::Size(67, 13);
			this->chargesLeftLabel->TabIndex = 8;
			this->chargesLeftLabel->Text = L"Charges Left";
			// 
			// ResetCharges
			// 
			this->ResetCharges->Location = System::Drawing::Point(151, 919);
			this->ResetCharges->Name = L"ResetCharges";
			this->ResetCharges->Size = System::Drawing::Size(109, 48);
			this->ResetCharges->TabIndex = 9;
			this->ResetCharges->Text = L"Reset Charges";
			this->ResetCharges->UseVisualStyleBackColor = true;
			this->ResetCharges->Click += gcnew System::EventHandler(this, &Form1::ResetCharges_Click);
			// 
			// TempGoal
			// 
			this->TempGoal->Location = System::Drawing::Point(1659, 569);
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
			this->EVectorField->Location = System::Drawing::Point(31, 111);
			this->EVectorField->Name = L"EVectorField";
			this->EVectorField->Size = System::Drawing::Size(82, 17);
			this->EVectorField->TabIndex = 11;
			this->EVectorField->Text = L"Vector Field";
			this->EVectorField->UseVisualStyleBackColor = true;
			this->EVectorField->CheckedChanged += gcnew System::EventHandler(this, &Form1::EVectorField_CheckedChanged);
			// 
			// ReversePolarity
			// 
			this->ReversePolarity->Location = System::Drawing::Point(151, 841);
			this->ReversePolarity->Name = L"ReversePolarity";
			this->ReversePolarity->Size = System::Drawing::Size(113, 48);
			this->ReversePolarity->TabIndex = 12;
			this->ReversePolarity->Text = L"Reverse Polarity";
			this->ReversePolarity->UseVisualStyleBackColor = true;
			this->ReversePolarity->Click += gcnew System::EventHandler(this, &Form1::ReversePolarity_Click);
			// 
			// ResetLevel
			// 
			this->ResetLevel->Location = System::Drawing::Point(31, 18);
			this->ResetLevel->Name = L"ResetLevel";
			this->ResetLevel->Size = System::Drawing::Size(82, 52);
			this->ResetLevel->TabIndex = 13;
			this->ResetLevel->Text = L"Reset Level";
			this->ResetLevel->UseVisualStyleBackColor = true;
			this->ResetLevel->Click += gcnew System::EventHandler(this, &Form1::ResetLevel_Click);
			// 
			// wallGamePanel1
			// 
			this->wallGamePanel1->Controls->Add(this->noneButton);
			this->wallGamePanel1->Controls->Add(this->dragCheckBox);
			this->wallGamePanel1->Controls->Add(this->Level);
			this->wallGamePanel1->Controls->Add(this->ExitWallGame);
			this->wallGamePanel1->Controls->Add(this->ResetLevel);
			this->wallGamePanel1->Controls->Add(this->ReversePolarity);
			this->wallGamePanel1->Controls->Add(this->checkBox1);
			this->wallGamePanel1->Controls->Add(this->ResetCharges);
			this->wallGamePanel1->Controls->Add(this->button4);
			this->wallGamePanel1->Controls->Add(this->button3);
			this->wallGamePanel1->Controls->Add(this->chargesLeftLabel);
			this->wallGamePanel1->Controls->Add(this->checkBox2);
			this->wallGamePanel1->Controls->Add(this->button1);
			this->wallGamePanel1->Controls->Add(this->TempGoal);
			this->wallGamePanel1->Controls->Add(this->EVectorField);
			this->wallGamePanel1->Controls->Add(this->checkBox3);
			this->wallGamePanel1->Controls->Add(this->checkBox4);
			this->wallGamePanel1->Location = System::Drawing::Point(-1, 3);
			this->wallGamePanel1->Name = L"wallGamePanel1";
			this->wallGamePanel1->Size = System::Drawing::Size(1834, 1005);
			this->wallGamePanel1->TabIndex = 14;
			this->wallGamePanel1->Visible = false;
			this->wallGamePanel1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::wallGamePanel1_Paint);
			// 
			// dragCheckBox
			// 
			this->dragCheckBox->AutoSize = true;
			this->dragCheckBox->Checked = true;
			this->dragCheckBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->dragCheckBox->Location = System::Drawing::Point(22, 247);
			this->dragCheckBox->Name = L"dragCheckBox";
			this->dragCheckBox->Size = System::Drawing::Size(91, 17);
			this->dragCheckBox->TabIndex = 16;
			this->dragCheckBox->Text = L"Drag Charges";
			this->dragCheckBox->UseVisualStyleBackColor = true;
			this->dragCheckBox->CheckedChanged += gcnew System::EventHandler(this, &Form1::dragCheckBox_CheckedChanged);
			// 
			// Level
			// 
			this->Level->AutoSize = true;
			this->Level->Location = System::Drawing::Point(148, 88);
			this->Level->Name = L"Level";
			this->Level->Size = System::Drawing::Size(42, 13);
			this->Level->TabIndex = 15;
			this->Level->Text = L"Level : ";
			// 
			// ExitWallGame
			// 
			this->ExitWallGame->Location = System::Drawing::Point(139, 23);
			this->ExitWallGame->Name = L"ExitWallGame";
			this->ExitWallGame->Size = System::Drawing::Size(94, 42);
			this->ExitWallGame->TabIndex = 14;
			this->ExitWallGame->Text = L"Exit Game";
			this->ExitWallGame->UseVisualStyleBackColor = true;
			this->ExitWallGame->Click += gcnew System::EventHandler(this, &Form1::ExitWallGame_Click);
			// 
			// playWallGame
			// 
			this->playWallGame->Location = System::Drawing::Point(732, 465);
			this->playWallGame->Name = L"playWallGame";
			this->playWallGame->Size = System::Drawing::Size(163, 126);
			this->playWallGame->TabIndex = 15;
			this->playWallGame->Text = L"Play Wall Game";
			this->playWallGame->UseVisualStyleBackColor = true;
			this->playWallGame->Click += gcnew System::EventHandler(this, &Form1::playWalllGame_Click);
			// 
			// homePanel
			// 
			this->homePanel->Controls->Add(this->playWallGame);
			this->homePanel->Location = System::Drawing::Point(-1, 0);
			this->homePanel->Name = L"homePanel";
			this->homePanel->Size = System::Drawing::Size(1837, 998);
			this->homePanel->TabIndex = 16;
			// 
			// noneButton
			// 
			this->noneButton->Location = System::Drawing::Point(290, 841);
			this->noneButton->Name = L"noneButton";
			this->noneButton->Size = System::Drawing::Size(75, 48);
			this->noneButton->TabIndex = 17;
			this->noneButton->Text = L"None";
			this->noneButton->UseVisualStyleBackColor = true;
			this->noneButton->Click += gcnew System::EventHandler(this, &Form1::noneButton_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1836, 1000);
			this->Controls->Add(this->wallGamePanel1);
			this->Controls->Add(this->homePanel);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->TopMost = true;
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::Form1_Paint);
			this->wallGamePanel1->ResumeLayout(false);
			this->wallGamePanel1->PerformLayout();
			this->homePanel->ResumeLayout(false);
			this->ResumeLayout(false);

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
		void lostLevel() {
			checkBox1->Checked = false;
			x = startX;
			y = startY;
			testCharge->setPos(Vec2D(x, y));
			collision = false;
			testCharge->setVx(0);
			testCharge->setVy(0);
		}
		void goalReached() {
			prevQUsed = QUsed;
			Walls->generate(screenWidth, screenHeight);
			restartLevel();
			level += 1;
		}
		void levelUpdate() {
			Level->Text = "Level : " + level.ToString();
;		}
		void applyDifficulty() {
			if (diff == Diff::easy) Walls->setHoleSize(5);
			else if (diff == Diff::medium) Walls->setHoleSize(3);
			else if (diff == Diff::hard) Walls->setHoleSize(1);
		}

		void drawGame(Graphics^ g) {
			Pen^ pen = gcnew Pen(Color::Black, 1);
			Pen^ dragPen = gcnew Pen(Color::Gray, 1);

			Pen^ vArrowPen = gcnew Pen(Color::FromArgb(0, 255, 0), 1);
			Pen^ fArrowPen = gcnew Pen(Color::FromArgb(255, 1 - intensity(*F), 0), 1);
			//Brush^ wallBrush = gcnew SolidBrush(Color::FromArgb(200, 100, 0));


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
				DrawArrow(g, x - testD / 2, y - testD / 2, totFDir(*F), testD, arSize(*F, qFarMaxSize, qFarScale), fArrowPen);
			}
			//Draw vel arrow if the test charge is moving
			if (testCharge->getVx() != 0 || testCharge->getVy() != 0) {
				DrawArrow(g, x - testD / 2, y - testD / 2, testVDir(*testCharge), testD, arSize(testCharge->getV(), qVarMaxSize, qVarScale), vArrowPen);
			}

			//Draw a circle following the mouse
			if (mode == Mode::addCharge) {
				g->DrawEllipse(dragPen, (float)mouseX - Qd / 2, (float)mouseY - Qd / 2, Qd, Qd);
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

	private: System::Void Form1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
		Graphics^ g = e->Graphics;
		//drawGame(g);
	}

	private: void Tick(Object^ sender, EventArgs^ e) {
		levelUpdate();
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
			if (y >= gameHeight - testD) y = gameHeight - testD;
			//if (x >= gameWidth - testD) x = gameWidth - testD;
			if (x <= 0) x = 0;
			if (y <= 0) y = 0;
			wallCollisionCheck();
			if (collision) { lostLevel(); }
			if (x >= gameWidth - testD) { goalReached(); }
		}
		if (doEField) EField->update(*Q, *testCharge);
		wallGamePanel1->Invalidate();
	}
		   //play checkbox
	private: System::Void checkBox1_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		simulate = checkBox1->Checked;
		if (checkBox1->Checked == true) {
			button4->Visible = false; // walls
			button1->Visible = false; //ad charges
			button3->Visible = false; //delete charges
			checkBox2->Visible = false; //easy
			checkBox3->Visible = false;// med
			checkBox4->Visible = false;//hard
			ResetCharges->Visible = false;
			ReversePolarity->Visible = false;
			chargesLeftLabel->Visible = false;
			dragCheckBox->Visible = false;
			drag = false; //not allowed to drag when in play
			mode = Mode::none;

		} 
		else {
			
			button4->Visible = true; // walls
			button1->Visible = true; //ad charges
			button3->Visible = true; //delete charges
			checkBox2->Visible = true; //easy
			checkBox3->Visible = true;// med
			checkBox4->Visible = true;//hard
			ResetCharges->Visible = true;
			ReversePolarity->Visible = true;
			chargesLeftLabel->Visible = true;
			dragCheckBox->Visible = true;
			mode = prevMode;
			drag = dragCheckBox->Checked; //back to whatever setting you had
		}

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
		  wallGamePanel1-> Invalidate();
	}
	
	void Mdown(Object^ sender, MouseEventArgs^ e) {
		double mx = e->X;
		double my = e->Y;
		//first revrse polarity then drag cuz otherwise you just drag since mode gets set to drag
		if (mode == Mode::reversePolarity) {
			for (int i = Q->size() - 1; i >= 0; --i) {
				if ((mx >= (*Q)[i].getX() - Qd / 2 && mx <= (*Q)[i].getX() + Qd / 2) && (my >= (*Q)[i].getY() - Qd / 2 && my <= (*Q)[i].getY() + Qd / 2)) { //if mose on top of charge
					(*Q)[i].setQ((*Q)[i].getQ() * -1); //multiplying the charge's q by -1
				}
			}
		}
		if (drag) {
			if (prevMode == Mode::deleteCharges) return;
			for (int i = Q->size() - 1; i >= 0; --i) {
				if ((mx >= (*Q)[i].getX() - Qd/2 && mx <= (*Q)[i].getX() +Qd/2) && (my >= (*Q)[i].getY()- Qd/2 && my <= (*Q)[i].getY() + Qd/2)) { //if mouse on top of charge
					mode = Mode::drag; //now actually drag
					dragIndex = i; //to know which charge to drag
				}
			}
		}
		
	}
	void Mup(Object^ sender, MouseEventArgs^ e) {
		if(mode == Mode::drag) mode = prevMode; //to stop draging charge when releasing mouse and back to what you where doing
	}

	private: System::Void button1_MouseClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) { //Add acharges button
		mode = Mode::addCharge;
		prevMode = Mode::addCharge;
	}

	private: System::Void button3_Click(System::Object^ sender, System::EventArgs^ e) { //Delete Charges button
		mode = Mode::deleteCharges;
		prevMode = Mode::deleteCharges;
	}

	private: System::Void button4_Click(System::Object^ sender, System::EventArgs^ e) { //Walls
		// FIRST set difficulty
		applyDifficulty();
		Walls->generate(gameWidth, gameHeight);
		doWalls = true;
	}
	private: System::Void checkBox2_CheckedChanged(System::Object^ sender, System::EventArgs^ e) { //easy
		if (checkBox2->Checked) {
			diff = Diff::easy;
			checkBox3->Checked = false;
			checkBox4->Checked = false;
			applyDifficulty();
			Walls->generate(gameWidth, gameHeight);
			doWalls = true;
		}	
	}

	private: System::Void checkBox3_CheckedChanged(System::Object^ sender, System::EventArgs^ e) { //medium
		if (checkBox3->Checked) {
			diff = Diff::medium;
			checkBox2->Checked = false;
			checkBox4->Checked = false;
			applyDifficulty();
			Walls->generate(gameWidth, gameHeight);
			doWalls = true;
		}
	}
	private: System::Void checkBox4_CheckedChanged(System::Object^ sender, System::EventArgs^ e) { //hard
		if (checkBox4->Checked) {
			diff = Diff::hard;
			checkBox2->Checked = false;
			checkBox3->Checked = false;
			applyDifficulty();
			Walls->generate(gameWidth, gameHeight);
			doWalls = true;
		}
	}

	private: System::Void ResetCharges_Click(System::Object^ sender, System::EventArgs^ e) {
		deleteQ();
	}
	private: System::Void TempGoal_Click(System::Object^ sender, System::EventArgs^ e) {
		prevQUsed = QUsed;
		Walls->generate(screenWidth, screenHeight);
		restartLevel();
		level += 1;
	}
	private: System::Void EVectorField_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		doEField = EVectorField->Checked;
		if (EVectorField->Checked) { EField->generate(*Q, *testCharge); }
		if (EVectorField->Checked == false) { EField ->deleteEF(); }
	}
	private: System::Void ReversePolarity_Click(System::Object^ sender, System::EventArgs^ e) {
		mode = Mode::reversePolarity;
		prevMode =  Mode::reversePolarity;
	}
	private: System::Void ResetLevel_Click(System::Object^ sender, System::EventArgs^ e) {
		restartLevel();
	}
	private: System::Void playWalllGame_Click(System::Object^ sender, System::EventArgs^ e) {
		wallGamePanel1->Visible = true;
		homePanel->Visible = false;
		timer->Start();
	
		// FIRST set difficulty
		applyDifficulty();
			Walls->generate(gameWidth, gameHeight);
			doWalls = true;
	
	}
	private: System::Void wallGamePanel1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
		Graphics^ g = e->Graphics;
		drawGame(g);
	}
	private: System::Void ExitWallGame_Click(System::Object^ sender, System::EventArgs^ e) {
		wallGamePanel1->Visible = false;
		homePanel->Visible = true;
		level = 1;
		Qleft = maxQN;
		prevQUsed = 0;
		dragCheckBox->Checked = false;
		deleteQ();
	}
private: System::Void dragCheckBox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	drag = dragCheckBox->Checked;
}
private: System::Void noneButton_Click(System::Object^ sender, System::EventArgs^ e) {
	mode = Mode::none;
	prevMode = Mode::none;
}
};
}



