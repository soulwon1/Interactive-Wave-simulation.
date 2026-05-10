#pragma once
#include "FieldLineTracer.h"
#include "MagneticSimWrapper.h"
#include "GameMode.h"
#include "TowerDefenseMode.h"
#include "UIButtons.h"

namespace CppCLRWinFormsProject {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing::Drawing2D;
    using namespace System::Drawing;
    using namespace System::Collections::Generic;

    public ref class Form1 : public System::Windows::Forms::Form
    {
    public:
        Form1(void)
        {
            InitializeComponent();

            sim = gcnew MagneticSimWrapper();
            tdMode = gcnew TowerDefenseMode(sim);

            this->KeyPreview = true;
            this->KeyDown += gcnew KeyEventHandler(this, &Form1::OnKeyDown);

            backbuffer = gcnew Bitmap(this->ClientSize.Width, this->ClientSize.Height);

            sim->AddMagnet(200, 200, 0.0, 150.0, 80.0, 20.0);
            sim->AddMagnet(400, 250, 0.0, 150.0, 80.0, 20.0);

            // Center camera on the world core (prevents any additonal visual issues). 

            timer = gcnew Timer();
            timer->Interval = 16;
            timer->Tick += gcnew EventHandler(this, &Form1::OnTick);
            timer->Start();

            //Ui Buttons:
            uiButtons = gcnew List<UIButton^>();

            uiButtons->Add(gcnew UIButton(Rectangle(10, 10, 120, 40), "Start",
                Color::FromArgb(0xF7, 0xC5, 0xCC))); //Colour Pastel Pink; 
            uiButtons->Add(gcnew UIButton(Rectangle(140, 10, 120, 40), "Reset",
                Color::FromArgb(0xCD, 0xE7, 0xF0))); //Patel Blue 
            uiButtons->Add(gcnew UIButton(Rectangle(270, 10, 160, 40), "Tower Defense",
                Color::FromArgb(0xCF, 0xF5, 0xD2))); //pastel mint 
            uiButtons->Add(gcnew UIButton(Rectangle(440, 10, 160, 40), "Simulation",
                Color::FromArgb(0xE3, 0xD4, 0xF3))); // pastel lavender 
        }

    protected:
        ~Form1()
        {
            if (components)
                delete components;
        }

        // *** PATCH 1: REMOVED OnPaint OVERRIDE ***
        // UI is now drawn inside OnTick into the backbuffer.

    private:
        System::ComponentModel::Container^ components;

        MagneticSimWrapper^ sim;
        TowerDefenseMode^ tdMode;

        GameMode mode = GameMode::Sandbox; //sets the gamemode to Sandbox. 

        PictureBox^ pictureBox1;
        Timer^ timer;

        Bitmap^ backbuffer; //This may become obsolute. 
        int frameCounter = 0;
        List<List<PointF>^>^ cachedLines; //all field lines. 

        //some needed bool and ints. 
        bool dragging = false;
        bool rotating = false;
        int dragMagnetIndex = -1;
        PointF dragOffset;
        PointF lastMousePos;
        DateTime lastMouseTime;

        float camX = 0, camY = 0; //camera position. 

        float zoom = 1.0f; //zoom value. 
        float shakeX = 0, shakeY = 0; //shake value. 
        Random^ rng = gcnew Random();

        //Ui buttons
        List<UIButton^>^ uiButtons;

#pragma region Windows Form Designer generated code
        void InitializeComponent(void)
        {
            this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
            this->SuspendLayout();

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

            this->ClientSize = System::Drawing::Size(800, 600);
            this->Controls->Add(this->pictureBox1);
            this->Name = L"Form1";
            this->Text = L"Magnet Sandbox";

            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
            this->ResumeLayout(false);
        }
#pragma endregion

    private:

        // MAIN LOOP
        void OnTick(Object^ sender, EventArgs^ e)
        {
            sim->Step(0.016); //0.016 as the tick should give 60fps. 

            // Tower Defense update
            if (mode == GameMode::TowerDefense)
                tdMode->Update(0.016);

            frameCounter++;
            if (cachedLines == nullptr || frameCounter % 5 == 0 )
                cachedLines = sim->GetFieldLines();

            if (backbuffer == nullptr ||
                backbuffer->Width != this->ClientSize.Width ||
                backbuffer->Height != this->ClientSize.Height)
            {
                delete backbuffer;
                backbuffer = gcnew Bitmap(this->ClientSize.Width, this->ClientSize.Height);
            }

            Graphics^ g = Graphics::FromImage(backbuffer);

            g->SmoothingMode = SmoothingMode::AntiAlias;
            g->PixelOffsetMode = PixelOffsetMode::HighQuality;
            g->CompositingQuality = CompositingQuality::HighQuality;
            g->InterpolationMode = InterpolationMode::HighQualityBicubic;

            g->ResetTransform();
            g->Clear(Color::Black);

            ApplyCamera(g);
            DrawBackground(g);
            DrawHeatmap(g, 6);

            if (cachedLines != nullptr)
            {
                for each (List<PointF> ^ line in cachedLines)
                {
                    if (line != nullptr && line->Count > 1)
                        DrawFieldLines(g, line);
                }
            }

            DrawMagnets(g);

            // Tower Defense drawing
            if (mode == GameMode::TowerDefense)
                tdMode->Draw(g);

            // HUD (screen space)
            g->ResetTransform();
            if (mode == GameMode::TowerDefense)
            {
                g->DrawString("Lives: " + tdMode->GetLives(),
                    gcnew System::Drawing::Font("Segoe UI", 12), Brushes::White, PointF(10, 10));

                g->DrawString("Wave: " + tdMode->GetWave(),
                    gcnew System::Drawing::Font("Segoe UI", 12), Brushes::White, PointF(10, 30));

                g->DrawString("Money: " + tdMode->GetMoney(),
                    gcnew System::Drawing::Font("Segoe UI", 12), Brushes::White, PointF(10, 50));
            }

            // *** PATCH 2: DRAW UI BUTTONS INTO BACKBUFFER ***
            g->ResetTransform();
            for each (UIButton ^ b in uiButtons)
                b->Draw(g);

            delete g;
            pictureBox1->Image = backbuffer;
        }

        void ResetSimulation()
        {
            if (sim != nullptr)
                sim->Reset();

            cachedLines = nullptr; 

            camX = 0;
            camY = 0;

            if (mode == GameMode::TowerDefense && tdMode != nullptr)
                tdMode->Reset();
        }

        // background rendering. 
        void DrawBackground(Graphics^ g)
        {
            Rectangle rect = this->ClientRectangle;

            array<Point>^ corners =
            {
                Point(rect.Left, rect.Top),
                Point(rect.Right, rect.Top),
                Point(rect.Left, rect.Bottom),
                Point(rect.Right, rect.Bottom)
            };

            PathGradientBrush^ bg = gcnew PathGradientBrush(corners);

            bg->CenterColor = Color::FromArgb(255, 20, 20, 40);
            bg->SurroundColors = gcnew array<Color>{Color::FromArgb(255, 5, 5, 15)};

            g->FillRectangle(bg, rect);
        }

        // function to go from screen to the world coords. 
        PointF ScreenToWorld(int sx, int sy)
        {
            float wx = (sx - camX - shakeX) / zoom;
            float wy = (sy - camY - shakeY) / zoom;
            return PointF(wx, wy);
        }

        // heatmap , this may become obsolete. (was working in previous versions).
        Color HeatColor(double B)
        {
            B = Math::Min(B, 1.0);

            int r = (int)(255 * B);
            int g = (int)(255 * (1.0 - fabs(B - 0.25) * 2));
            int b = (int)(255 * (1.0 - B));

            r = Math::Max(0, Math::Min(255, r));
            g = Math::Max(0, Math::Min(255, g));
            b = Math::Max(0, Math::Min(255, b));

            return Color::FromArgb(40, r, g, b);
        }

        void DrawHeatmap(Graphics^ g, int cellSize) //Draws the heatmap. 
        {
            for (int y = 0; y < Height; y += cellSize)
            {
                for (int x = 0; x < Width; x += cellSize)
                {
                    double Bx, By;
                    PointF world = ScreenToWorld(x, y);
                    sim->GetField(world.X, world.Y, Bx, By);

                    double B = sqrt(Bx * Bx + By * By);
                    B = Math::Min(B / 2000.0, 1.0);

                    SolidBrush^ b = gcnew SolidBrush(HeatColor(B));
                    g->FillRectangle(b, x, y, cellSize, cellSize);
                }
            }
        }

        void DrawArrow(Graphics^ g, PointF from, PointF to) //draw arrow function to show the direction of the field lines
        {
            float dx = to.X - from.X;
            float dy = to.Y - from.Y;

            float len = Math::Sqrt(dx * dx + dy * dy);
            if (len < 0.001f) return;

            dx /= len;
            dy /= len;

            // Arrow size
            float size = 8.0f;

            // Perpendicular
            float px = -dy;
            float py = dx;

            PointF p1(to.X, to.Y);
            PointF p2(to.X - dx * size + px * size * 0.5f,
                to.Y - dy * size + py * size * 0.5f);
            PointF p3(to.X - dx * size - px * size * 0.5f,
                to.Y - dy * size - py * size * 0.5f);

            array<PointF>^ tri = gcnew array<PointF>{ p1, p2, p3 };

            SolidBrush^ b = gcnew SolidBrush(Color::FromArgb(255, 0, 220, 255));
            g->FillPolygon(b, tri);
        }

        //draws field lines of the magnets. 
        void DrawFieldLines(Graphics^ g, List<PointF>^ pts)
        {
            if (pts->Count < 2) return;

            Pen^ glow = gcnew Pen(Color::FromArgb(40, 0, 180, 255), 6.0f);
            glow->LineJoin = LineJoin::Round;

            array<PointF>^ arr = pts->ToArray();

            g->DrawLines(glow, arr);
            
            //draw arrows every N segments
            int step = 12; //Controls the spacing of the arrows 
            for (int i = step; i < pts->Count; i += step)
            {
                DrawArrow(g, pts[i - 1], pts[i]); 
            }
        }

        // Draw magnet function for our magnet.
        void DrawMagnet(Graphics^ g, float cx, float cy, float width, float height, float angle)
        {
            g->TranslateTransform(cx, cy); //translates the position to the simulation position. 
            g->RotateTransform(angle * 180.0f / 3.1415926f); //same for the rotation
            g->TranslateTransform(-width / 2, -height / 2);

            float x = 0.0f;
            float y = 0.0f;

            SolidBrush^ shadow = gcnew SolidBrush(Color::FromArgb(60, 0, 0, 0));
            g->FillEllipse(shadow, RectangleF(x - 5.0f, y + height + 3.0f, width + 10.0f, 12.0f));
            delete shadow;

            GraphicsPath^ path = gcnew GraphicsPath();
            float r = 12.0f;

            path->AddArc(x, y, r, r, 180, 90);
            path->AddArc(x + width - r, y, r, r, 270, 90);
            path->AddArc(x + width - r, y + height - r, r, r, 0, 90);
            path->AddArc(x, y + height - r, r, r, 90, 90);
            path->CloseFigure();

            LinearGradientBrush^ body =
                gcnew LinearGradientBrush(RectangleF(x, y, width, height),
                    Color::FromArgb(255, 255, 80, 80),
                    Color::FromArgb(250, 150, 0, 0),
                    90.0f);
            g->FillPath(body, path);
            delete body;

            LinearGradientBrush^ gloss =
                gcnew LinearGradientBrush(RectangleF(x, y, width, height),
                    Color::FromArgb(120, 255, 255, 255),
                    Color::FromArgb(0, 255, 255, 255),
                    90.0f);
            g->FillPath(gloss, path);
            delete gloss;

            StringFormat^ sf = gcnew StringFormat();
            sf->Alignment = StringAlignment::Center;
            sf->LineAlignment = StringAlignment::Center;

            //Writes the North and South of the Magnet. 
            g->DrawString("N",
                gcnew System::Drawing::Font("Segoe UI", 10, FontStyle::Bold),
                Brushes::White,
                PointF(x + width * 0.25f, y + height / 2), sf);

            g->DrawString("S",
                gcnew System::Drawing::Font("Segoe UI", 10, FontStyle::Bold),
                Brushes::White,
                PointF(x + width * 0.75f, y + height / 2), sf);

            delete sf; //Clears sf from the memory cache
            delete path; //clears path from the memory cache. 

            g->ResetTransform();
        }

        void DrawMagnets(Graphics^ g)
        {
            int count = sim->GetMagnetCount();

            for (int i = 0; i < count; ++i)
            {
                double x, y;
                sim->GetMagnetPosition(i, x, y);
                double angle = sim->GetMagnetAngle(i);

                float width = 80.0f;
                float height = 20.0f;

                g->FillEllipse(Brushes::Lime, (float)x - 3.0f, (float)y - 3.0f, 6.0f, 6.0f);

                DrawMagnet(g, (float)x, (float)y, width, height, (float)angle);
            }
        }

        // HIT TEST
        int HitTestMagnet(int mx, int my)
        {
            for (int i = 0; i < sim->GetMagnetCount(); i++)
            {
                double x, y;
                sim->GetMagnetPosition(i, x, y);

                float dx = mx - (float)x;
                float dy = my - (float)y;

                if (Math::Abs(dx) < 40 && Math::Abs(dy) < 20)
                    return i;
            }
            return -1;
        }
        //Function for is the Button is Clicked. 
        void HandleButtonClick(String^ text)
        {
            if (text == "Start") //what happens when start button is pressed. 
            {
                if (mode == GameMode::TowerDefense) 
                {
                    tdMode->Start();
                }
                
            }
            else if (text == "Reset") //what happens when the reset button is pressed. 
            {
                ResetSimulation(); 
            }
            else if (text == "Tower Defense") //what happens when the Tower Defense mode is selected. 
            {
                mode == GameMode::TowerDefense; 
            }
            else if (text == "Simulation") // what happens when the sandbox button is clicked
            {
                mode = GameMode::Sandbox; 
            }
        }
        // CAMERA
        void ApplyCamera(Graphics^ g)
        {
            g->TranslateTransform(camX + shakeX, camY + shakeY);
            g->ScaleTransform(zoom, zoom);
        }

        void CameraShake(float intensity)
        {
            shakeX = (float)(rng->NextDouble() * intensity - intensity / 2);
            shakeY = (float)(rng->NextDouble() * intensity - intensity / 2);
        }

        // INPUT
        void OnKeyDown(Object^ sender, KeyEventArgs^ e)
        {
            if (e->KeyCode == Keys::F1)
                mode = GameMode::Sandbox;

            if (e->KeyCode == Keys::F2)
            {
                mode = GameMode::TowerDefense;
                tdMode->Start();
            }

            if (e->KeyCode == Keys::Delete)
            {
                if (dragMagnetIndex >= 0)
                {
                    sim->RemoveMagnet(dragMagnetIndex);
                    dragMagnetIndex = -1;
                    return;
                }
            }
        }

        void OnMouseDown(Object^ sender, MouseEventArgs^ e)
        {
            int idx = HitTestMagnet(e->X, e->Y);
            //Updates the hover state when the mouse is clicked. 
            for each (UIButton ^ b in uiButtons)
            {
                if (b->hovered)
                {
                    b->pressed = true; 
                }
            }

            if (e->Button == System::Windows::Forms::MouseButtons::Left)
            {
                if (idx >= 0)
                {
                    dragging = true;
                    dragMagnetIndex = idx;

                    double x, y;
                    sim->GetMagnetPosition(idx, x, y);
                    dragOffset = PointF(e->X - (float)x, e->Y - (float)y);
                }
                else if (mode == GameMode::TowerDefense)
                {
                    if (tdMode->TrySpend(50))   // cost to place a magnet
                    {
                        sim->AddMagnetAt(e->X, e->Y);
                    }
                    else
                    {
                        // optional: flash red or shake camera
                        CameraShake(10.0f);
                    }
                }
                else
                {
                    // sandbox mode: free placement
                    sim->AddMagnetAt(e->X, e->Y);

                    //This line is for debugging reasons. 
                    int count = sim->GetMagnetCount();
                    Console::WriteLine("Magnets in sim: " + count);

                    //Force the field lines to regen
                    cachedLines = nullptr; 

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
            //Upadtes Hover positon of the mouse. 
            for each (UIButton ^ b in uiButtons)
            {
                b->hovered = b->Contains(e->Location);
            }
            Invalidate();  //redraw UI

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
            for each (UIButton ^ b in uiButtons)
            {
                if (b->pressed && b->hovered)
                {
                    HandleButtonClick(b->text); 
                }
                b->pressed = false; 
            }
            if (dragging && dragMagnetIndex >= 0)
            {
                TimeSpan dt = DateTime::Now - lastMouseTime;
                double dtSec = Math::Max(dt.TotalSeconds, 1e-3);

                double vx = (e->X - lastMousePos.X) / (dtSec * 0.1);
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
                sim->FlipMagnet(idx);
        }
    };
}
