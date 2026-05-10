#include "pch.h"
#include "ArenaMode.h"
using namespace System; 
using namespace System::Drawing; 

ArenaMode::ArenaMode(MagneticSimWrapper^ wrapper)
{
	sim = wrapper;

	//Spawn Player and enemy magnets
	playerIndex = sim->AddMagnetAt(250, 300); 
	enemyIndex = sim->AddMagnetAt(550, 300);
}

void ArenaMode::ResetRound()
{
	roundOver = false; 
	hillControlTime = 0.0f; 

	sim->SetMagnetPosition(playerIndex, 250, 300); //resets the players positon and velocity
	sim->SetMagnetVelocity(playerIndex, 0, 0); 

	sim->SetMagnetPosition(enemyIndex, 550, 300); //does the same here
	sim->SetMagnetVelocity(enemyIndex, 0, 0); 
}

bool ArenaMode::IsInsideHill(double x, double y)  //checks if the psoition of a magnet is inside the hill
{
	double dx = x - hillX; 
	double dy = y - hillY; 
	return(dx * dx + dy * dy) <= (hillRadius * hillRadius); //compares the positons to give true or false using pythag. 
}

bool ArenaMode::IsOutOfBounds(double x, double y) 
{
	return (x < 0 || x > arenaWidth || y < 0 || y > arenaHeight); 
}

void ArenaMode::Update(float dt)
{
	if (roundOver) return; 

	double px, py, ex, ey; //players and enemies coordinates

	sim->GetMagnetPosition(playerIndex, px, py); 
	sim->GetMagnetPosition(enemyIndex, ex, ey); 

	//Hill control logix
	if (IsInsideHill(px, py))
	{
		hillControlTime += dt;
	}
	else
	{
		hillControlTime = std::max(0.0f, hillControlTime - dt * 1.5f);
	}
	if (hillControlTime >= hillWinTime)
	{
		roundOver = true; 
		roundMessage = "You WON!!!!"; 
	}

	//Out of bounds detection: 

	if (IsOutOfBounds(px, py))
	{
		roundOver = true; 
		roundMessage = "You lost :("; 
	}
	else if (IsOutOfBounds(ex, ey))
	{
		roundOver = true;
		roundMessage = "You WON!!!!"; 
	}

	UpdateAI(dt); 
}

void ArenaMode::UpdateAI(float dt)
{
	if (roundOver) return; 

	double px, py, ex, ey; 
	sim->GetMagnetPosition(playerIndex, px, py);
	sim->GetMagnetPosition(enemyIndex, ex, ey); 

	double dx = px - ex; 
	double dy = py - ey; 
	double dist = sqrt(dx * dx + dy * dy); 

	if (dist > 1e-3)
	{
		dx /= dist; //This works out the distance between the enemy 
		dy /= dist; 
	}
	
	bool playerInHill = IsInsideHill(px, py); 

	if (playerInHill)
	{
		//charge at the player; 
		sim->SetMagnetVelocity(enemyIndex, dx * 50, dy * 50);
	}
	else
	{
		//patrol the hill 
		double hx = hillX - ex; 
		double hy = hillY - ey; 
		double hdist = sqrt(hx * hx + hy * hy); 

		if (hdist > hillRadius + 50)
		{
			hx /= hdist; 
			hy /= hdist; 
			sim->SetMagnetVelocity(enemyIndex, hx * 30, hy * 30); 
		}
		else
		{
			//circle around the hill
			sim->SetMagnetVelocity(enemyIndex, -hy * 20, hx * 20);
		}
	}
}

void ArenaMode::DrawArena(Graphics^ g) //Draws our Arena for us
{
	Pen^ borderPen = gcnew Pen(Color::White, 3); 
	g->DrawRectangle(borderPen, (int)0, (int)0, (int)arenaWidth - 1, (int)arenaHeight - 1);

	Pen^ hillPen = gcnew Pen(Color::Yellow, 3); 
	g->DrawEllipse(hillPen, (int)hillX - (int)hillRadius, (int)hillY - (int)hillRadius, (int)hillRadius * 2, (int)hillRadius * 2);
	
	delete borderPen; 
	delete hillPen; 
}

void ArenaMode::DrawRoundMessage(Graphics^ g)
{
	if (!roundOver) return;

	Font^ font = gcnew Font("Ariel", 32, FontStyle::Bold);
	SizeF size = g->MeasureString(roundMessage, font); 

	g->DrawString(roundMessage, font, Brushes::White, (arenaWidth - size.Width) / 2, (arenaHeight - size.Height) / 2);

	delete font; //Clears the font to save space. 
}

bool ArenaMode::IsArenaMagnet(int index)
{
	return index == playerIndex || index == enemyIndex; 
}