#pragma once
#include "EFieldForce.h"
#include "Classes.h"
#include "ArrowThings.h"
#include <random>
#include "EFieldGrid.h"
#include "ElineField.h"
#include "inspectCalculator1.h"

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
			System::Reflection::PropertyInfo^ p1 =
				wallGamePanel1->GetType()->GetProperty("DoubleBuffered", //otherwise it was flickering, double buffering apparently is not acessible easily, had to do this
					System::Reflection::BindingFlags::Instance |
					System::Reflection::BindingFlags::NonPublic);
			p1->SetValue(wallGamePanel1, true, nullptr);
			
			System::Reflection::PropertyInfo^ p2 =
				ElectricSandboxPanel->GetType()->GetProperty("DoubleBuffered", //Same for sandbox
					System::Reflection::BindingFlags::Instance |
					System::Reflection::BindingFlags::NonPublic);
			p2->SetValue(ElectricSandboxPanel, true, nullptr);

			//screenWidth = this->ClientSize.Width; //set width and height of screen
			//screenHeight = this->ClientSize.Height;
			screenWidth = ElectricSandboxPanel->Width;
			screenHeight = ElectricSandboxPanel->Height;

			gameWidth = wallGamePanel1 ->Width; //set width and height of game panel
			gameHeight = wallGamePanel1->Height;

			/*
			this->MouseClick += gcnew MouseEventHandler(this, &Form1::MClick); //to access mouse coords when click
			this->MouseMove += gcnew MouseEventHandler(this, &Form1::OnMouseMove); //To access mouse cords at all times
			this->MouseUp += gcnew MouseEventHandler(this, &Form1::Mup); //For dragging (when clicking)
			this->MouseDown += gcnew MouseEventHandler(this, &Form1::Mdown); //For dragging (when releasing)
			*/
			wallGamePanel1->MouseClick += gcnew MouseEventHandler(this, &Form1::MClick);
			wallGamePanel1->MouseDown += gcnew MouseEventHandler(this, &Form1::Mdown);
			wallGamePanel1->MouseUp += gcnew MouseEventHandler(this, &Form1::Mup);
			wallGamePanel1->MouseMove += gcnew MouseEventHandler(this, &Form1::OnMouseMove);

			ElectricSandboxPanel->MouseClick += gcnew MouseEventHandler(this, &Form1::MClick);
			ElectricSandboxPanel->MouseDown += gcnew MouseEventHandler(this, &Form1::Mdown);
			ElectricSandboxPanel->MouseUp += gcnew MouseEventHandler(this, &Form1::Mup);
			ElectricSandboxPanel->MouseMove += gcnew MouseEventHandler(this, &Form1::OnMouseMove);
			
			//timer and rendering
			this->DoubleBuffered = true; //better rendering, otherwise it looks trash
			timer = gcnew Timer(); // simulation
			timer->Interval = 15;
			timer->Tick += gcnew EventHandler(this, &Form1::Tick);
			timer->Stop();

			//Charges
			Q = new vector<Charge>();
			//Test Charge
			testqField = new Vec2D(0, 0);
			startX = screenWidth /10; //the -100 + testD/2 + 10 is to align iwth the center of the station
			startY = screenHeight /2;
			x = startX;
			y = startY;

			sandStartX = screenWidth - 150 + testD / 2 + 5; //the -100 + testD/2 + 10 is to align iwth the center of the station
			sandStartY = screenHeight * 0.45 + testD / 2 + 5;
			xs = sandStartX;
			ys = sandStartY;
			testCharge = new Charge(2.0, 1e-7, testD, Vec2D(x, y), Vec2D(vx, vy), 0);
			sandboxTestCharge = new Charge(2.0, 1e-7, testD, Vec2D(xs, ys), Vec2D(vx, vy), 0);

			//Max number of Q
			maxQN = 30;

			//Walls
			wallSegHeight = 100;
			wallWidth = 30;
			wallN = 4;
			Walls = new walls(wallWidth, wallSegHeight, wallN);
			
			//Efield
			fieldColl = (screenWidth/(EFMaxSize +20)); //the + 20 is so that you do have arrows at the edge of screen
			fieldRow = (screenHeight / (EFMaxSize +20));
			EField = new EFieldGrid( fieldRow, fieldColl);

			//E line field
			eLine = new vector<Eline>();
			qCenters = new vector<Vec2D>(Q->size());
			EfatQ = new vector<Vec2D>(Q->size());
			lineFieldRow = screenHeight / (maxLength -maxLength/5);
			lineFieldColl = screenWidth / (maxLength -maxLength/15);
			lineHspacing = screenWidth / lineFieldColl;
			lineVspacing = screenHeight / lineFieldRow;
			

			//Induced mag field
			mag = new vector<inducMag>();

			//So that the works in sandbox pannels work, they werent for some reason
			this->SandboxAddChargeB->Click += gcnew System::EventHandler(this, &Form1::SandboxAddChargeB_Click);
			this->SandboxDeleteChargesB->Click += gcnew System::EventHandler(this, &Form1::SandboxDeleteChargesB_Click);
			this->SandboxResetChargesB->Click += gcnew System::EventHandler(this, &Form1::SandboxResetChargesB_Click);
			this->SandboxReversePolB->Click += gcnew System::EventHandler(this, &Form1::SandboxReversePolB_Click);
			this->SandboxNoneB->Click += gcnew System::EventHandler(this, &Form1::SandboxNoneB_Click);
			this->SandboxSimulateCB->CheckedChanged +=gcnew System::EventHandler(this, &Form1::SandboxSimulateCB_CheckedChanged);
			this->SandboxInspectCB->CheckedChanged +=gcnew System::EventHandler(this, &Form1::SandboxInspectCB_CheckedChanged);
			this->SandboxVecFieldCB->CheckedChanged +=gcnew System::EventHandler(this, &Form1::SandboxVecFieldCB_CheckedChanged);
			
		}
	private: System::Windows::Forms::Timer^ timer;
		   
		   //Charges
		   double vx = 0;
		   double vy = 0;
		   vector<Charge>* Q;
		   float Qd = 40.0;
		   int sandQd = 80;
		   int maxQN =30;
		   int Qleft;
		   int QUsed;
		   int prevQUsed = 0;
		   int maxQleft;
		   bool drawQandM = true;
		
		   //Test Charges
		   Charge* testCharge;
		   Charge* sandboxTestCharge;
		   float testD = 20.0;
		   double startX; 
		   double startY;
		   double x;
		   double y;
		   double sandStartX;
		   double sandStartY;
		   double xs;
		   double ys;
		   bool dragTestQ = false;
		   bool activeTestQ = false;
		   //Force (adding it here to be able to update the force in paint (for arrow)
		   Vec2D* testqField;
		   
		   //Screen
		   int screenHeight;
		   int screenWidth;
		   int gameWidth;
		   int gameHeight;

		   //test charge arrows scales for visual purposes
		   double qVarMaxSize = 200;
		   double qVarScale = 0.6;
		   double qFarMaxSize = 200;
		   double qFarScale = 1.2e0;
		   double gameqFarScale = 1e7;
		   double qfMinSize = 1;
		   //modes and mouse
		   enum class Mode {
			   none,
			   addCharge,
			   deleteCharges,
			   drag,
			   reversePolarity,
			   induceMag,
		   };
		   Mode mode;
		   Mode prevMode; //for when releasing drag you can still what you were doing
		   bool drag = true;
		   bool dragQ = false;
		   bool dragMag = false;
		   bool sandDrag = true;
		   double mouseX;
		   double mouseY;
		   int dragIndex;
		   

		   //simulation checkbox
		   bool play = false; 
		   bool simulateAllQ = false;
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
		   double EFMaxSize = 35;
		   double minSize = 5;
		   double EFScale = 1.2e0;
		   int fieldColl;
		   int fieldRow;
		   bool doEField = false;
		   bool sandDoEField = false;
		   EFieldGrid* EField;

		   //E field lines variables
		   int tickCount = 50;
		   int maxTick = 100;
		   bool doEfieldLines = false;
		   bool drawEfieldLines = false;
		   int maxLife = 100;
		   float maxLength = 100;
		   float minLength = 5;
		   int baseSpeed = 2; 
		   vector<Eline>* eLine;
		   vector<Vec2D>* qCenters;
		   vector<Vec2D>* EfatQ;
		   int genCycle = 1;
		   int fieldLgenerated = 3; //per generation cycle
		   int lineFieldColl;
		   int lineFieldRow;
		   int lineHspacing;
		   int lineVspacing;

		   enum class gen {
			   RandomField,
			   GridGen,
			   RandomCharge,
			   RadialGen,
		   };
		   gen generation;

		   // induced magnetic field
		   vector<inducMag>* mag;
		   float magDia = 80.0;
		   double dbdt = 10;
		   int in = -1;
		   int out = 1;

		   //inspect
		   bool inspect = false;

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
private: System::Windows::Forms::Button^ ElectricSandbox;
private: System::Windows::Forms::Panel^ ElectricSandboxPanel;
private: System::Windows::Forms::CheckBox^ SandboxVecFieldCB;
private: System::Windows::Forms::CheckBox^ SandboxSimulateCB;
private: System::Windows::Forms::CheckBox^ SandboxInspectCB;
private: System::Windows::Forms::CheckBox^ SandboxDragCB;
private: System::Windows::Forms::Button^ SandboxNoneB;
private: System::Windows::Forms::Button^ SandboxReversePolB;
private: System::Windows::Forms::Button^ SandboxResetChargesB;
private: System::Windows::Forms::Button^ SandboxDeleteChargesB;
private: System::Windows::Forms::Button^ SandboxAddChargeB;
private: System::Windows::Forms::Button^ ExitESandboxB;
private: System::Windows::Forms::CheckBox^ EFieldLinesCB;
private: System::Windows::Forms::Button^ SandboxInduceMagB;
private: System::Windows::Forms::Label^ DebugLable;
private: System::Windows::Forms::ComboBox^ EFieldLinesGen;
private: System::Windows::Forms::CheckBox^ DrawQM;
private: System::Windows::Forms::CheckBox^ TestChargeActivate;
private: System::Windows::Forms::Label^ InspectLable;
private: System::Windows::Forms::Label^ ActivateTesQWarning;




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
			this->noneButton = (gcnew System::Windows::Forms::Button());
			this->dragCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->Level = (gcnew System::Windows::Forms::Label());
			this->ExitWallGame = (gcnew System::Windows::Forms::Button());
			this->playWallGame = (gcnew System::Windows::Forms::Button());
			this->homePanel = (gcnew System::Windows::Forms::Panel());
			this->ElectricSandbox = (gcnew System::Windows::Forms::Button());
			this->ElectricSandboxPanel = (gcnew System::Windows::Forms::Panel());
			this->ActivateTesQWarning = (gcnew System::Windows::Forms::Label());
			this->InspectLable = (gcnew System::Windows::Forms::Label());
			this->TestChargeActivate = (gcnew System::Windows::Forms::CheckBox());
			this->DrawQM = (gcnew System::Windows::Forms::CheckBox());
			this->EFieldLinesGen = (gcnew System::Windows::Forms::ComboBox());
			this->DebugLable = (gcnew System::Windows::Forms::Label());
			this->SandboxInduceMagB = (gcnew System::Windows::Forms::Button());
			this->EFieldLinesCB = (gcnew System::Windows::Forms::CheckBox());
			this->ExitESandboxB = (gcnew System::Windows::Forms::Button());
			this->SandboxVecFieldCB = (gcnew System::Windows::Forms::CheckBox());
			this->SandboxSimulateCB = (gcnew System::Windows::Forms::CheckBox());
			this->SandboxInspectCB = (gcnew System::Windows::Forms::CheckBox());
			this->SandboxDragCB = (gcnew System::Windows::Forms::CheckBox());
			this->SandboxNoneB = (gcnew System::Windows::Forms::Button());
			this->SandboxReversePolB = (gcnew System::Windows::Forms::Button());
			this->SandboxResetChargesB = (gcnew System::Windows::Forms::Button());
			this->SandboxDeleteChargesB = (gcnew System::Windows::Forms::Button());
			this->SandboxAddChargeB = (gcnew System::Windows::Forms::Button());
			this->wallGamePanel1->SuspendLayout();
			this->homePanel->SuspendLayout();
			this->ElectricSandboxPanel->SuspendLayout();
			this->SuspendLayout();
			// 
			// checkBox1
			// 
			this->checkBox1->BackColor = System::Drawing::Color::DarkGreen;
			this->checkBox1->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->checkBox1->ForeColor = System::Drawing::Color::LightGray;
			this->checkBox1->Location = System::Drawing::Point(20, 88);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(60, 30);
			this->checkBox1->TabIndex = 0;
			this->checkBox1->Text = L"Play";
			this->checkBox1->UseVisualStyleBackColor = false;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// button1
			// 
			this->button1->BackColor = System::Drawing::Color::ForestGreen;
			this->button1->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button1->Location = System::Drawing::Point(13, 841);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(113, 48);
			this->button1->TabIndex = 1;
			this->button1->Text = L"Add Charge";
			this->button1->UseVisualStyleBackColor = false;
			this->button1->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::button1_MouseClick);
			// 
			// button3
			// 
			this->button3->BackColor = System::Drawing::Color::Maroon;
			this->button3->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button3->Location = System::Drawing::Point(13, 919);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(109, 48);
			this->button3->TabIndex = 3;
			this->button3->Text = L"Delete Charges";
			this->button3->UseVisualStyleBackColor = false;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// button4
			// 
			this->button4->BackColor = System::Drawing::Color::DarkSlateGray;
			this->button4->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button4->Location = System::Drawing::Point(139, 116);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(94, 34);
			this->button4->TabIndex = 4;
			this->button4->Text = L"Do walls";
			this->button4->UseVisualStyleBackColor = false;
			this->button4->Click += gcnew System::EventHandler(this, &Form1::button4_Click);
			// 
			// checkBox2
			// 
			this->checkBox2->AutoSize = true;
			this->checkBox2->BackColor = System::Drawing::Color::Indigo;
			this->checkBox2->Checked = true;
			this->checkBox2->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox2->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->checkBox2->Location = System::Drawing::Point(31, 161);
			this->checkBox2->Name = L"checkBox2";
			this->checkBox2->Size = System::Drawing::Size(57, 19);
			this->checkBox2->TabIndex = 5;
			this->checkBox2->Text = L"Easy";
			this->checkBox2->UseVisualStyleBackColor = false;
			this->checkBox2->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox2_CheckedChanged);
			// 
			// checkBox3
			// 
			this->checkBox3->AutoSize = true;
			this->checkBox3->BackColor = System::Drawing::Color::Indigo;
			this->checkBox3->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->checkBox3->Location = System::Drawing::Point(31, 184);
			this->checkBox3->Name = L"checkBox3";
			this->checkBox3->Size = System::Drawing::Size(82, 19);
			this->checkBox3->TabIndex = 6;
			this->checkBox3->Text = L"Medium";
			this->checkBox3->UseVisualStyleBackColor = false;
			this->checkBox3->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox3_CheckedChanged);
			// 
			// checkBox4
			// 
			this->checkBox4->AutoSize = true;
			this->checkBox4->BackColor = System::Drawing::Color::Indigo;
			this->checkBox4->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->checkBox4->Location = System::Drawing::Point(31, 207);
			this->checkBox4->Name = L"checkBox4";
			this->checkBox4->Size = System::Drawing::Size(60, 19);
			this->checkBox4->TabIndex = 7;
			this->checkBox4->Text = L"Hard";
			this->checkBox4->UseVisualStyleBackColor = false;
			this->checkBox4->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox4_CheckedChanged);
			// 
			// chargesLeftLabel
			// 
			this->chargesLeftLabel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->chargesLeftLabel->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 10.5F, System::Drawing::FontStyle::Bold));
			this->chargesLeftLabel->Location = System::Drawing::Point(13, 805);
			this->chargesLeftLabel->Name = L"chargesLeftLabel";
			this->chargesLeftLabel->Size = System::Drawing::Size(138, 20);
			this->chargesLeftLabel->TabIndex = 8;
			this->chargesLeftLabel->Text = L"Charges Left";
			// 
			// ResetCharges
			// 
			this->ResetCharges->BackColor = System::Drawing::Color::Maroon;
			this->ResetCharges->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ResetCharges->Location = System::Drawing::Point(151, 919);
			this->ResetCharges->Name = L"ResetCharges";
			this->ResetCharges->Size = System::Drawing::Size(109, 48);
			this->ResetCharges->TabIndex = 9;
			this->ResetCharges->Text = L"Reset Charges";
			this->ResetCharges->UseVisualStyleBackColor = false;
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
			this->EVectorField->Location = System::Drawing::Point(20, 126);
			this->EVectorField->Name = L"EVectorField";
			this->EVectorField->Size = System::Drawing::Size(82, 17);
			this->EVectorField->TabIndex = 11;
			this->EVectorField->Text = L"Vector Field";
			this->EVectorField->UseVisualStyleBackColor = true;
			this->EVectorField->CheckedChanged += gcnew System::EventHandler(this, &Form1::EVectorField_CheckedChanged);
			// 
			// ReversePolarity
			// 
			this->ReversePolarity->BackColor = System::Drawing::Color::DarkMagenta;
			this->ReversePolarity->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ReversePolarity->Location = System::Drawing::Point(151, 841);
			this->ReversePolarity->Name = L"ReversePolarity";
			this->ReversePolarity->Size = System::Drawing::Size(113, 48);
			this->ReversePolarity->TabIndex = 12;
			this->ReversePolarity->Text = L"Reverse Polarity";
			this->ReversePolarity->UseVisualStyleBackColor = false;
			this->ReversePolarity->Click += gcnew System::EventHandler(this, &Form1::ReversePolarity_Click);
			// 
			// ResetLevel
			// 
			this->ResetLevel->BackColor = System::Drawing::Color::DarkSlateGray;
			this->ResetLevel->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ResetLevel->Location = System::Drawing::Point(22, 284);
			this->ResetLevel->Name = L"ResetLevel";
			this->ResetLevel->Size = System::Drawing::Size(82, 52);
			this->ResetLevel->TabIndex = 13;
			this->ResetLevel->Text = L"Reset Level";
			this->ResetLevel->UseVisualStyleBackColor = false;
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
			this->wallGamePanel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->wallGamePanel1->ForeColor = System::Drawing::Color::LightGray;
			this->wallGamePanel1->Location = System::Drawing::Point(0, 0);
			this->wallGamePanel1->Name = L"wallGamePanel1";
			this->wallGamePanel1->Size = System::Drawing::Size(1836, 1000);
			this->wallGamePanel1->TabIndex = 14;
			this->wallGamePanel1->Visible = false;
			this->wallGamePanel1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::wallGamePanel1_Paint);
			// 
			// noneButton
			// 
			this->noneButton->BackColor = System::Drawing::Color::OldLace;
			this->noneButton->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->noneButton->ForeColor = System::Drawing::Color::MidnightBlue;
			this->noneButton->Location = System::Drawing::Point(281, 883);
			this->noneButton->Name = L"noneButton";
			this->noneButton->Size = System::Drawing::Size(75, 48);
			this->noneButton->TabIndex = 17;
			this->noneButton->Text = L"None";
			this->noneButton->UseVisualStyleBackColor = false;
			this->noneButton->Click += gcnew System::EventHandler(this, &Form1::noneButton_Click);
			// 
			// dragCheckBox
			// 
			this->dragCheckBox->AutoSize = true;
			this->dragCheckBox->BackColor = System::Drawing::Color::SaddleBrown;
			this->dragCheckBox->Checked = true;
			this->dragCheckBox->CheckState = System::Windows::Forms::CheckState::Checked;
			this->dragCheckBox->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->dragCheckBox->Location = System::Drawing::Point(13, 724);
			this->dragCheckBox->Name = L"dragCheckBox";
			this->dragCheckBox->Size = System::Drawing::Size(118, 19);
			this->dragCheckBox->TabIndex = 16;
			this->dragCheckBox->Text = L"Drag Charges";
			this->dragCheckBox->UseVisualStyleBackColor = false;
			this->dragCheckBox->CheckedChanged += gcnew System::EventHandler(this, &Form1::dragCheckBox_CheckedChanged);
			// 
			// Level
			// 
			this->Level->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->Level->Location = System::Drawing::Point(148, 88);
			this->Level->Name = L"Level";
			this->Level->Size = System::Drawing::Size(71, 16);
			this->Level->TabIndex = 15;
			this->Level->Text = L"Level : ";
			// 
			// ExitWallGame
			// 
			this->ExitWallGame->BackColor = System::Drawing::Color::DarkRed;
			this->ExitWallGame->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ExitWallGame->Location = System::Drawing::Point(22, 21);
			this->ExitWallGame->Name = L"ExitWallGame";
			this->ExitWallGame->Size = System::Drawing::Size(94, 42);
			this->ExitWallGame->TabIndex = 14;
			this->ExitWallGame->Text = L"Exit Game";
			this->ExitWallGame->UseVisualStyleBackColor = false;
			this->ExitWallGame->Click += gcnew System::EventHandler(this, &Form1::ExitWallGame_Click);
			// 
			// playWallGame
			// 
			this->playWallGame->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(220)), static_cast<System::Int32>(static_cast<System::Byte>(70)),
				static_cast<System::Int32>(static_cast<System::Byte>(20)));
			this->playWallGame->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->playWallGame->ForeColor = System::Drawing::Color::LightGray;
			this->playWallGame->Location = System::Drawing::Point(732, 465);
			this->playWallGame->Name = L"playWallGame";
			this->playWallGame->Size = System::Drawing::Size(163, 126);
			this->playWallGame->TabIndex = 15;
			this->playWallGame->Text = L"Play Wall Game";
			this->playWallGame->UseVisualStyleBackColor = false;
			this->playWallGame->Click += gcnew System::EventHandler(this, &Form1::playWalllGame_Click);
			// 
			// homePanel
			// 
			this->homePanel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(30)));
			this->homePanel->Controls->Add(this->ElectricSandbox);
			this->homePanel->Controls->Add(this->playWallGame);
			this->homePanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->homePanel->Location = System::Drawing::Point(0, 0);
			this->homePanel->Name = L"homePanel";
			this->homePanel->Size = System::Drawing::Size(1836, 1000);
			this->homePanel->TabIndex = 16;
			// 
			// ElectricSandbox
			// 
			this->ElectricSandbox->BackColor = System::Drawing::Color::DarkSlateGray;
			this->ElectricSandbox->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ElectricSandbox->ForeColor = System::Drawing::Color::LightGray;
			this->ElectricSandbox->Location = System::Drawing::Point(1026, 465);
			this->ElectricSandbox->Name = L"ElectricSandbox";
			this->ElectricSandbox->Size = System::Drawing::Size(177, 126);
			this->ElectricSandbox->TabIndex = 16;
			this->ElectricSandbox->Text = L"Electric Sandbox";
			this->ElectricSandbox->UseVisualStyleBackColor = false;
			this->ElectricSandbox->Click += gcnew System::EventHandler(this, &Form1::ElectricSandbox_Click);
			// 
			// ElectricSandboxPanel
			// 
			this->ElectricSandboxPanel->Controls->Add(this->ActivateTesQWarning);
			this->ElectricSandboxPanel->Controls->Add(this->InspectLable);
			this->ElectricSandboxPanel->Controls->Add(this->TestChargeActivate);
			this->ElectricSandboxPanel->Controls->Add(this->DrawQM);
			this->ElectricSandboxPanel->Controls->Add(this->EFieldLinesGen);
			this->ElectricSandboxPanel->Controls->Add(this->DebugLable);
			this->ElectricSandboxPanel->Controls->Add(this->SandboxInduceMagB);
			this->ElectricSandboxPanel->Controls->Add(this->EFieldLinesCB);
			this->ElectricSandboxPanel->Controls->Add(this->ExitESandboxB);
			this->ElectricSandboxPanel->Controls->Add(this->SandboxVecFieldCB);
			this->ElectricSandboxPanel->Controls->Add(this->SandboxSimulateCB);
			this->ElectricSandboxPanel->Controls->Add(this->SandboxInspectCB);
			this->ElectricSandboxPanel->Controls->Add(this->SandboxDragCB);
			this->ElectricSandboxPanel->Controls->Add(this->SandboxNoneB);
			this->ElectricSandboxPanel->Controls->Add(this->SandboxReversePolB);
			this->ElectricSandboxPanel->Controls->Add(this->SandboxResetChargesB);
			this->ElectricSandboxPanel->Controls->Add(this->SandboxDeleteChargesB);
			this->ElectricSandboxPanel->Controls->Add(this->SandboxAddChargeB);
			this->ElectricSandboxPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->ElectricSandboxPanel->ForeColor = System::Drawing::Color::LightGray;
			this->ElectricSandboxPanel->Location = System::Drawing::Point(0, 0);
			this->ElectricSandboxPanel->Name = L"ElectricSandboxPanel";
			this->ElectricSandboxPanel->Size = System::Drawing::Size(1836, 1000);
			this->ElectricSandboxPanel->TabIndex = 18;
			this->ElectricSandboxPanel->Visible = false;
			this->ElectricSandboxPanel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::ElectricSandboxPanel_Paint);
			// 
			// ActivateTesQWarning
			// 
			this->ActivateTesQWarning->AutoSize = true;
			this->ActivateTesQWarning->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->ActivateTesQWarning->ForeColor = System::Drawing::Color::IndianRed;
			this->ActivateTesQWarning->Location = System::Drawing::Point(1346, 407);
			this->ActivateTesQWarning->Name = L"ActivateTesQWarning";
			this->ActivateTesQWarning->Size = System::Drawing::Size(265, 13);
			this->ActivateTesQWarning->TabIndex = 17;
			this->ActivateTesQWarning->Text = L"ACTIVATE THE TEST CHARGE TO INSPECT";
			this->ActivateTesQWarning->Visible = false;
			// 
			// InspectLable
			// 
			this->InspectLable->BackColor = System::Drawing::Color::AliceBlue;
			this->InspectLable->Font = (gcnew System::Drawing::Font(L"Mongolian Baiti", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->InspectLable->ForeColor = System::Drawing::Color::Black;
			this->InspectLable->Location = System::Drawing::Point(1360, 465);
			this->InspectLable->Name = L"InspectLable";
			this->InspectLable->Size = System::Drawing::Size(202, 143);
			this->InspectLable->TabIndex = 16;
			this->InspectLable->Text = L"Inspect Lable";
			this->InspectLable->Visible = false;
			// 
			// TestChargeActivate
			// 
			this->TestChargeActivate->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->TestChargeActivate->AutoSize = true;
			this->TestChargeActivate->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->TestChargeActivate->Location = System::Drawing::Point(1627, 405);
			this->TestChargeActivate->Name = L"TestChargeActivate";
			this->TestChargeActivate->Size = System::Drawing::Size(171, 19);
			this->TestChargeActivate->TabIndex = 15;
			this->TestChargeActivate->Text = L"Activate Test Charge";
			this->TestChargeActivate->UseVisualStyleBackColor = true;
			this->TestChargeActivate->CheckedChanged += gcnew System::EventHandler(this, &Form1::TestChargeActivate_CheckedChanged);
			// 
			// DrawQM
			// 
			this->DrawQM->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->DrawQM->AutoSize = true;
			this->DrawQM->Checked = true;
			this->DrawQM->CheckState = System::Windows::Forms::CheckState::Checked;
			this->DrawQM->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->DrawQM->Location = System::Drawing::Point(1631, 589);
			this->DrawQM->Name = L"DrawQM";
			this->DrawQM->Size = System::Drawing::Size(118, 19);
			this->DrawQM->TabIndex = 14;
			this->DrawQM->Text = L"Draw Objects";
			this->DrawQM->UseVisualStyleBackColor = true;
			this->DrawQM->CheckedChanged += gcnew System::EventHandler(this, &Form1::DrawQM_CheckedChanged);
			// 
			// EFieldLinesGen
			// 
			this->EFieldLinesGen->AllowDrop = true;
			this->EFieldLinesGen->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->EFieldLinesGen->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(30)));
			this->EFieldLinesGen->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->EFieldLinesGen->ForeColor = System::Drawing::Color::WhiteSmoke;
			this->EFieldLinesGen->FormattingEnabled = true;
			this->EFieldLinesGen->Items->AddRange(gcnew cli::array< System::Object^  >(4) {
				L"Random Field", L"Grid Generation", L"Random from Charge",
					L"Radial Generation"
			});
			this->EFieldLinesGen->Location = System::Drawing::Point(1631, 363);
			this->EFieldLinesGen->Name = L"EFieldLinesGen";
			this->EFieldLinesGen->Size = System::Drawing::Size(121, 23);
			this->EFieldLinesGen->TabIndex = 13;
			this->EFieldLinesGen->Text = L"Field Line Generation";
			this->EFieldLinesGen->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::EFieldLinesGen_SelectedIndexChanged);
			// 
			// DebugLable
			// 
			this->DebugLable->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->DebugLable->AutoSize = true;
			this->DebugLable->Location = System::Drawing::Point(1675, 309);
			this->DebugLable->Name = L"DebugLable";
			this->DebugLable->Size = System::Drawing::Size(35, 13);
			this->DebugLable->TabIndex = 12;
			this->DebugLable->Text = L"label1";
			// 
			// SandboxInduceMagB
			// 
			this->SandboxInduceMagB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->SandboxInduceMagB->BackColor = System::Drawing::Color::DarkBlue;
			this->SandboxInduceMagB->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SandboxInduceMagB->Location = System::Drawing::Point(1536, 773);
			this->SandboxInduceMagB->Name = L"SandboxInduceMagB";
			this->SandboxInduceMagB->Size = System::Drawing::Size(120, 61);
			this->SandboxInduceMagB->TabIndex = 11;
			this->SandboxInduceMagB->Text = L"Induce a Changing Magnetic Field";
			this->SandboxInduceMagB->UseVisualStyleBackColor = false;
			this->SandboxInduceMagB->Click += gcnew System::EventHandler(this, &Form1::SandboxInduceMagB_Click);
			// 
			// EFieldLinesCB
			// 
			this->EFieldLinesCB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->EFieldLinesCB->AutoSize = true;
			this->EFieldLinesCB->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->EFieldLinesCB->Location = System::Drawing::Point(1631, 338);
			this->EFieldLinesCB->Name = L"EFieldLinesCB";
			this->EFieldLinesCB->Size = System::Drawing::Size(114, 19);
			this->EFieldLinesCB->TabIndex = 10;
			this->EFieldLinesCB->Text = L"E Field Lines";
			this->EFieldLinesCB->UseVisualStyleBackColor = true;
			this->EFieldLinesCB->CheckedChanged += gcnew System::EventHandler(this, &Form1::EFieldLinesCB_CheckedChanged);
			// 
			// ExitESandboxB
			// 
			this->ExitESandboxB->BackColor = System::Drawing::Color::Maroon;
			this->ExitESandboxB->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->ExitESandboxB->Location = System::Drawing::Point(29, 21);
			this->ExitESandboxB->Name = L"ExitESandboxB";
			this->ExitESandboxB->Size = System::Drawing::Size(122, 59);
			this->ExitESandboxB->TabIndex = 9;
			this->ExitESandboxB->Text = L"Exit Electric Sandbox";
			this->ExitESandboxB->UseVisualStyleBackColor = false;
			this->ExitESandboxB->Click += gcnew System::EventHandler(this, &Form1::ExitESandboxB_Click);
			// 
			// SandboxVecFieldCB
			// 
			this->SandboxVecFieldCB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->SandboxVecFieldCB->AutoSize = true;
			this->SandboxVecFieldCB->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SandboxVecFieldCB->Location = System::Drawing::Point(1631, 544);
			this->SandboxVecFieldCB->Name = L"SandboxVecFieldCB";
			this->SandboxVecFieldCB->Size = System::Drawing::Size(111, 19);
			this->SandboxVecFieldCB->TabIndex = 8;
			this->SandboxVecFieldCB->Text = L"Vector Field";
			this->SandboxVecFieldCB->UseVisualStyleBackColor = true;
			// 
			// SandboxSimulateCB
			// 
			this->SandboxSimulateCB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->SandboxSimulateCB->AutoSize = true;
			this->SandboxSimulateCB->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SandboxSimulateCB->Location = System::Drawing::Point(1700, 38);
			this->SandboxSimulateCB->Name = L"SandboxSimulateCB";
			this->SandboxSimulateCB->Size = System::Drawing::Size(87, 19);
			this->SandboxSimulateCB->TabIndex = 7;
			this->SandboxSimulateCB->Text = L"Simulate";
			this->SandboxSimulateCB->UseVisualStyleBackColor = true;
			// 
			// SandboxInspectCB
			// 
			this->SandboxInspectCB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->SandboxInspectCB->AutoSize = true;
			this->SandboxInspectCB->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SandboxInspectCB->Location = System::Drawing::Point(1631, 495);
			this->SandboxInspectCB->Name = L"SandboxInspectCB";
			this->SandboxInspectCB->Size = System::Drawing::Size(77, 19);
			this->SandboxInspectCB->TabIndex = 6;
			this->SandboxInspectCB->Text = L"Inspect";
			this->SandboxInspectCB->UseVisualStyleBackColor = true;
			// 
			// SandboxDragCB
			// 
			this->SandboxDragCB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->SandboxDragCB->AutoSize = true;
			this->SandboxDragCB->Checked = true;
			this->SandboxDragCB->CheckState = System::Windows::Forms::CheckState::Checked;
			this->SandboxDragCB->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SandboxDragCB->Location = System::Drawing::Point(1631, 635);
			this->SandboxDragCB->Name = L"SandboxDragCB";
			this->SandboxDragCB->Size = System::Drawing::Size(118, 19);
			this->SandboxDragCB->TabIndex = 5;
			this->SandboxDragCB->Text = L"Drag Charges";
			this->SandboxDragCB->UseVisualStyleBackColor = true;
			// 
			// SandboxNoneB
			// 
			this->SandboxNoneB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->SandboxNoneB->BackColor = System::Drawing::Color::OldLace;
			this->SandboxNoneB->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SandboxNoneB->ForeColor = System::Drawing::Color::Black;
			this->SandboxNoneB->Location = System::Drawing::Point(1396, 883);
			this->SandboxNoneB->Name = L"SandboxNoneB";
			this->SandboxNoneB->Size = System::Drawing::Size(120, 61);
			this->SandboxNoneB->TabIndex = 4;
			this->SandboxNoneB->Text = L"None";
			this->SandboxNoneB->UseVisualStyleBackColor = false;
			// 
			// SandboxReversePolB
			// 
			this->SandboxReversePolB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->SandboxReversePolB->BackColor = System::Drawing::Color::Indigo;
			this->SandboxReversePolB->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SandboxReversePolB->Location = System::Drawing::Point(1678, 846);
			this->SandboxReversePolB->Name = L"SandboxReversePolB";
			this->SandboxReversePolB->Size = System::Drawing::Size(120, 63);
			this->SandboxReversePolB->TabIndex = 3;
			this->SandboxReversePolB->Text = L"Reverse Polarity";
			this->SandboxReversePolB->UseVisualStyleBackColor = false;
			// 
			// SandboxResetChargesB
			// 
			this->SandboxResetChargesB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->SandboxResetChargesB->BackColor = System::Drawing::Color::Maroon;
			this->SandboxResetChargesB->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SandboxResetChargesB->Location = System::Drawing::Point(1678, 922);
			this->SandboxResetChargesB->Name = L"SandboxResetChargesB";
			this->SandboxResetChargesB->Size = System::Drawing::Size(120, 66);
			this->SandboxResetChargesB->TabIndex = 2;
			this->SandboxResetChargesB->Text = L"Reset Charges";
			this->SandboxResetChargesB->UseVisualStyleBackColor = false;
			// 
			// SandboxDeleteChargesB
			// 
			this->SandboxDeleteChargesB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->SandboxDeleteChargesB->BackColor = System::Drawing::Color::Maroon;
			this->SandboxDeleteChargesB->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SandboxDeleteChargesB->Location = System::Drawing::Point(1536, 922);
			this->SandboxDeleteChargesB->Name = L"SandboxDeleteChargesB";
			this->SandboxDeleteChargesB->Size = System::Drawing::Size(120, 63);
			this->SandboxDeleteChargesB->TabIndex = 1;
			this->SandboxDeleteChargesB->Text = L"Delete Charges";
			this->SandboxDeleteChargesB->UseVisualStyleBackColor = false;
			// 
			// SandboxAddChargeB
			// 
			this->SandboxAddChargeB->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->SandboxAddChargeB->BackColor = System::Drawing::Color::DarkGreen;
			this->SandboxAddChargeB->Font = (gcnew System::Drawing::Font(L"Lucida Fax", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->SandboxAddChargeB->ForeColor = System::Drawing::Color::LightGray;
			this->SandboxAddChargeB->Location = System::Drawing::Point(1536, 846);
			this->SandboxAddChargeB->Name = L"SandboxAddChargeB";
			this->SandboxAddChargeB->Size = System::Drawing::Size(120, 63);
			this->SandboxAddChargeB->TabIndex = 0;
			this->SandboxAddChargeB->Text = L"Add Charges";
			this->SandboxAddChargeB->UseVisualStyleBackColor = false;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)),
				static_cast<System::Int32>(static_cast<System::Byte>(30)));
			this->ClientSize = System::Drawing::Size(1836, 1000);
			this->Controls->Add(this->wallGamePanel1);
			this->Controls->Add(this->ElectricSandboxPanel);
			this->Controls->Add(this->homePanel);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->TopMost = true;
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::Form1_Paint);
			this->wallGamePanel1->ResumeLayout(false);
			this->wallGamePanel1->PerformLayout();
			this->homePanel->ResumeLayout(false);
			this->ElectricSandboxPanel->ResumeLayout(false);
			this->ElectricSandboxPanel->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	
		void ChargesUsed() {
			QUsed = prevQUsed + Q->size(); // the prevQUsed is so that when you move to next level you still used those
		}
		void QleftLableUpdate() {
			ChargesUsed();
			Qleft = maxQN - QUsed; 
			chargesLeftLabel->Text = "Charges left : " + Qleft.ToString();
		}
		void deleteQ() {
			for (int i = Q->size() - 1; i >= 0; --i) {
				Q->erase(Q->begin() + i);
			}
		}
		void inspectLable() {
			InspectLable->Text = "Electric Field Magnitude = " + Emag(totEat(*Q, *mag, xs, ys)).ToString("F2") + "\n\n" +
			"Electric Field Direction = " + EdirInDeg(totEat(*Q,*mag,xs,ys)).ToString("F2") + "° \n\n" +
				"Curl E = " + computeCurl(*Q,*mag,xs,ys).ToString("F5") + "\n\n" +
				"Div E = " + computeDivergence(*Q,*mag,xs,ys).ToString("F2") + "\n\n" +
				"Grad |E| x = " + computeGradMag(*Q,*mag,xs,ys).getX().ToString("F2") + "\nGrad |E| y = " + computeGradMag(*Q, *mag, xs, ys).getY().ToString("F2");
			InspectLable->Location = Point(xs+testD/2, ys+testD/2);
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
			deleteMag(*mag);
			x = startX;
			y = startY;
			testCharge->setPos(Vec2D(x, y));
			collision = false;
			testCharge->setVx(0);
			testCharge->setVy(0);
			if (doEField || sandDoEField) { EField->updateEfield(*Q, *mag); *qCenters = getQCenters(*Q); }
			
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
			Pen^ fArrowPen = gcnew Pen(Color::FromArgb(255, 1 - intensity(*testqField), 0), 1);
			//Brush^ wallBrush = gcnew SolidBrush(Color::FromArgb(200, 100, 0));


			//test charge draw
			 testCharge->fill(g); 
			//E field draw (before stationary charges to hide arrows over them)
			if (doEField) {
				EField->draw(g, EFMaxSize, minSize, EFScale);
			}
			//All stationary charges
			for (int i = 0; i < Q->size(); ++i) {
				double cx = (*Q)[i].getX();
				double cy = (*Q)[i].getY();
				(*Q)[i].fill(g);
			}

			//Draw force arrow if there is a force acting on test charge. THESE CONDITIONS CAN BE CHANGED TO A BUTTON CLIKC (to say if you want thea arrows to show or not)
			if (testqField->getX() != 0 || testqField->getY() != 0) {
				DrawArrow(g, x - testD / 2, y - testD / 2, totFDir(*testqField), testD, arSize(*testqField, qFarMaxSize, minSize, gameqFarScale), fArrowPen);
			}
			//Draw vel arrow if the test charge is moving
			if (testCharge->getVx() != 0 || testCharge->getVy() != 0) {
				DrawArrow(g, x - testD / 2, y - testD / 2, testVDir(*testCharge), testD, arSize(testCharge->getV(), qVarMaxSize,minSize, qVarScale), vArrowPen);
			}

			//Draw a circle following the mouse
			if (mode == Mode::addCharge) {
				if (wallGamePanel1->Visible) g->DrawEllipse(dragPen, (float)mouseX - Qd / 2, (float)mouseY - Qd / 2, Qd, Qd);
				else if (ElectricSandboxPanel->Visible) g->DrawEllipse(dragPen, (float)mouseX - (float)sandQd / 2, (float)mouseY - sandQd / 2, (float)sandQd, (float)sandQd);
			}
			//walls
			if (doWalls) {
				Walls->draw(g);
			}

		}

		void drawSandbox(Graphics^ g) {
			Pen^ pen = gcnew Pen(Color::Black, 1);
			Pen^ dragPen = gcnew Pen(Color::Gray, 2);
			Pen^ shapePen = gcnew Pen(Color::White, 3);
			Brush^ testQBrush = gcnew SolidBrush(Color::SeaGreen);

			Pen^ vArrowPen = gcnew Pen(Color::FromArgb(0, 255, 0), 1);
			Pen^ fArrowPen = gcnew Pen(Color::FromArgb(255, 0, 0), 1);
			//keep the drawing order
			//E field draw (before stationary charges to hide arrows over them)
			if (sandDoEField && (Q->size() >0 || mag->size() >0)) {
				EField->draw(g, EFMaxSize, minSize, EFScale);
			}
			//All stationary charges
			if (drawQandM) {
				for (int i = 0; i < Q->size(); ++i) {
					double cx = (*Q)[i].getX();
					double cy = (*Q)[i].getY();
					(*Q)[i].fill(g);
				}
				//All induced Mag fields
				for (int i = 0; i < mag->size(); ++i) {
					(*mag)[i].draw(g);
				}
			}
			//test charge station and draw
			g->DrawRectangle(shapePen, ElectricSandboxPanel->Width - 150, ElectricSandboxPanel->Height *0.45, testCharge->getDiameter()+10, testCharge->getDiameter()+10);
			if (!activeTestQ) g->FillEllipse(testQBrush, ElectricSandboxPanel->Width - 150 + testD / 2 -5, ElectricSandboxPanel->Height * 0.45 + testD / 2-5 , testD, testD);
			else if (activeTestQ) {
				sandboxTestCharge->fill(g);
				//Draw force arrow if there is a force acting on test charge. THESE CONDITIONS CAN BE CHANGED TO A BUTTON CLIKC (to say if you want thea arrows to show or not)
				//Vec2D field = totEat(*Q, *mag, sandboxTestCharge->getX(), sandboxTestCharge->getY());
				if (testqField->getX() != 0 || testqField->getY() != 0) {
					
					DrawArrow(g, xs - testD / 2, ys - testD / 2, totFDir(*testqField), testD, arSize(*testqField, qFarMaxSize, qfMinSize,  qFarScale), fArrowPen);
				}
				//Draw vel arrow if the test charge is moving
				if (sandboxTestCharge->getVx() != 0 || sandboxTestCharge->getVy() != 0) {
					DrawArrow(g, xs - testD / 2, ys - testD / 2, testVDir(*sandboxTestCharge), testD, arSize(testCharge->getV(), qVarMaxSize,qfMinSize, qVarScale), vArrowPen);
				}
			}
			
			
			
			//Draw a circle following the mouse
			if (mode == Mode::addCharge) {
				if(wallGamePanel1->Visible ) g->DrawEllipse(dragPen, (float)mouseX - Qd / 2, (float)mouseY - Qd / 2, Qd, Qd);
				else if(ElectricSandboxPanel ->Visible) g->DrawEllipse(dragPen, (float)mouseX - (float)sandQd / 2, (float)mouseY - sandQd / 2, (float)sandQd, (float)sandQd);
			}
			if (mode == Mode::induceMag) {
				if (ElectricSandboxPanel->Visible)  g->DrawEllipse(dragPen, (float)mouseX - (float)magDia / 2, (float)mouseY - magDia / 2, (float)magDia, (float)magDia);
				drawXinO(g, dragPen, mouseX, mouseY, magDia);
			}
			
			if (doEfieldLines) { 
				
				drawElines(g,*eLine, *Q, *mag); }
		}
		bool negQPresent(vector<Charge> Q) {
			for (int i = 0; i < Q.size(); ++i) {
				if (Q[i].getQ() < 0) { return true; }
				else continue;
			}
			return false;
		}

		void ShowPanel(Panel^ target) {
			homePanel->Visible = false;
			wallGamePanel1->Visible = false;
			ElectricSandboxPanel->Visible = false;

			target->Visible = true;
			target->BringToFront();
		}

	private: System::Void Form1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
	}
		  
	private: void Tick(Object^ sender, EventArgs^ e) {
		double dt = 0.05;
		if (wallGamePanel1->Visible) {
			levelUpdate();
			QleftLableUpdate();
			Vec2D force = ForceCalc(*Q, x, y, *testCharge);
			testqField->setX(force.getX()); // updating global F, for test charge arrow purposes
			testqField->setY(force.getY());
			if (play) {
				Vec2D c = dqCords(*testCharge, force, dt);
				x += c.getX();
				y += c.getY();
				testCharge->setPos(Vec2D(x, y));
				if (y >= wallGamePanel1->Height - testD) y = wallGamePanel1->Height - testD;
				//if (x >= gameWidth - testD) x = gameWidth - testD;
				if (x <= 0) x = 0;
				if (y <= 0) y = 0;
				wallCollisionCheck();
				if (collision) { lostLevel(); }
				if (x >= wallGamePanel1->Width - testD) { goalReached(); }
				dragQ = false;
				//mode = Mode::none;
			}
			
		}
		if (ElectricSandboxPanel->Visible && activeTestQ) { *testqField = Vec2D(totEat(*Q, *mag, sandboxTestCharge->getX(), sandboxTestCharge->getY())); }
		else if(!wallGamePanel1->Visible && !ElectricSandbox->Visible)*testqField = Vec2D(0, 0);
		//drag
		if (mode == Mode::drag) {
			if (dragQ) { (*Q)[dragIndex].setPos(Vec2D(mouseX, mouseY)); } 
			if (dragMag) { (*mag)[dragIndex].setPos(Vec2D(mouseX, mouseY)); }
			if (dragTestQ) {
			xs = mouseX;
			ys = mouseY;
			sandboxTestCharge->setPos(Vec2D(mouseX, mouseY));
			}
		}
		//simulate all Q
		if (simulateAllQ) {
			simulateAllQcord(*Q, *mag, dt);
			allQscreenCollision(*Q,ElectricSandboxPanel->Width, ElectricSandboxPanel->Height);
			if (sandDoEField) {EField->updateEfield(*Q, *mag); *qCenters = getQCenters(*Q);	}
		}
		//E Field lines generation
		if (doEfieldLines) {
			if (generation == gen::RadialGen) {
				++tickCount;
				if (tickCount >= maxTick) {
					*qCenters = getQCenters(*Q);
					*EfatQ = getEatQ(*Q);
					generateLines2(*eLine, *qCenters, fieldLgenerated, maxLife, maxLength, minLength, *Q);
					//initialElineGen(*eLine, lineFieldColl, lineFieldRow, lineHspacing, lineVspacing,maxLife, minLength);
					tickCount = 0;
					DebugLable->Text = "lines alive = " + eLine->size().ToString();
					if (negQPresent(*Q)) { lineAtEdgeGen(*eLine, ElectricSandboxPanel->Width, ElectricSandboxPanel->Height, lineHspacing, lineVspacing, maxLife, minLength); }
					drawEfieldLines = true;
				}
			}
			if (generation == gen::RandomCharge) {
				//rotFieldLines
				++tickCount;
				if (tickCount >= maxTick) {
					*qCenters = getQCenters(*Q);
					*EfatQ = getEatQ(*Q);
					++genCycle;
					RotgenerateLines2(*eLine, *qCenters, *EfatQ, fieldLgenerated, maxLife, maxLength, minLength, *Q, genCycle);
					tickCount = 0;
					//initialElineGen(*eLine, lineFieldColl, lineFieldRow, lineHspacing, lineVspacing, maxLife, minLength);
					drawEfieldLines = true;
				}
			}
			if (generation == gen::GridGen) {
				++tickCount;
				if (tickCount >= maxTick) {
					niceFieldGen(*eLine, *Q, *mag, lineFieldColl, lineFieldRow, lineHspacing, lineVspacing, maxLife, minLength, genCycle);
					++genCycle;
					tickCount = 0;
					if (genCycle >= 5) genCycle = 1; //restart cycle
					drawEfieldLines = true;
				}
			}
			if (generation == gen::RandomField) {
				randFieldGen(*eLine, *Q, *mag, fieldLgenerated, ElectricSandboxPanel->Width, ElectricSandboxPanel->Height, maxLife);
				drawEfieldLines = true;
			}
	
		}

		//E field lines update
		if (drawEfieldLines) {
			DebugLable->Text = "lines alive = " + eLine->size().ToString();
			fieldLineUpdateGridVersion(*eLine, *Q, *mag, sandQd, baseSpeed, dt, maxLength, minLength, maxLife, dragQ,dragMag);
			updateLineLife(*eLine, *Q,*mag, ElectricSandboxPanel->Width, ElectricSandboxPanel->Height);
		}
		// Arrow field update
		if ((doEField || sandDoEField) && mode == Mode::drag) { EField->updateEfield(*Q,*mag); *qCenters = getQCenters(*Q); }
		if (ElectricSandboxPanel->Visible)
			ElectricSandboxPanel->Invalidate();
		else if (wallGamePanel1->Visible)
			wallGamePanel1->Invalidate();
		//inspect
		if (inspect&& activeTestQ) {
			ActivateTesQWarning->Visible = false;
			InspectLable->Visible = true;
			inspectLable();
		}
		else {
			InspectLable->Visible = false;
		}
	}
		   //play checkbox
	private: System::Void checkBox1_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		play = checkBox1->Checked;
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
	   
	   if (mode == Mode::addCharge) {
		   if (ElectricSandboxPanel->Visible) { Q->push_back(Charge(1, 1, sandQd, Vec2D(mx, my), Vec2D(0, 0), Q->size() - 1)); }
		   else if (wallGamePanel1->Visible && Qleft > 0) { Q->push_back(Charge(1, 1, Qd, Vec2D(mx, my), Vec2D(0, 0), Q->size() - 1)); }
		   if (doEField || sandDoEField){ EField->updateEfield(*Q, *mag); *qCenters = getQCenters(*Q);}
		}
		if (mode == Mode::induceMag) {
			if (ElectricSandboxPanel->Visible) { mag->push_back(inducMag(dbdt, Vec2D(mouseX, mouseY), magDia, in, mag->size() - 1)); }
			
			if (doEField || sandDoEField) EField->updateEfield(*Q, *mag);
		}
		if (mode == Mode::deleteCharges ) {
			//for charges
			for (int i = Q->size() - 1; i >= 0; --i) {
				if(inCircle((*Q)[i].getX(), (*Q)[i].getY(), (*Q)[i].getRad(), mx, my)) { //Check if mouse is in diameter of that charge
					Q->erase(Q->begin() + i);
					if (doEField || sandDoEField) { EField->updateEfield(*Q, *mag); *qCenters = getQCenters(*Q); }
				}
				
			}
			//for magnets
			for (int i = mag->size() - 1; i >= 0; --i) {
				if (inCircle((*mag)[i].getX(), (*mag)[i].getY(), (*mag)[i].getRad(), mx, my)) {
					mag->erase(mag->begin() + i);
					if (doEField || sandDoEField) { EField->updateEfield(*Q, *mag); }
				}
			}
		}
		 if(wallGamePanel1->Visible) wallGamePanel1-> Invalidate();
		 if (ElectricSandboxPanel->Visible) ElectricSandboxPanel->Invalidate();
	}
	
	void Mdown(Object^ sender, MouseEventArgs^ e) {
		double mx = e->X;
		double my = e->Y;
		//first revrse polarity then drag cuz otherwise you just drag since mode gets set to drag
		if (mode == Mode::reversePolarity) {
			for (int i = Q->size() - 1; i >= 0; --i) {
				if (inCircle((*Q)[i].getX(), (*Q)[i].getY(), (*Q)[i].getRad(), mx, my)) { //if mose on top of charge
					(*Q)[i].setQ((*Q)[i].getQ() * -1); //multiplying the charge's q by -1
				}
				if (doEField || sandDoEField) EField->updateEfield(*Q, *mag);
			}
			for (int i = mag->size() - 1; i >= 0; --i) {
				if (inCircle((*mag)[i].getX(), (*mag)[i].getY(), (*mag)[i].getRad(), mx, my)) {
					(*mag)[i].inverse();
				}
				if (doEField || sandDoEField) EField->updateEfield(*Q, *mag);
			}
		}
		if (drag|| sandDrag) {
			if (prevMode == Mode::deleteCharges) return;
			for (int i = Q->size() - 1; i >= 0; --i) { //drag Charges
				if (inCircle((*Q)[i].getX(), (*Q)[i].getY(), (*Q)[i].getRad(), mx, my)) { //if mouse on top of charge
					mode = Mode::drag; //now actually drag
					dragIndex = i; //to know which charge to drag
					dragQ = true;
					dragMag = false; // so that you dont also drag that index of mag
					dragTestQ = false;
				}
			}//drag mags
			for (int i = mag->size() - 1; i >= 0; --i) {
				if (inCircle((*mag)[i].getX(), (*mag)[i].getY(), (*mag)[i].getRad(), mx, my)) {
					mode = Mode::drag; //now actually drag
					dragIndex = i; //to know which charge to drag
					dragMag = true;
					dragQ = false;
					dragTestQ = false;
				}
			}//drag test Q
			if (inCircle(sandboxTestCharge->getX(), sandboxTestCharge->getY(), sandboxTestCharge->getRad(), mx, my) && activeTestQ) {
				mode = Mode::drag; //now actually drag
				dragIndex = -1; //so that you dont drag another charge accidently, testcharge index doesnt matter
				dragMag = false;
				dragQ = false;
				dragTestQ = true;
			}
		}
		
	}
	void Mup(Object^ sender, MouseEventArgs^ e) {
		if(mode == Mode::drag && play == false) mode = prevMode; //to stop draging charge when releasing mouse and back to what you where doing. Also, the play = false is so that you cant do anything while play simulation is on (otherwise if you click on charge mode = prevMode)
		dragQ = false;
		dragMag = false;
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
		deleteMag(*mag);
		if (doEField) { EField->updateEfield(*Q, *mag); *qCenters = getQCenters(*Q); }
	}
	private: System::Void TempGoal_Click(System::Object^ sender, System::EventArgs^ e) {
		prevQUsed = QUsed;
		Walls->generate(screenWidth, screenHeight);
		restartLevel();
		level += 1;
	}
	private: System::Void EVectorField_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		doEField = EVectorField->Checked;
		if (EVectorField->Checked) { EField->generate(*Q, *mag, wallGamePanel1->Width, wallGamePanel1->Height); }
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
		ShowPanel(wallGamePanel1);
		timer->Start();
		mode = Mode::none;
		prevMode = Mode::none;
		// FIRST set difficulty
		applyDifficulty();
			Walls->generate(gameWidth, gameHeight);
			doWalls = true;
			EVectorField->Checked = false;
			doEField = false;
			dragCheckBox->Checked = true;
	}
	private: System::Void wallGamePanel1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
		Graphics^ g = e->Graphics;
		drawGame(g);
	}
	private: System::Void ExitWallGame_Click(System::Object^ sender, System::EventArgs^ e) {
		ShowPanel(homePanel);
		level = 1;
		Qleft = maxQN;
		prevQUsed = 0;
		dragCheckBox->Checked = false;
		deleteQ();
		deleteMag(*mag);
		
	}
		   private: System::Void dragCheckBox_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			   drag = dragCheckBox->Checked;
		   }
private: System::Void noneButton_Click(System::Object^ sender, System::EventArgs^ e) {
	mode = Mode::none;
	prevMode = Mode::none;
}
//Sandbox----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

private: System::Void ElectricSandbox_Click(System::Object^ sender, System::EventArgs^ e) {
	ShowPanel(ElectricSandboxPanel);
	mode = Mode::none;
	prevMode = Mode::none;
	timer->Start();
}
private: System::Void SandboxAddChargeB_Click(System::Object^ sender, System::EventArgs^ e) {
	mode = Mode::addCharge;
	prevMode = Mode::addCharge;
	ElectricSandboxPanel->Invalidate();
}
private: System::Void SandboxDeleteChargesB_Click(System::Object^ sender, System::EventArgs^ e) {
	mode = Mode::deleteCharges;
	prevMode = Mode::deleteCharges;
	ElectricSandboxPanel->Invalidate();
}

private: System::Void SandboxReversePolB_Click(System::Object^ sender, System::EventArgs^ e) {
	mode = Mode::reversePolarity;
	prevMode = Mode::reversePolarity;
	ElectricSandboxPanel->Invalidate();
}
private: System::Void SandboxResetChargesB_Click(System::Object^ sender, System::EventArgs^ e) {
	deleteQ();
	deleteMag(*mag);
	if (sandDoEField) EField->updateEfield(*Q,*mag); *qCenters = getQCenters(*Q);
	SandboxSimulateCB->Checked = false; //it was crashing if while you simulate you reset charges, and then add a charge
}
private: System::Void SandboxNoneB_Click(System::Object^ sender, System::EventArgs^ e) {
	mode = Mode::none;
	prevMode = Mode::none;
	ElectricSandboxPanel->Invalidate(); //it was not invalidating for some reaoson
}
private: System::Void SandboxSimulateCB_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	simulateAllQ = SandboxSimulateCB->Checked;
	ElectricSandboxPanel->Invalidate();
}
private: System::Void SandboxDragCB_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	sandDrag = SandboxDragCB->Checked;
	ElectricSandboxPanel->Invalidate();
}
private: System::Void SandboxInspectCB_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	inspect = SandboxInspectCB->Checked;
	if (!activeTestQ) {
		ActivateTesQWarning->Visible = true;
	}
	if (!inspect) { ActivateTesQWarning->Visible = false; }
	ElectricSandboxPanel->Invalidate();
}
private: System::Void SandboxVecFieldCB_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	sandDoEField = SandboxVecFieldCB->Checked;
	if (SandboxVecFieldCB->Checked) { EField->generate(*Q, *mag, ElectricSandboxPanel->Width, ElectricSandboxPanel->Height); }
	if (SandboxVecFieldCB->Checked == false) { EField->deleteEF(); }
	ElectricSandboxPanel->Invalidate();
}
private: System::Void ElectricSandboxPanel_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
	Graphics^ g = e->Graphics;
	drawSandbox(g);
	
}
private: System::Void ExitESandboxB_Click(System::Object^ sender, System::EventArgs^ e) {
	ShowPanel(homePanel);

	SandboxDragCB->Checked = false;
	EFieldLinesCB->Checked = false;
	SandboxVecFieldCB->Checked = false;
	deleteQ();
	deleteMag(*mag);
	mode = Mode::none;
	prevMode = Mode::none;
	sandDoEField = false;
}
private: System::Void EFieldLinesCB_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	doEfieldLines = EFieldLinesCB->Checked;
	if (EFieldLinesCB->Checked == false) {
		eLine->clear();
	}
	//if (doEfieldLines) initialElineGen(*eLine, lineFieldColl, lineFieldRow, lineHspacing, lineVspacing, maxLife, minLength);
}
private: System::Void SandboxInduceMagB_Click(System::Object^ sender, System::EventArgs^ e) {
	mode = Mode::induceMag;
	prevMode = Mode::induceMag;
}
private: System::Void EFieldLinesGen_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	if ("Random Field" == EFieldLinesGen->SelectedItem->ToString()) {
		generation = gen::RandomField;
		maxLife = 100;
		fieldLgenerated = 3;
		//doEfieldLines = true;
	}
	if ("Grid Generation" == EFieldLinesGen->SelectedItem->ToString()) {
		generation = gen::GridGen;
		maxLife = 100;
		tickCount = 20;
		maxTick = 50;
		//doEfieldLines = true;
	}
	if ("Random from Charge" == EFieldLinesGen->SelectedItem->ToString()) {
		generation = gen::RandomCharge;
		maxLife = 10000;
		tickCount = 0;
		maxTick = 5;
		fieldLgenerated = 3;
	//	doEfieldLines = true;
	}
	if ("Radial Generation" == EFieldLinesGen->SelectedItem->ToString()) {
		generation = gen::RadialGen;
		maxLife = 10000;
		tickCount = 50;
		maxTick = 100;
		fieldLgenerated = 50;
		//doEfieldLines = true;
	}
}
private: System::Void DrawQM_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	drawQandM=DrawQM->Checked;
}
private: System::Void TestChargeActivate_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	activeTestQ = TestChargeActivate->Checked;
}
};
}



