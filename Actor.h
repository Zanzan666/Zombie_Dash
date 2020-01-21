#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#include <iostream>
#include <string>
#include <vector>
#include "GameConstants.h"
using namespace std;

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(int imageID, double startX, double startY, Direction dir, int depth,StudentWorld* world):GraphObject(imageID, startX, startY,dir,depth)
    {
        m_world=world;
        m_isAlive=true;
        m_isInfected=false;
        m_infectionLevel=0;
    }
    //whether the actor is alive
    bool isAlive() const {return m_isAlive;}
    //whether the actor is infected
    bool isInfected() const {return m_isInfected;}
    //set the actor to the not infected state and clear the infection level
    void setNotInfected(){m_isInfected=false;m_infectionLevel=0;}
    //set the actor to be infected
    void setInfected(){m_isInfected=true;}
    //set the actor to dead
    void setDead() {m_isAlive=false;}
    //return the world's pointer
    StudentWorld* getWorld() const {return m_world;}
    //return the infection level
    int getInfectionLevel() const {return m_infectionLevel;}
    //increase infectiomn level by 1
    void increaseInfection() {m_infectionLevel+=1;}
    //when it is vommited
    virtual bool vomitted()=0;
    //do something function
    virtual void doSomething()=0;
    //whether if it can block movements
    virtual bool movementBlockable()=0;
    //when a flame is passing the actor
    virtual void flamePassing()=0;
    //whether the actor is flame passable, true if not blocking flame
    virtual bool flamePassable()=0;
    //when the actor is trying to exit. true if it can exit
    virtual bool exiting()=0;
    //whether the actor is vommitable
    virtual bool vomittable()=0;
    //whether the actor will trigger the landmine
    virtual bool explosive()=0;
    //whether the actor is a zombie
    virtual bool isZombie()=0;
    virtual ~Actor(){}
    
private:
    StudentWorld* m_world;
    bool m_isAlive;
    bool m_isInfected;
    int m_infectionLevel;
};

class Penelope : public Actor
{
public:
    Penelope(double startX, double startY,StudentWorld* world):Actor(IID_PLAYER,startX,startY,0,0,world){}
    virtual bool vomitted();
    virtual bool movementBlockable(){return true;}
    virtual bool exiting();
    virtual void flamePassing();
    virtual bool flamePassable(){return true;}
    virtual bool vomittable(){return true;}
    virtual bool isZombie(){return false;}
    virtual bool explosive(){return true;}
    virtual void doSomething();
    virtual ~Penelope(){}
};



class Wall : public Actor
{
public:
    Wall(double startX, double startY, StudentWorld* world):Actor(IID_WALL,startX,startY,0,0,world){}
    virtual bool vomitted(){return false;}
    virtual bool movementBlockable(){return true;}
    virtual bool exiting(){return false;}
    virtual void flamePassing(){}
    virtual bool flamePassable(){return false;}
    virtual bool vomittable(){return false;}
    virtual bool isZombie(){return false;}
    virtual bool explosive(){return false;}
    virtual void doSomething(){};
    virtual ~Wall(){}
};


class Exit: public Actor
{
public:
    Exit(double startX, double startY, StudentWorld* world):Actor(IID_EXIT,startX,startY,0,1,world){}
    virtual bool vomitted(){return false;}
    virtual bool movementBlockable(){return false;}
    virtual bool exiting(){return false;}
    virtual void flamePassing(){}
    virtual bool flamePassable(){return false;}
    virtual bool vomittable(){return false;}
    virtual bool isZombie(){return false;}
    virtual bool explosive(){return false;}
    virtual void doSomething();
    virtual ~Exit(){}
};

class Pit: public Actor
{
public:
    Pit(double startX, double startY, StudentWorld* world): Actor(IID_PIT,startX, startY, 0,0,world){}
    virtual bool vomitted(){return false;}
    virtual bool movementBlockable(){return false;}
    virtual bool exiting(){return false;}
    virtual void flamePassing(){}
    virtual bool flamePassable(){return true;}
    virtual bool vomittable(){return false;}
    virtual bool isZombie(){return false;}
    virtual bool explosive(){return false;}
    virtual void doSomething();
    virtual ~Pit(){}
};


class Flame: public Actor
{
public:
    Flame(double startX, double startY, Direction dir, StudentWorld* world):Actor(IID_FLAME,startX,startY,dir,0,world){m_tick=0;}
    virtual bool vomitted(){return false;}
    virtual bool movementBlockable(){return false;}
    virtual bool exiting(){return false;}
    virtual void flamePassing(){}
    virtual bool flamePassable(){return true;}
    virtual bool vomittable(){return false;}
    virtual bool isZombie(){return false;}
    virtual bool explosive(){return false;}
    virtual void doSomething();
    virtual ~Flame(){}
private:
    int m_tick;
};

class Vomit: public Actor
{
public:
    Vomit(double startX, double startY, Direction dir, StudentWorld* world):Actor(IID_VOMIT,startX,startY,dir,0,world)
    {
        m_tick=0;
    }
    virtual bool vomitted(){return false;}
    virtual bool movementBlockable(){return false;}
    virtual bool exiting(){return false;}
    virtual void flamePassing(){}
    virtual bool flamePassable(){return true;}
    virtual bool vomittable(){return false;}
    virtual bool isZombie(){return false;}
    virtual bool explosive(){return false;}
    virtual void doSomething();
    virtual ~Vomit(){}
private:
    int m_tick;
};



class Goodie: public Actor
{
public:
    Goodie(int imageID, double startX, double startY,StudentWorld* world):Actor(imageID, startX, startY,0,1,world){}
    virtual bool vomitted(){return false;}
    virtual bool movementBlockable(){return false;}
    virtual bool exiting(){return false;}
    virtual void flamePassing();
    virtual bool flamePassable(){return true;}
    virtual bool vomittable(){return false;}
    virtual bool isZombie(){return false;}
    virtual bool explosive(){return false;}
    virtual void doSomething()=0;
    virtual ~Goodie(){}
};

class VaccineGoodie: public Goodie
{
public:
    VaccineGoodie(double startX, double startY,StudentWorld* world):Goodie(IID_VACCINE_GOODIE, startX, startY,world){}
    virtual void doSomething();
    virtual ~VaccineGoodie(){}
};


class GasGoodie: public Goodie
{
public:
    GasGoodie(double startX, double startY,StudentWorld* world):Goodie(IID_GAS_CAN_GOODIE, startX, startY,world){}
    virtual void doSomething();
    virtual ~GasGoodie(){}
};

class LandmineGoodie: public Goodie
{
public:
    LandmineGoodie(double startX, double startY,StudentWorld* world):Goodie(IID_LANDMINE_GOODIE, startX, startY,world){}
    virtual void doSomething();
    virtual ~LandmineGoodie(){};
};


class Landmine: public Actor
{
public:
    Landmine(double startX, double startY,StudentWorld* world):Actor(IID_LANDMINE,startX,startY,0,1,world)
    {
        m_active=false; //whether the landmine is in an active state
        m_safetyTick=30;
    }
    void explode();
    virtual bool vomitted(){return false;}
    virtual bool movementBlockable(){return false;}
    virtual bool exiting(){return false;}
    virtual void flamePassing();
    virtual bool flamePassable(){return true;}
    virtual bool vomittable(){return false;}
    virtual bool isZombie(){return false;}
    virtual bool explosive(){return false;}
    virtual void doSomething();
    virtual ~Landmine(){}
private:
    bool m_active;
    int m_safetyTick;
};


class DumbZombie: public Actor
{
public:
    DumbZombie(double startX, double startY, StudentWorld* world):Actor(IID_ZOMBIE,startX, startY, 0, 0, world)
    {
        m_paralysis=true;   //paralysis state
        m_movementplan=0;
        m_vomitX=0; //vomit coord x
        m_vomitY=0; //vomit coord y
        m_destX=0;  //destination coord x
        m_destY=0;  //destinatiojn coord y
    }
    virtual bool vomitted(){return false;}
    virtual bool movementBlockable(){return true;}
    virtual bool exiting(){return false;}
    virtual void flamePassing();
    virtual bool flamePassable(){return true;}
    virtual bool vomittable(){return false;}
    virtual bool isZombie(){return true;}
    virtual bool explosive(){return true;}
    virtual void doSomething();
    virtual ~DumbZombie(){}
private:
    bool m_paralysis;
    int m_movementplan;
    int m_vomitX;
    int m_vomitY;
    int m_destX;
    int m_destY;
};

class SmartZombie: public Actor
{
public:
    SmartZombie(double startX, double startY, StudentWorld* world):Actor(IID_ZOMBIE,startX, startY, 0, 0, world)
    {
        m_paralysis=true;   //paralysis state
        m_movementplan=0;
        m_vomitX=0; //vomit coord x
        m_vomitY=0; //vomit coord y
        m_destX=0;  //destination coord x
        m_destY=0;  //destinatiojn coord y
    }
    virtual bool vomitted(){return false;}
    virtual bool movementBlockable(){return true;}
    virtual bool exiting(){return false;}
    virtual void flamePassing();
    virtual bool flamePassable(){return true;}
    virtual bool vomittable(){return false;}
    virtual bool isZombie(){return true;}
    virtual bool explosive(){return true;}
    virtual void doSomething();
    virtual ~SmartZombie(){}
private:
    bool m_paralysis;
    int m_movementplan;
    int m_vomitX;
    int m_vomitY;
    int m_destX;
    int m_destY;
};


class Citizen: public Actor
{
public:
    Citizen(double startX, double startY, StudentWorld* world):Actor(IID_CITIZEN,startX, startY, 0, 0, world)
    {
        m_paralysis=true;
        m_distP=0;  //distance from Penelope
        m_distZ=0;  //distance from closest zombie
    }
    virtual bool vomitted();
    virtual bool movementBlockable(){return true;}
    virtual bool exiting();
    virtual void flamePassing();
    virtual bool flamePassable(){return true;}
    virtual bool vomittable(){return true;}
    virtual bool isZombie(){return false;}
    virtual bool explosive(){return true;}
    virtual void doSomething();
    virtual ~Citizen(){}
private:
    bool m_paralysis;
    int m_distP;
    int m_distZ;
};


#endif // ACTOR_H_
