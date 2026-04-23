#pragma once
#include "FieldLineTracer.h"
#include "C:\Users\Ezrah\Documents\Coding wokr\Magnetics-Uzair\MagneticSimWrapper.h" 
// I want to note I was getting an error when using #include "MagneticSimWrapper.h" without the full path, even though the file is in the same directory as Form1.h. I think this might be an issue with Visual Studio's include directories or something, but for now I just put the full path to get it working.

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
			sim = gcnew MagneticSimWrapper(); 

			// add you magnets here. (For now Im adding 2) 
			sim->AddMagnet(200, 200, 0.0, 150.0, 80.0, 20.0); //x, y, angle, strength, width, height
			sim->AddMagnet(400, 250, 0.0, 150.0, 80.0, 20.0); //x, y, angle, strength, width, height

			heatmap = gcnew Bitmap(pictureBox1->Width, pictureBox1->Height);//create a bitmap for the heatmap

			timer = gcnew Timer();
			timer->Interval = 16; //about 60 fps
			timer->Tick += gcnew EventHandler(this, &Form1::OnTick); 
			timer->Start(); //start the simulation loop
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

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		int frameCounter = 0; 
		System::Collections::Generic::List <
			System::Collections::Generic::List<System::Drawing::PointF>^>^ cachedLines;  //cache field lines to avoid regenerating them every frame (since they don't change unless magnets move)
		System::ComponentModel::Container ^components;
		MagneticSimWrapper^ sim; 
		Bitmap^ heatmap; 
		Timer^ timer;
		//Im adding some new variables for interactivity, These are not needed for the barebones sim. 

		bool dragging = false; //whether we are currently dragging a magnet
		int dragMagnetIndex = -1; //index of the magnet being dragged, -1 if none
		PointF dragOffset; //offset from the mouse position to the magnet center when dragging
		PointF lastMousePos; //last mouse position, for calculating drag offset
		DateTime lastMouseTime;
		bool rotating = false; 


	private: System::Windows::Forms::PictureBox^ pictureBox1;
		  

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox1
			// 
			this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox1->Location = System::Drawing::Point(0, 0);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(800, 600);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->MouseDown += gcnew MouseEventHandler(this, &Form1::OnMouseDown);
			this->pictureBox1->MouseMove += gcnew MouseEventHandler(this, &Form1::OnMouseMove);
			this->pictureBox1->MouseUp += gcnew MouseEventHandler(this, &Form1::OnMouseUp);
			this->pictureBox1->MouseDoubleClick += gcnew MouseEventHandler(this, &Form1::OnMouseDoubleClick); 
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(800, 600);
			this->Controls->Add(this->pictureBox1);
			this->Name = L"Form1";
			this->Text = L"Magnet Sandbox";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: 
		//Main Simulation LOOP

		void OnTick(Object^ sender, EventArgs^ e)
		{
			sim->Step(0.016); //step the simulation forward by 16 ms (about 60 fps)

			frameCounter++; 
			if (frameCounter % 5 == 0 || cachedLines == nullptr) //recompute every 5 frames
			{
				cachedLines = sim->GetFieldLines(); 
			}

			DrawHeatmap();
			DrawFieldLines();
			DrawMagnets();

			pictureBox1->Image = heatmap; //update the picture box with the new heatmap (which also has field lines and magnets drawn on it)
		}

		//Rendering Heatmap. 
		void DrawHeatmap() //I rewrote the heatmap drawing code to be more efficient by sampling every few pixels instead of every single pixel, and by using a single Graphics object for the whole heatmap instead of creating one for each pixel. This should significantly improve performance while still giving a good visual representation of the field.
		{
			Graphics^ g = Graphics::FromImage(heatmap);
			g->Clear(Color::Black); 

			const int step = 4; // sample every 4 pixels. 

			for (int y = 0; y < heatmap->Height; y += step)
			{
				for (int x = 0; x < heatmap->Width; x += step)
				{
					double Bx, By; 
					sim->GetField(x, y, Bx, By); 

					double mag = std::sqrt(Bx * Bx + By * By); //calculate field magnitude
					mag = Math::Min(mag * 40.0, 1.0); //scaled a bit lower

					int r = (int)(mag * 255); //red channel based on field strength
					int gCol = 0; 
					int b = (int)((1.0 - mag) * 255); //blue channel inverse of field strength

					Brush^ brush = gcnew SolidBrush(Color::FromArgb(r, gCol, b));
					//create a brush with the calculated color
					g->FillRectangle(brush, x, y, step, step); //fill a rectangle at the sampled position with the brush color
					delete brush; //clean up the brush
				}
			}
			delete g; //clean up graphics object
		}

		//Draw field Lines. 
		void DrawFieldLines() //I have also optimized the field line drawing code by caching the field lines and only redrawing them every few frames (since they don't change unless the magnets move). This way we avoid regenerating and redrawing the field lines every single frame, which can be expensive. The DrawFieldLines function now simply draws the cached lines on top of the heatmap, which should be much faster.
		{
			if (cachedLines == nullptr) return; 

			Graphics^ g = Graphics::FromImage(heatmap); 
			Pen^ pen = gcnew Pen(Color::White, 1.0f); //white pen for field lines

			for each (auto line in cachedLines)
			{
				if (line->Count > 1)
					g->DrawLines(pen, line->ToArray());
			}

			delete pen; //clean up pen
			delete g; //clean up graphics object
		}

		//Draw Magnets 
		void DrawMagnets()
		{
			Graphics^ g = Graphics::FromImage(heatmap);

			for (int i = 0; i < sim->GetMagnetCount(); i++)
			{
				double x, y; 
				sim->GetMagnetPosition(i, x, y); 
				double angle = sim->GetMagnetAngle(i); 

				//Magnet size (you can adjust this or get it from the simulator if you want variable sizes)
				float w = 80; 
				float h = 20; 

				g->TranslateTransform((float)x, (float)y); //Move to the magnet position
				g->RotateTransform((float)(angle * 180.0 / Math::PI)); //Rotate to the magnet angle (convert from radians to degrees)

				g->ResetTransform(); //Reset transform for the next magnet
			}

			delete g; //clean up graphics object
		}

		//Hit testing for the magnets;
		int HitTestMagnet(int mx, int my)
		{
			for (int i = 0; i < sim->GetMagnetCount(); i++)
			{
				double x, y; 
				sim->GetMagnetPosition(i, x, y); 

				float dx = mx - (float)x;
				float dy = my - (float)y;

				if (Math::Abs(dx) < 40 && Math::Abs(dy) < 20)
					return i; //return the index of the magnet that was hit
			}
			return -1; //no magnet hit
		}

		//Mouse Dragging Logic...  //These have been edited to include the HitTestMagnet.
		void OnMouseDown(Object^ sender, MouseEventArgs^ e)
		{
			int idx = HitTestMagnet(e->X, e->Y);

			if (e->Button == System::Windows::Forms::MouseButtons::Left)
			{
				if(idx >= 0) //if we hit a magnet, start dragging it
				{
					dragging = true;
					dragMagnetIndex = idx;

					double x, y;
					sim->GetMagnetPosition(idx, x, y);
					dragOffset = PointF(e->X - (float)x, e->Y - (float)y); 
				}
				else
				{
					//click to spawn new magnet 
					sim->AddMagnetAt(e->X, e->Y); 
				}
			}
			else if (e->Button == System::Windows::Forms::MouseButtons::Right && idx >= 0)
			{
				rotating = true; 
				dragMagnetIndex = idx; 
			}

			lastMousePos = PointF((float)e->X, (float)e->Y);
			lastMouseTime = DateTime::Now; 
		}


		void OnMouseMove(Object^ sender, MouseEventArgs^ e)
		{
			if (dragging && dragMagnetIndex >= 0)
			{
				double x = e->X - dragOffset.X; 
				double y = e->Y - dragOffset.Y;
				sim->SetMagnetPosition(dragMagnetIndex, x, y);
			}
			else if (rotating && dragMagnetIndex >= 0)
			{
				double cx, cy; 
				sim->GetMagnetPosition(dragMagnetIndex, cx, cy); 

				double dx = e->X - cx;
				double dy = e->Y - cy;
				double angle = Math::Atan2(dy, dx);

				sim->SetMagnetAngle(dragMagnetIndex, angle);
			}

			lastMousePos = PointF((float)e->X, (float)e->Y);
		}

		void OnMouseUp(Object^ sender, MouseEventArgs^ e)
		{
			if (dragging && dragMagnetIndex >= 0)
			{
				TimeSpan dt = DateTime::Now - lastMouseTime; 
				double dtSec = Math::Max(dt.TotalSeconds, 1e-3); //avoid division by zero

				double vx = (e->X - lastMousePos.X) / (dtSec * 0.1); //scale factor.
				double vy = (e->Y - lastMousePos.Y) / (dtSec * 0.1);

				sim->SetMagnetVelocity(dragMagnetIndex, vx, vy); 
			}

			dragging = false; 
			rotating = false;
			dragMagnetIndex = -1; 
		}

		void OnMouseDoubleClick(Object^ sender, MouseEventArgs^ e)
		{
			int idx = HitTestMagnet(e->X, e->Y);
			if (idx >= 0)
				sim->FlipMagnet(idx); //flip the magnet if double-clicked
		}
	};
}
