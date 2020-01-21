#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
using namespace std;

//when a gas goodie is picked up by penelope
void StudentWorld::pickUpGas()
{
    m_amtGas+=5;
}

//when a vaccine goodie is picked up by penelope
void StudentWorld::pickUpVaccine()
{
    m_amtVaccine++;
}

//when a landmine goodie is picked up by penelope
void StudentWorld::pickUpLandmine()
{
    m_amtLandmine+=2;
}
//return the amount of gas/flame penelope is holding
int StudentWorld::getAmtGas() const
{
    return m_amtGas;
}

//return the amount of vaccine penelope is holding
int StudentWorld::getAmtVaccine() const
{
    return m_amtVaccine;
}

//return the amount of landmine penelope is holding
int StudentWorld::getAmtLandmine() const
{
    return m_amtLandmine;
}



GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_amtGas=0;
    m_amtVaccine=0;
    m_amtLandmine=0;
    m_numOfCitizensLeft=0;
    m_initialized=false;
    m_levelDone=false;
    m_player=nullptr;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    m_initialized=false;
    m_player=nullptr;
    m_amtGas=0;
    m_amtVaccine=0;
    m_amtLandmine=0;
    m_numOfCitizensLeft=0;
    m_levelDone=false;
    Level lev(assetPath());
    int level=getLevel();
    ostringstream oss;
    if (level>=1 && level <=9)
        oss<<"level0"<<level<<".txt";
    else
        oss<<"level"<<level<<".txt";
    string levelFile = oss.str();
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
    {
        cerr << "Cannot find level"<<level<<".txt data file" << endl;
        return GWSTATUS_PLAYER_WON;
    }
    else if (result == Level::load_fail_bad_format)
    {
        cerr << "Your level was improperly formatted" << endl;
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        for (int x=0;x<VIEW_WIDTH;x++)
        {
            for (int y=0;y<VIEW_HEIGHT;y++)
            {
                //adding all the elements to the pointer m_player and Actor pointer list
                Level::MazeEntry ge = lev.getContentsOf(x,y);
                switch (ge)
                {
                    case Level::empty:
                        break;
                    case Level::player:
                        m_player=new Penelope(SPRITE_WIDTH*x,SPRITE_HEIGHT*y,this);
                        break;
                    case Level::wall:
                        m_actors.push_back(new Wall(SPRITE_WIDTH*x,SPRITE_HEIGHT*y,this));
                        break;
                        
                    case Level::exit:
                        m_actors.push_back(new Exit(SPRITE_WIDTH*x,SPRITE_HEIGHT*y,this));
                        break;
                        
                    case Level::citizen:
                        m_numOfCitizensLeft++;
                        m_actors.push_back(new Citizen(SPRITE_WIDTH*x,SPRITE_HEIGHT*y,this));
                        break;
                    case Level::smart_zombie:
                        m_actors.push_back(new SmartZombie(SPRITE_WIDTH*x,SPRITE_HEIGHT*y,this));
                        break;
                    case Level::dumb_zombie:
                        m_actors.push_back(new DumbZombie(SPRITE_WIDTH*x,SPRITE_HEIGHT*y,this));
                        break;
                    case Level::pit:
                         m_actors.push_back(new Pit(SPRITE_WIDTH*x,SPRITE_HEIGHT*y,this));
                        break;
                    case Level::vaccine_goodie:
                         m_actors.push_back(new VaccineGoodie(SPRITE_WIDTH*x,SPRITE_HEIGHT*y,this));
                        break;
                    case Level::gas_can_goodie:
                        m_actors.push_back(new GasGoodie(SPRITE_WIDTH*x,SPRITE_HEIGHT*y,this));
                        break;
                    case Level::landmine_goodie:
                        m_actors.push_back(new LandmineGoodie(SPRITE_WIDTH*x,SPRITE_HEIGHT*y,this));
                        break;
                }
            }
        }
        m_initialized=true;
    }
    return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move()
{
    DisplayText();
    m_player->doSomething();    //Penelope do somehting first
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
        (*p)->doSomething();    //all rest of the actors do somehting
    
    //kill the dead actors
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
    {
        if(!(*p)->isAlive())
        {
            if ((*p)->exiting())
                m_numOfCitizensLeft--;
            Actor* killme=*p;
            delete killme;
            m_actors.erase(p);
        }
    }
    //if the player is not alive, decrease its live
    if (!m_player->isAlive())
    {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    //when level done and player is alive, move to the next level
    if (m_levelDone)
    {
        playSound(SOUND_LEVEL_FINISHED);
        return GWSTATUS_FINISHED_LEVEL;
    }
    return GWSTATUS_CONTINUE_GAME;
}

//clean all the dynamically allocated Actors when needed
void StudentWorld::cleanUp()
{
    if (!m_initialized)
        return;
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
    {
        Actor* killme=*p;
        delete killme;
        m_actors.erase(p);
    }
    delete m_player;
    m_initialized=false;
}


//this function is for displaying the text on the top of the screen
void StudentWorld::DisplayText()
{
    int score=getScore();
    ostringstream TempScore;
    TempScore<<score;
    ostringstream oss;
    oss<<"Score: ";
    if (score>=0)
    {
        for (int i=0;i<6-TempScore.str().size();i++)
            oss<<"0";
        oss<<score;
    }
    else
    {
        oss<<"-";
        for (int i=0;i<5-TempScore.str().size();i++)
            oss<<"0";
        oss<<-score;
    }
    oss<<"  Level: "<<getLevel()<<"  Lives: "<<getLives()<<"  Vaccines: "<<getAmtVaccine()<<"  Flames: "<<getAmtGas()<<"  Mines: "<<getAmtLandmine()<<"  Infected: "<<m_player->getInfectionLevel();
    setGameStatText(oss.str());
}


//to check if the object is movable at the direction toX and toY, if anyting is blockable in the range, then return false
bool StudentWorld::isMovableAt(int toX, int toY, Actor* current)    //toX toY is the coord the actor is going to, current is the pointer points to the object calling this function
{
    if (current!=m_player)
        if((m_player->getX()-SPRITE_WIDTH)<toX  &&    (m_player->getX()+SPRITE_WIDTH)>toX  &&  (m_player->getY()-SPRITE_HEIGHT)<toY && (m_player->getY()+SPRITE_HEIGHT)>toY) return false;
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
    {
        if ((*p)==current)
            continue;
        if ((*p)->movementBlockable())
        {
            if(((*p)->getX()-SPRITE_WIDTH)<toX  &&    ((*p)->getX()+SPRITE_WIDTH)>toX  &&  ((*p)->getY()-SPRITE_HEIGHT)<toY && ((*p)->getY()+SPRITE_HEIGHT)>toY) return false;
        }
    }
    
    return true;
}

//to check if any citizen is overlaping with exit, if so, all citizens overlaping with the exit is saved
void StudentWorld::theExitOverlapWithCitizens(int x, int y) //x y is the coord of the exit calling thsi function
{
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
    {
        if( (pow(((*p)->getX()-x),2.0)+ pow(((*p)->getY()-y),2.0))<=100 )
        {
            if((*p)->exiting())
            {
                increaseScore(500);
                (*p)->setDead();
                playSound(SOUND_CITIZEN_SAVED);
            }
        }
    }
}

//if m_player is at exit and there is no citizen left, level is done
void StudentWorld::determineIfLeavable(int x, int y)    // x y is the coord of the exit calling this function
{
    if (m_numOfCitizensLeft==0  &&  (pow(m_player->getX()-x,2.0)+ pow(m_player->getY()-y,2.0))<=100)
        m_levelDone=true;
}

//to check if the coord x y is overlaping with Penelope
bool StudentWorld::OverlapWithPenelope(int x, int y)    //x y is the coord of the calling object
{
    if ( (pow((m_player->getX()-x),2.0)+ pow((m_player->getY()-y),2.0))<=100)
        return true;
    return false;
}

//to check if anything overlaping with flame and try to burn them if flamePassable
void StudentWorld::OverlapWithFlame(int x, int y)   //x y is the coord of the calling object flame
{
    if( (pow((m_player->getX()-x),2.0)+ pow((m_player->getY()-y),2.0))<=100)
    {
        m_player->flamePassing();
    }
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
    {
        if (!(*p)->flamePassable())
            continue;
        if((pow(((*p)->getX()-x),2.0)+ pow(((*p)->getY()-y),2.0))<=100)
        {
            (*p)->flamePassing();
        }
    }
}

//to check if anything explosive is overlaping with landmine and if so the landmine will explode (the function will return true)
bool StudentWorld::OverlapWithLandmine(int x, int y)    //x y is the coord of calling object landmine
{
    if( (pow((m_player->getX()-x),2.0)+ pow((m_player->getY()-y),2.0))<=100)
    {
        return true;
    }
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
    {
        if((pow(((*p)->getX()-x),2.0)+ pow(((*p)->getY()-y),2.0))<=100)
        {
            if ((*p)->explosive())
                return true;
        }
    }
    return false;
}

//to check if the flame if flamable at the position x,y
bool StudentWorld::isFlamableAt(int x, int y)   //x y is the coord of the flame that is going to be created
{
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
    {
        if ((*p)->flamePassable())
            continue;
        if( (pow(((*p)->getX()-x),2.0)+ pow(((*p)->getY()-y),2.0))<=100)
        {
            return false;
        }
    }
    return true;
}


//the vomit(already exists) try to vomit everything, sometimes it may do nothing
void StudentWorld::OverlapWithVomit(int x, int y)   //x y is the coord of vomit
{
    if( (pow((m_player->getX()-x),2.0)+ pow((m_player->getY()-y),2.0))<=100)
        m_player->vomitted();
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
    {
        if( (pow(((*p)->getX()-x),2.0)+ pow(((*p)->getY()-y),2.0))<=100)
        {
            (*p)->vomitted();
        }
    }
}

//to check if any person is overlaping with the vomit coord, if so, return true
bool StudentWorld::GoingToVomitAt(int x, int y) //x y is the coord of vomit going to be created
{
    if( (pow((m_player->getX()-x),2.0)+ pow((m_player->getY()-y),2.0))<=100)
        return true;
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
    {
        if( (pow(((*p)->getX()-x),2.0)+ pow(((*p)->getY()-y),2.0))<=100)
        {
            if((*p)->vomittable())
                return true;
        }
    }
    return false;
}


//add flames, if not flamable, return false, otherwise true
bool StudentWorld::addFlame(int x,int y,Direction dir)  //x y is the coord of the flame going to be added, dir is its direction
{
    if (!isFlamableAt(x,y))
        return false;
    m_actors.push_front(new Flame(x,y,dir,this));
    return true;
}

//add vomit at the position
void StudentWorld::addVomit(int x, int y, Direction dir)    //x y is the coord of the vomit going to be added, dir is its direction
{
    m_actors.push_front(new Vomit(x,y,dir,this));
}


//find the closest human and return the direction it should have
Direction StudentWorld::findClosestHuman(int x, int y)  //x y is the coord of the zombie calling this function
{
    
    //Smartly find the direction
    /*
     Select the person (Penelope or a citizen) closest to the smart zombie, i.e., the one whose Euclidean distance from the zombie is the smallest. If more than one person is the same smallest distance away, select one of them.
     Set its direction to a random direction:
     i. If the distance to the selected nearest person is more than 80 pixels away, the direction is chosen from up, down, left, and right.
     ii. Otherwise, the nearest person is less than or equal to 80 pixels away, the direction is chosen to be one that would cause the zombie to get closer to the person:
     1. If the zombie is on the same row or column as the person, choose the (only) direction that gets the zombie closer.
     2. Otherwise, choose randomly between the two directions (one horizontal and one vertical) that get the zombie closer.
     */
    
    int minimum=sqrt((pow((m_player->getX()-x),2.0)+ pow((m_player->getY()-y),2.0)));
    int preyX=m_player->getX();
    int preyY=m_player->getY();
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
    {
        if( sqrt((pow(((*p)->getX()-x),2.0)+ pow(((*p)->getY()-y),2.0)))<=minimum)
        {
            if ((*p)->exiting())
            {
                minimum=sqrt((pow(((*p)->getX()-x),2.0)+ pow(((*p)->getY()-y),2.0)));
                preyX=(*p)->getX();
                preyY=(*p)->getY();
            }
        }
    }
    if (preyX==x)
    {
        if (preyY<=y)
            return Actor::down;
        else return Actor::up;
    }
    if (preyY==y)
    {
        if (preyX<=x)
            return Actor::left;
        else return Actor::right;
    }
    int chance=randInt(1, 2);
    if (chance==1)
    {
        if (preyX<=x)
            return Actor::left;
        else return Actor::right;
    }
    if (preyY<=y)
        return Actor::down;
    else return Actor::up;
}

//when the citizen is infected, a new zombie will be born
void StudentWorld::addZombie(int x, int y)  //x y is the coord of the zombie going to be added
{
    playSound(SOUND_ZOMBIE_BORN);
    increaseScore(-1000);
    int chance=randInt(1, 10);
    if (chance<=3)
        m_actors.push_front(new SmartZombie(x,y,this));
    else
        m_actors.push_front(new DumbZombie(x,y,this));
}

//find the distance to the closest zombie, if no zombie in the level, return the number large enough to be neglected in comparison to other(e.g. distance to penelope)
double StudentWorld::findClosestZombie(int x, int y)    //x y is the coord of citizen calling this function
{
    
    double dist_z=256*256;
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
    {
        if( sqrt((pow(((*p)->getX()-x),2.0)+ pow(((*p)->getY()-y),2.0)))<dist_z  &&  (*p)->isZombie())
        {
            dist_z=sqrt((pow(((*p)->getX()-x),2.0)+ pow(((*p)->getY()-y),2.0)));
        }
    }
    return dist_z;
}

//when citizen is doing something, the citizen use this function
void StudentWorld::citizenMove(int x, int y, Actor*thisCitizen) //x y is the coord of the citizen calling this functioni and thisCitizen is the pointer of the citizen calling the function
{
    //The citizen must determine its distance to Penelope: dist_p
    double dist_p=sqrt((pow((m_player->getX()-x),2.0)+ pow((m_player->getY()-y),2.0)));
    // The citizen must determine its distance to the nearest zombie: dist_z
    double dist_z=findClosestZombie(x, y);
    
    //If dist_p < dist_z or no zombies exist in the level (so dist_z is irrelevant), and the citizen's Euclidean distance from Penelope is less than or equal to 80 pixels (so the citizen wants to follow Penelope):
    if (dist_p<dist_z)
    {
        if (dist_p<=80)
        {
            if (x==m_player->getX())
            {
                if (y<m_player->getY())
                {
                    if (isMovableAt(x,y+2,thisCitizen))
                    {
                        thisCitizen->setDirection(Actor::up);
                        thisCitizen->moveTo(x, y+2);
                        return;
                    }
                }
                if (y>m_player->getY())
                {
                    if (isMovableAt(x,y-2,thisCitizen))
                    {
                        thisCitizen->setDirection(Actor::down);
                        thisCitizen->moveTo(x, y-2);
                        return;
                    }
                }
            }
            if (y==m_player->getY())
            {
                if (x<m_player->getX())
                {
                    if (isMovableAt(x+2,y,thisCitizen))
                    {
                        thisCitizen->setDirection(Actor::right);
                        thisCitizen->moveTo(x+2, y);
                        return;
                    }
                }
                if (x>m_player->getX())
                {
                    if (isMovableAt(x-2,y,thisCitizen))
                    {
                        thisCitizen->setDirection(Actor::left);
                        thisCitizen->moveTo(x-2, y);
                        return;
                    }
                }
            }
            //there are two possible dirction, randomly choose one
            int chance=randInt(1, 2);
            if (chance==1)
            {
                //horizontal first, if blocked then vertical
                if (x<m_player->getX())
                {
                    if (isMovableAt(x+2,y,thisCitizen))
                    {
                        thisCitizen->setDirection(Actor::right);
                        thisCitizen->moveTo(x+2, y);
                        return;
                    }
                }
                if (x>m_player->getX())
                {
                    if (isMovableAt(x-2,y,thisCitizen))
                    {
                        thisCitizen->setDirection(Actor::left);
                        thisCitizen->moveTo(x-2, y);
                        return;
                    }
                }
                if (y<m_player->getY())
                {
                    if (isMovableAt(x,y+2,thisCitizen))
                    {
                        thisCitizen->setDirection(Actor::up);
                        thisCitizen->moveTo(x, y+2);
                        return;
                    }
                }
                if (y>m_player->getY())
                {
                    if (isMovableAt(x,y-2,thisCitizen))
                    {
                        thisCitizen->setDirection(Actor::down);
                        thisCitizen->moveTo(x, y-2);
                        return;
                    }
                }
            }
            if (chance==2)
            {
                //vertical first, if blocked then horizontal
                if (y<m_player->getY())
                {
                    if (isMovableAt(x,y+2,thisCitizen))
                    {
                        thisCitizen->setDirection(Actor::up);
                        thisCitizen->moveTo(x, y+2);
                        return;
                    }
                }
                if (y>m_player->getY())
                {
                    if (isMovableAt(x,y-2,thisCitizen))
                    {
                        thisCitizen->setDirection(Actor::down);
                        thisCitizen->moveTo(x, y-2);
                        return;
                    }
                }
                if (x<m_player->getX())
                {
                    if (isMovableAt(x+2,y,thisCitizen))
                    {
                        thisCitizen->setDirection(Actor::right);
                        thisCitizen->moveTo(x+2, y);
                        return;
                    }
                }
                if (x>m_player->getX())
                {
                    if (isMovableAt(x-2,y,thisCitizen))
                    {
                        thisCitizen->setDirection(Actor::left);
                        thisCitizen->moveTo(x-2, y);
                        return;
                    }
                }
            }
        }
    }
    //If there is a zombie whose Euclidean distance from the citizen is less than or equal to 80 pixels, the citizen wants to run away
    if (dist_z<=80)
    {
        double newdist_z=dist_z;
        Direction newDir=thisCitizen->getDirection();
        if (isMovableAt(thisCitizen->getX(), thisCitizen->getY()+2,thisCitizen))
        {
                if (findClosestZombie(thisCitizen->getX(), thisCitizen->getY()+2) > newdist_z)
                {
                    newDir=Actor::up;
                    newdist_z=findClosestZombie(thisCitizen->getX(), thisCitizen->getY()+2);
                }
        }
        if (isMovableAt(thisCitizen->getX(), thisCitizen->getY()-2,thisCitizen))
        {
            if (findClosestZombie(thisCitizen->getX(), thisCitizen->getY()-2) > newdist_z)
            {
                newDir=Actor::down;
                newdist_z=findClosestZombie(thisCitizen->getX(), thisCitizen->getY()-2);
            }
        }
        if (isMovableAt(thisCitizen->getX()+2, thisCitizen->getY(),thisCitizen))
        {
            if (findClosestZombie(thisCitizen->getX()+2, thisCitizen->getY()) > newdist_z)
            {
                newDir=Actor::right;
                newdist_z=findClosestZombie(thisCitizen->getX()+2, thisCitizen->getY());
            }
        }
        if (isMovableAt(thisCitizen->getX()-2, thisCitizen->getY(),thisCitizen))
        {
            if (findClosestZombie(thisCitizen->getX()-2, thisCitizen->getY()) > newdist_z)
            {
                newDir=Actor::left;
                newdist_z=findClosestZombie(thisCitizen->getX()-2, thisCitizen->getY());
            }
        }
        if (newdist_z<=dist_z)
            return;//If none of the movement options would allow the citizen to get further away from the nearest zombie 
        thisCitizen->setDirection(newDir);
        switch (newDir)
        {
            case Actor::up:
                thisCitizen->moveTo(thisCitizen->getX(), thisCitizen->getY()+2);
                break;
            case Actor::down:
                thisCitizen->moveTo(thisCitizen->getX(), thisCitizen->getY()-2);
                break;
            case Actor::right:
                thisCitizen->moveTo(thisCitizen->getX()+2, thisCitizen->getY());
                break;
            case Actor::left:
                thisCitizen->moveTo(thisCitizen->getX()-2, thisCitizen->getY());
                break;
        }
        return; //At this point, there are no zombies whose Euclidean distance from the citizen is less than or equal to 80 pixels. The citizen does nothing this tick.
    }
}

//add landmine at the position x y
void StudentWorld::addLandmine(int x, int y)
{
    m_actors.push_front(new Landmine(x,y,this));
}

//when a dumb zombie is died, a new vaccine goodie may be added
void StudentWorld::addVaccineGoodieWhenZombieIsAt(int x, int y)
{
    int xVaccine=x,yVaccine=y;
    int chance=randInt(1, 4);
    switch (chance)
    {
        case 1://right
            xVaccine=x+SPRITE_WIDTH;
            break;
        case 2://left
            xVaccine=x-SPRITE_WIDTH;
            break;
        case 3://up
            yVaccine=y-SPRITE_HEIGHT;
            break;
        case 4:
            yVaccine=y+SPRITE_HEIGHT;
            break;
    }
    if( (pow((m_player->getX()-xVaccine),2.0)+ pow((m_player->getY()-yVaccine),2.0))<=100)
        return;
    for (list<Actor*>::iterator p=m_actors.begin();p!=m_actors.end();p++)
    {
        if( (pow(((*p)->getX()-xVaccine),2.0)+ pow(((*p)->getY()-yVaccine),2.0))<=100)
            return;
    }
    m_actors.push_front(new VaccineGoodie(xVaccine,yVaccine,this));
}

//add pit at position x y
void StudentWorld::addPit(int x, int y) //x y is the coord of pit going to be added
{
    m_actors.push_front(new Pit(x,y,this));
}
