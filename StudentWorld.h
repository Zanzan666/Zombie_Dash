#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <list>
#include <string>

using namespace std;
using Direction = int;

class Actor;
class Penelope;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    ~StudentWorld();
    void pickUpGas();
    void pickUpVaccine();
    void pickUpLandmine();
    void useGas(){m_amtGas--;}
    void useVaccine(){m_amtVaccine--;}
    void useLandmine(){m_amtLandmine--;}
    int getAmtGas() const;
    int getAmtVaccine() const;
    int getAmtLandmine() const;
    bool isMovableAt(int toX, int toY, Actor* current);
    void DisplayText();
    void theExitOverlapWithCitizens(int x, int y);
    void OverlapWithFlame(int x, int y);
    bool isFlamableAt(int x, int y);
    void determineIfLeavable(int x,int y);
    bool OverlapWithPenelope(int x,int y);
    bool addFlame(int x,int y,Direction dir);
    bool GoingToVomitAt(int x, int y);
    void addVomit(int x,int y,Direction dir);
    Direction findClosestHuman(int x, int y);
    void addZombie(int x,int y);
    double findClosestZombie(int x, int y);
    void citizenMove(int x, int y, Actor*thisCitizen);
    void addLandmine(int x, int y);
    void addVaccineGoodieWhenZombieIsAt(int x, int y);
    void OverlapWithVomit(int x, int y);
    bool ifLevelDone() {return m_levelDone;}
    bool OverlapWithLandmine(int x, int y);
    void addPit(int x, int y);

private:
    list<Actor*> m_actors;
    Penelope* m_player;
    bool m_initialized;
    int m_numOfCitizensLeft;
    bool m_levelDone;
    int m_amtGas;
    int m_amtVaccine;
    int m_amtLandmine;
};

#endif // STUDENTWORLD_H_
