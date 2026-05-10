#pragma once

using namespace System;
using namespace System::Drawing;
using namespace System::Collections::Generic;

ref class MagneticSimWrapper;

public ref class TowerDefenseMode //this had to be a public ref class to use access the MagneticSimWrapper^. 
{
public:
    TowerDefenseMode(MagneticSimWrapper^ sim); //uses logic from the simulation.  

    //Void functions that can be called. 
    void Start(); 
    void Reset();
    void Update(double dt);
    void Draw(Graphics^ g);

    //Getter functions. 
    int GetLives() { return lives; }
    int GetWave() { return wave; }
    int GetMoney() { return money; }
    //This gives us a cost for the magnetCost. This can be tweaked as needed. 

    literal int magnetCost = 50; 

    //the use of a literal, speeds up compile time because it does not occupy memory space in every instance. 
    //Furthermore it prevents accidental changes of the int during run time. 

    bool TrySpend(int amount) //Checks if money can be spent 
    {
        if (money < amount)
            return false;

        money -= amount; 
        return true;
    }
private:
    //since the class is public ref all structs have to be ref. 
    ref struct Enemy //simple struct for the enemy that has all the needed variables for the enemy. 
    {
        double x, y;
        double vx, vy;
        double health;
        double susceptibility;
        double speed;
        bool alive;
    };

    MagneticSimWrapper^ sim; //allows the TowerDefenseMode to run the sim 

    List<Enemy^>^ enemies; 

    // World
    literal double worldW = 800.0;   // match your window width
    literal double worldH = 600.0;   // match your window height

    //Core
    literal double coreX = worldW / 2.0; //Creates a core 
    literal double coreY = worldH / 2.0;
    literal double coreRadius = 40.0;

    // Game state
    int lives;
    int wave;
    int money;

    // Wave spawning
    int enemiesToSpawn;
    double spawnTimer;
    double spawnInterval;

    Random^ rng;

    // Helper functions that are being created here so they can be called later. 
    void SpawnEnemy();
    void UpdateEnemies(double dt);
    void DrawEnemies(Graphics^ g);
    void DrawCore(Graphics^ g);
    bool EnemyReachedCore(Enemy^ e);

   

};
