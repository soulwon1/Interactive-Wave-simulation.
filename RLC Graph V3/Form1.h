#pragma once

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
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^ ResistanceLabel;
	private: System::Windows::Forms::Label^ InductanceLabel;
	private: System::Windows::Forms::Label^ CapacitanceLabel;
	private: System::Windows::Forms::TextBox^ tbR;
	private: System::Windows::Forms::TextBox^ tbL;
	private: System::Windows::Forms::TextBox^ tbC;
	private: System::Windows::Forms::Button^ btnSimulate;
	private: System::Windows::Forms::PictureBox^ picGraph;
	private: System::Windows::Forms::Timer^ simTimer;
	private: array<double>^ yPoints;   // stores all the y values
	private: int drawIndex;            // tracks how far we've drawn
	private: System::Drawing::Bitmap^ bmp;
	private: System::Drawing::Graphics^ g;
	private: Pen^ graphPen;
	private: System::Windows::Forms::Timer^ timer1;
	private: System::Windows::Forms::Label^ Title;


	private: System::ComponentModel::IContainer^ components;
	protected:

	protected:








	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->ResistanceLabel = (gcnew System::Windows::Forms::Label());
			this->InductanceLabel = (gcnew System::Windows::Forms::Label());
			this->CapacitanceLabel = (gcnew System::Windows::Forms::Label());
			this->tbR = (gcnew System::Windows::Forms::TextBox());
			this->tbL = (gcnew System::Windows::Forms::TextBox());
			this->tbC = (gcnew System::Windows::Forms::TextBox());
			this->btnSimulate = (gcnew System::Windows::Forms::Button());
			this->picGraph = (gcnew System::Windows::Forms::PictureBox());
			this->simTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->Title = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picGraph))->BeginInit();
			this->SuspendLayout();
			// 
			// ResistanceLabel
			// 
			this->ResistanceLabel->AutoSize = true;
			this->ResistanceLabel->Location = System::Drawing::Point(20, 135);
			this->ResistanceLabel->Name = L"ResistanceLabel";
			this->ResistanceLabel->Size = System::Drawing::Size(134, 16);
			this->ResistanceLabel->TabIndex = 0;
			this->ResistanceLabel->Text = L"Resistance R (Ohms)";
			this->ResistanceLabel->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
			// 
			// InductanceLabel
			// 
			this->InductanceLabel->AutoSize = true;
			this->InductanceLabel->Location = System::Drawing::Point(20, 175);
			this->InductanceLabel->Name = L"InductanceLabel";
			this->InductanceLabel->Size = System::Drawing::Size(103, 16);
			this->InductanceLabel->TabIndex = 1;
			this->InductanceLabel->Text = L"Inductance L (H)";
			// 
			// CapacitanceLabel
			// 
			this->CapacitanceLabel->AutoSize = true;
			this->CapacitanceLabel->Location = System::Drawing::Point(20, 215);
			this->CapacitanceLabel->Name = L"CapacitanceLabel";
			this->CapacitanceLabel->Size = System::Drawing::Size(114, 16);
			this->CapacitanceLabel->TabIndex = 2;
			this->CapacitanceLabel->Text = L"Capacitance C (F)";
			// 
			// tbR
			// 
			this->tbR->Location = System::Drawing::Point(160, 135);
			this->tbR->Name = L"tbR";
			this->tbR->Size = System::Drawing::Size(100, 22);
			this->tbR->TabIndex = 3;
			// 
			// tbL
			// 
			this->tbL->Location = System::Drawing::Point(160, 175);
			this->tbL->Name = L"tbL";
			this->tbL->Size = System::Drawing::Size(100, 22);
			this->tbL->TabIndex = 4;
			// 
			// tbC
			// 
			this->tbC->Location = System::Drawing::Point(160, 215);
			this->tbC->Name = L"tbC";
			this->tbC->Size = System::Drawing::Size(100, 22);
			this->tbC->TabIndex = 5;
			// 
			// btnSimulate
			// 
			this->btnSimulate->Location = System::Drawing::Point(160, 255);
			this->btnSimulate->Name = L"btnSimulate";
			this->btnSimulate->Size = System::Drawing::Size(100, 23);
			this->btnSimulate->TabIndex = 6;
			this->btnSimulate->Text = L"Simulate";
			this->btnSimulate->UseVisualStyleBackColor = true;
			this->btnSimulate->Click += gcnew System::EventHandler(this, &Form1::btnSimulate_Click_1);
			// 
			// picGraph
			// 
			this->picGraph->BackColor = System::Drawing::Color::White;
			this->picGraph->Location = System::Drawing::Point(266, 110);
			this->picGraph->Name = L"picGraph";
			this->picGraph->Size = System::Drawing::Size(863, 496);
			this->picGraph->TabIndex = 7;
			this->picGraph->TabStop = false;
			// 
			// simTimer
			// 
			this->simTimer->Interval = 10;
			this->simTimer->Tick += gcnew System::EventHandler(this, &Form1::simTimer_Tick);
			// 
			// Title
			// 
			this->Title->AutoSize = true;
			this->Title->Location = System::Drawing::Point(263, 57);
			this->Title->Name = L"Title";
			this->Title->Size = System::Drawing::Size(200, 50);
			this->Title->TabIndex = 8;
			this->Title->Text = L"RLC Circuit Simulator";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::PaleTurquoise;
			this->ClientSize = System::Drawing::Size(1379, 708);
			this->Controls->Add(this->Title);
			this->Controls->Add(this->picGraph);
			this->Controls->Add(this->btnSimulate);
			this->Controls->Add(this->tbC);
			this->Controls->Add(this->tbL);
			this->Controls->Add(this->tbR);
			this->Controls->Add(this->CapacitanceLabel);
			this->Controls->Add(this->InductanceLabel);
			this->Controls->Add(this->ResistanceLabel);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->TopMost = true;
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picGraph))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void label1_Click(System::Object^ sender, System::EventArgs^ e) {
	}
	private:
		// Draws the RLC discharge graph onto the PictureBox
		void DrawGraph(double R, double L, double C) {

			simTimer->Stop();
			drawIndex = 0;

			int w = picGraph->Width;
			int h = picGraph->Height;

			// Pre-calculate all y points
			double omega = 1.0 / Math::Sqrt(L * C);
			double totalTime = 10.0 * (1.0 / omega) * (1.0 + R / (2.0 * Math::Sqrt(L / C)));
			int steps = w - 50;
			double dt = totalTime / steps;

			yPoints = gcnew array<double>(steps);
			double V = 1.0, I = 0.0;
			for (int i = 0; i < steps; i++) {
				double dI = (V - I * R) / L * dt;
				double dV = (-I / C) * dt;
				I += dI;
				V += dV;
				double y = ((h * 4) / 5) - (V * ((h * 4) / 5 - 20));
				y = Math::Max(10.0, Math::Min((double)(h - 10), y));
				yPoints[i] = y;
			}

			// Set up the bitmap and draw axes once
			bmp = gcnew Bitmap(w, h);
			g = Graphics::FromImage(bmp);
			g->Clear(Color::White);

			Pen^ axisPen = gcnew Pen(Color::Black, 2);
			g->DrawLine(axisPen, 40, 10, 40, h - 30);
			g->DrawLine(axisPen, 40, (h * 4) / 5, w - 10, (h * 4) / 5);

			System::Drawing::Font^ font = gcnew System::Drawing::Font("Arial", 8);
			g->DrawString("V(t)", font, Brushes::Black, 2, 10);
			g->DrawString("t", font, Brushes::Black, w - 20, (h * 4) / 5 - 15);

			String^ timeLabel = "Total time: " + totalTime.ToString("F4") + "s";
			g->DrawString(timeLabel, font, Brushes::Black, 40, h - 25);
			graphPen = gcnew Pen(Color::Pink, 2);
			picGraph->Image = bmp;

			simTimer->Start();
		}

	private: System::Void simTimer_Tick(System::Object^ sender, System::EventArgs^ e) {
		if (drawIndex >= yPoints->Length - 1) {
			simTimer->Stop();
			return;
		}

		// Draw a few points per tick so it's smooth but not too slow
		int pointsPerTick = 2;
		for (int i = 0; i < pointsPerTick && drawIndex < yPoints->Length - 1; i++) {
			float x1 = 40 + drawIndex;
			float x2 = 40 + drawIndex + 1;
			g->DrawLine(graphPen, x1, (float)yPoints[drawIndex], x2, (float)yPoints[drawIndex + 1]);
			drawIndex++;
		}

		picGraph->Image = bmp;
		picGraph->Refresh();
	};

	private: System::Void btnSimulate_Click_1(System::Object^ sender, System::EventArgs^ e) {
		double R, L, C;

		// Parse inputs safely
		if (!Double::TryParse(tbR->Text, R) ||
			!Double::TryParse(tbL->Text, L) ||
			!Double::TryParse(tbC->Text, C)) {
			MessageBox::Show("Please enter valid numbers for R, L, and C.");
			return;
		}

		if (R <= 0 || L <= 0 || C <= 0) {
			MessageBox::Show("R, L, and C must all be positive.");
			return;
		}

		DrawGraph(R, L, C);
	}

	};
};
