#include "pch.h"
#include "TowerDefenseMode.h"
#include "MagneticSimWrapper.h"

TowerDefenseMode::TowerDefenseMode(MagneticSimWrapper^ sim)
{
    this->sim = sim;
    this->rng = gcnew Random();
    this->enemies = gcnew List<Enemy^>(); //allocates RAM to the Enemy struct. 
    Reset();
}

void TowerDefenseMode::Reset() //A reset function that allows the game to refresh. 
{
    enemies->Clear();
    lives = 20;
    wave = 0;
    money = 100;

    enemiesToSpawn = 0;
    spawnTimer = 0.0;
    spawnInterval = 0.5;
}

void TowerDefenseMode::Start() //Simple start function. 
{
    Reset(); //resets everything incase the game was already running. 
    wave = 1;
    enemiesToSpawn = 10;
}

void TowerDefenseMode::Update(double dt) //basic update logic 
{
    if (lives <= 0)
        return;

    // Spawn enemies
    if (enemiesToSpawn > 0)
    {
        spawnTimer -= dt;
        if (spawnTimer <= 0.0)
        {
            SpawnEnemy(); //creates a new enemy 
            enemiesToSpawn--;
            spawnTimer = spawnInterval;
        }
    }

    UpdateEnemies(dt);

    // Check if wave is done
    bool anyAlive = false;
    for each (Enemy ^ e in enemies)
        if (e->alive) anyAlive = true;

    if (!anyAlive && enemiesToSpawn == 0)
    {
        wave++;
        enemiesToSpawn = 10 + wave * 2; //A simple enemy increase line so the game gets progressively hard. 
    }
}

void TowerDefenseMode::SpawnEnemy() //The function to spawn enemies. 
{
    Enemy^ e = gcnew Enemy(); 
    e->vx = e->vy = 0.0; //sets the new enemies velocities to 0, 
    e->health = 100.0 + wave * 10;

    //e->susceptibility = 0.8;
   // e->speed = 1.0 + wave * 0.05;

    e->susceptibility = 10.0; //sets the susceptibility, to a value (this is essentially how much affect the magnetic field has on it). 
    e->speed = 80.0 + wave * 5.0;  //Increases the speed per wave. 

    e->alive = true; //sets the enemy to alive. 

    double r = rng->NextDouble(); //NextDouble gives a random value between 0.0 - 1.0, where rng acts as our seed. 

    if (r < 0.25) //this set of if statements allow us to evenly distribute the spawns across the screen. 
    {
        e->x = rng->NextDouble() * worldW;
        e->y = 0.0;
    }
    else if (r < 0.5)
    {
        e->x = rng->NextDouble() * worldW;
        e->y = worldH;
    }
    else if (r < 0.75)
    {
        e->x = 0.0;
        e->y = rng->NextDouble() * worldH;
    }
    else
    {
        e->x = worldW;
        e->y = rng->NextDouble() * worldH;
    }

    enemies->Add(e); //adds to the vector. 
}

void TowerDefenseMode::UpdateEnemies(double dt) //Update function for the enemies. 
{
    for each (Enemy ^ e in enemies) //runs this for every enemy. 
    {
        if (!e->alive) continue; //checks if an enemy is dead if it is, it skips to the next one without breaking the loop (continue). 

        // Magnetic field
        double Bx, By;
        sim->GetField(e->x, e->y, Bx, By);

        //Magnet strength, that can be tweaked to feel right. 
        double magneticStrength = 1000.0;   

        Bx *= magneticStrength;
        By *= magneticStrength;


        // Add a direct pull toward the core so enemies always move
        double dx = coreX - e->x;
        double dy = coreY - e->y;
        double len = Math::Sqrt(dx * dx + dy * dy) + 1e-6;

        Bx += (dx / len) * 2.0;   // 50 = strength of core attraction
        By += (dy / len) * 2.0;

        e->vx += Bx * e->susceptibility * dt;
        e->vy += By * e->susceptibility * dt;

        // Random drift
        e->vx += (rng->NextDouble() - 0.5) * 0.2; //picks random direction. 
        e->vy += (rng->NextDouble() - 0.5) * 0.2;

        // Move
        e->x += e->vx * dt * e->speed; //basic movement calculations. 
        e->y += e->vy * dt * e->speed;

        //checks if its in the Core. 
        if (EnemyReachedCore(e))
        {
            e->alive = false;
            lives--;
            continue;
        }

        // Checks if its Out of bounds
        if (e->x < -200 || e->x > worldW + 200 ||
            e->y < -200 || e->y > worldH + 200)
        {
            e->alive = false;
        }
    }
}

bool TowerDefenseMode::EnemyReachedCore(Enemy^ e) //Logic for the cores, radius. 
{
    double dx = e->x - coreX;
    double dy = e->y - coreY;
    return (dx * dx + dy * dy < coreRadius * coreRadius);
}


void TowerDefenseMode::Draw(Graphics^ g) //callabale function that draws both enemies and core 
{
    DrawCore(g);
    DrawEnemies(g);
}

void TowerDefenseMode::DrawCore(Graphics^ g) //logic for drawing the core. 
{
    SolidBrush^ coreBrush = gcnew SolidBrush(Color::FromArgb(200, 0, 200, 255));
    g->FillEllipse(coreBrush,
        (float)(coreX - coreRadius),
        (float)(coreY - coreRadius),
        (float)(coreRadius * 2),
        (float)(coreRadius * 2));
}

void TowerDefenseMode::DrawEnemies(Graphics^ g) //logic for drawing the enemies. 
{
    for each (Enemy ^ e in enemies)
    {
        if (!e->alive) continue;

        SolidBrush^ b = gcnew SolidBrush(Color::FromArgb(255, 255, 80, 80));
        g->FillEllipse(b, (float)e->x - 6.0f, (float)e->y - 6.0f, 12.0f, 12.0f);
    }
}


