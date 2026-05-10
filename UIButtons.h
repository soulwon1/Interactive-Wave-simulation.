#pragma once
//Ive added my own buttons because the standard ones look too 1990. I also wanted to test how drawing buttons work

#include <iostream>
using namespace System; 
using namespace System::Drawing;
using namespace System::Drawing::Drawing2D; 
using namespace System::Collections::Generic; 
using namespace System::Windows::Forms; 

public ref class UIButton
{
public: 
	Rectangle bounds; 
	String^ text; 
	bool hovered = false;
	bool pressed = false; 

	//Im using pastel Colours but these can be changed as seen fit
	Color pastelColor; 
	UIButton(Rectangle r, String^ t, Color pastel)
	{
		bounds = r; 
		text = t; 
	}

	void Draw(Graphics^ g)
	{
		//smooth edges out 
		g->SmoothingMode = SmoothingMode::AntiAlias; 

		//Hovering and pressed effects
		Color fillColor = pastelColor; 
		
		if (hovered)
		{
			fillColor = ControlPaint::Light(pastelColor, 0.25f);
		}

		if (pressed)
		{
			fillColor = ControlPaint::Dark(pastelColor, 0.15f); 
		}

		//Soft Shadows for visual pazaz. 
		Rectangle shadowRect = Rectangle(bounds.X + 3, bounds.Y + 3, bounds.Width, bounds.Height); 
		SolidBrush^ shadowBrush = gcnew SolidBrush(Color::FromArgb(40, 0, 0, 0));
		g->FillRectangle(shadowBrush, shadowRect);

		//Rounded Rectangle path
		GraphicsPath^ path = gcnew GraphicsPath();
		int radius = 14; 

		path->AddArc(bounds.X, bounds.Y, radius, radius, 180, 90); 
		path->AddArc(bounds.Right - radius, bounds.Y, radius, radius, 270, 90);
		path->AddArc(bounds.Right - radius, bounds.Bottom - radius, radius, radius, 0, 90);
		path->AddArc(bounds.X, bounds.Bottom - radius, radius, radius, 90, 90);
		path->CloseFigure(); 

		//Fill with pastel Color
		SolidBrush^ fillBrush = gcnew SolidBrush(fillColor); 
		g->FillPath(fillBrush , path);

		//Soft Border
		Pen^ borderPen = gcnew Pen(Color::FromArgb(120, 255, 255, 255), 2); 
		g->DrawPath(borderPen, path);

		//Text centered
		StringFormat^ sf = gcnew StringFormat(); 
		sf->Alignment = StringAlignment::Center; 
		sf->LineAlignment = StringAlignment::Center; 

		g->DrawString(text, gcnew Font("Segoe UI", 11, FontStyle::Bold), Brushes::Black, bounds, sf);
	}

	bool Contains(Point p)
	{
		return bounds.Contains(p); 
	}
};