#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


/////////////////////
///Penelope Basics///
/////////////////////


//when penelope is vommited
bool Penelope::vomitted()
{
    setInfected();
    return true;
}

//when penelope is trying to exit
bool Penelope::exiting()
{
    return true;
}

//when penelope is burned by a flame
void Penelope::flamePassing()
{
    setDead();
    getWorld()->playSound(SOUND_PLAYER_DIE);
}

////////////////////
///Zombies Basics///
////////////////////

//when the dumb zombie is burned by a flame
void DumbZombie::flamePassing()
{
    setDead();
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    getWorld()->increaseScore(1000);
    int chance=randInt(1, 10);
    if (chance==1)
        getWorld()->addVaccineGoodieWhenZombieIsAt(getX(), getY());
}

//when a smart zombie is burned by a flame
void SmartZombie::flamePassing()
{
    setDead();
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    getWorld()->increaseScore(2000);
}

////////////////////
///Citizen Basics///
////////////////////

//when the citizen is burned by a flame
void Citizen::flamePassing()
{
    setDead();
    getWorld()->playSound(SOUND_CITIZEN_DIE);
    getWorld()->increaseScore(-1000);
}

//when the citizen is vommited
bool Citizen::vomitted()
{
    if (isInfected())
        return true;
    setInfected();
    getWorld()->playSound(SOUND_CITIZEN_INFECTED);
    return true;
}

//when the citizen is trying to exit
bool Citizen::exiting()
{
    return true;
}

//////////////////////////////
///Rest of the Items Basics///
//////////////////////////////
//When the goodie is burned by a flame
void Goodie::flamePassing()
{
    setDead();
}

//when the landmine is burned by a flame
void Landmine::flamePassing()
{
    explode();
    getWorld()->addPit(getX(),getY());
}

//landmine is explode either due to flameplassing or triggered by humans or zombies passing
void Landmine::explode()
{
    setDead();
    getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
    //add flames at all directions
    getWorld()->addFlame(getX(),getY(),up);
    getWorld()->addFlame(getX()+SPRITE_WIDTH,getY()+SPRITE_HEIGHT,up);
    getWorld()->addFlame(getX()+SPRITE_WIDTH,getY()-SPRITE_HEIGHT,up);
    getWorld()->addFlame(getX()+SPRITE_WIDTH,getY(),up);
    getWorld()->addFlame(getX()-SPRITE_WIDTH,getY()+SPRITE_HEIGHT,up);
    getWorld()->addFlame(getX()-SPRITE_WIDTH,getY()-SPRITE_HEIGHT,up);
    getWorld()->addFlame(getX()-SPRITE_WIDTH,getY(),up);
    getWorld()->addFlame(getX(),getY()+SPRITE_HEIGHT,up);
    getWorld()->addFlame(getX(),getY()-SPRITE_HEIGHT,up);
}






///////////////////////////
///All the do somethings///
///////////////////////////


void Penelope::doSomething()
{
    if (!isAlive())
        return;
    if (isInfected())
        increaseInfection();
    if (getInfectionLevel()>=500)
    {
        setDead();
        getWorld()->playSound(SOUND_PLAYER_DIE);
        return;
    }
    int ch;
    if (getWorld()->getKey(ch))
    {
        // user hit a key during this tick!
        switch (ch)
        {
            case KEY_PRESS_SPACE:
                if (getWorld()->getAmtGas()>0)
                {
                    getWorld()->useGas();
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    bool continueFiring=true;
                    //add up to three new flame objects to the game, if overlap with a wall or exit, it immediately stop the loop
                    switch (getDirection())
                    {
                        case up:
                            for(int i=1;i<4 && continueFiring;i++)
                            {
                                continueFiring=getWorld()->addFlame(getX(),getY()+i*SPRITE_HEIGHT,getDirection());
                            }
                            break;
                        case down:
                            for(int i=1;i<4 && continueFiring;i++)
                            {
                                continueFiring=getWorld()->addFlame(getX(),getY()-i*SPRITE_HEIGHT,getDirection());
                            }
                            break;
                        case left:
                            for(int i=1;i<4 && continueFiring;i++)
                            {
                                continueFiring=getWorld()->addFlame(getX()-i*SPRITE_WIDTH,getY(),getDirection());
                            }
                            break;
                        case right:
                            for(int i=1;i<4 && continueFiring;i++)
                            {
                                continueFiring=getWorld()->addFlame(getX()+i*SPRITE_WIDTH,getY(),getDirection());
                            }
                            break;
                    }
                }
                break;
            case KEY_PRESS_TAB:
                if (getWorld()->getAmtLandmine()>0)
                {
                    getWorld()->useLandmine();
                    getWorld()->addLandmine(getX(), getY());
                }
                break;
            case KEY_PRESS_ENTER:
                if (getWorld()->getAmtVaccine()>0)
                {
                    setNotInfected();
                }
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                for (int i=0;i<4;i++)
                {
                    if (getWorld()->isMovableAt(getX(),getY()-1,this))
                        moveTo(getX(), getY()-1);
                    else break;
                }
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                for (int i=0;i<4;i++)
                {
                    if (getWorld()->isMovableAt(getX(), getY()+1,this))
                        moveTo(getX(), getY()+1);
                    else break;
                }
                break;
            case KEY_PRESS_LEFT:
                setDirection(left);
                for (int i=0;i<4;i++)
                {
                    if (getWorld()->isMovableAt(getX()-1, getY(),this))
                        moveTo(getX()-1, getY());
                    else break;
                }
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                for (int i=0;i<4;i++)
                {
                    if (getWorld()->isMovableAt(getX()+1, getY(),this))
                        moveTo(getX()+1, getY());
                    else break;
                }
                break;
        }
    }
}


void Exit::doSomething()
{
    getWorld()->theExitOverlapWithCitizens(getX(), getY());
    getWorld()->determineIfLeavable(getX(), getY());
}

void Pit::doSomething()
{
    getWorld()->OverlapWithFlame(getX(), getY());
}

void Flame::doSomething()
{
    if (!isAlive())
        return;
    m_tick++;   //when two tick pass, set it to dead
    getWorld()->OverlapWithFlame(getX(), getY());   //when something overlap with flame, it burned. this is to try to triger every actor to pass the flame
    if (m_tick==2)
    {
        setDead();
        return;
    }
}

void Vomit::doSomething()
{
    if (!isAlive())
        return;
    m_tick++;   //when two tick pass, set it to dead
    getWorld()->OverlapWithVomit(getX(), getY());//when humans overlap with vomit, he will be infected. this is to try to trigger every humans to pass the vomit
    if (m_tick==2)
    {
        setDead();
        return;
    }
}

void VaccineGoodie::doSomething()
{
    if (!isAlive())
        return;
    if (getWorld()->OverlapWithPenelope(getX(), getY()))    //penelope get the vaccine goodie
    {
        getWorld()->increaseScore(50);
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->pickUpVaccine();
    }
}



void GasGoodie::doSomething()
{
    if (!isAlive())
        return;
    if (getWorld()->OverlapWithPenelope(getX(), getY()))    //penelope get the gas goodie
    {
        getWorld()->increaseScore(50);
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->pickUpGas();
    }
}

void LandmineGoodie::doSomething()
{
    if (!isAlive())
        return;
    if (getWorld()->OverlapWithPenelope(getX(), getY()))    //penelope get the landmine goodie
    {
        getWorld()->increaseScore(50);
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->pickUpLandmine();
    }
}

void Landmine::doSomething()
{
    if (!isAlive())
        return;
    if (!m_active)
    {
        m_safetyTick--; //when 30 ticks pass, it will be activated
        if (m_safetyTick==0)
            m_active=true;
        return;
    }
    if(getWorld()->OverlapWithLandmine(getX(), getY())) //when triggered
    {
        explode();
        getWorld()->addPit(getX(),getY());  //leave a pit
    }
}

void DumbZombie::doSomething()
{
    if (!isAlive())
        return;
    m_paralysis=!m_paralysis;   //flip the paralysis state
    if (m_paralysis)    //if paralized, do nothing
        return;
    //determine the vomit coord
    switch (getDirection())
    {
        case right:
            m_vomitX=getX()+SPRITE_WIDTH;
            m_vomitY=getY();
            break;
        case left:
            m_vomitX=getX()-SPRITE_WIDTH;
            m_vomitY=getY();
            break;
        case up:
            m_vomitX=getX();
            m_vomitY=getY()+SPRITE_HEIGHT;
            break;
        case down:
            m_vomitX=getX();
            m_vomitY=getY()-SPRITE_HEIGHT;
            break;
    }
    
    //if a person is in front of it in the direction it is facing
    if (getWorld()->GoingToVomitAt(m_vomitX, m_vomitY))
    {
        int chance=randInt(1, 3);
        if (chance==1)  //1 in 3 chance that the dumb zombie will vomit
        {
            getWorld()->addVomit(m_vomitX,m_vomitY,getDirection());
            getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
            return;
        }
    }
    //check to see if it needs a new movement plan
    if (m_movementplan==0)
    {
        m_movementplan=randInt(3, 10);  //pick a new random movement plan distance in the range 3 through 10 inclusive
        int dir=randInt(1,4);   //random direction
        if (dir==1)
            setDirection(right);
        if (dir==2)
            setDirection(left);
        if (dir==3)
            setDirection(up);
        if (dir==4)
            setDirection(down);
    }
    
    //determine a destination coordinate, if movable, update the location and decrease the movement plan by 1, otherwise, set the movement plan to 0
    switch (getDirection())
    {
        case right:
            m_destX=getX()+1;
            m_destY=getY();
            if (getWorld()->isMovableAt(m_destX,m_destY,this))
            {
                moveTo(m_destX, m_destY);
                m_movementplan--;
            }
            else m_movementplan=0;
            break;
        case left:
            m_destX=getX()-1;
            m_destY=getY();
            if (getWorld()->isMovableAt(m_destX,m_destY,this))
            {
                moveTo(m_destX, m_destY);
                m_movementplan--;
            }
            else m_movementplan=0;
            break;
        case up:
            m_destX=getX();
            m_destY=getY()+1;
            if (getWorld()->isMovableAt(m_destX,m_destY,this))
            {
                moveTo(m_destX, m_destY);
                m_movementplan--;
            }
            else m_movementplan=0;
            break;
        case down:
            m_destX=getX();
            m_destY=getY()-1;
            if (getWorld()->isMovableAt(m_destX,m_destY,this))
            {
                moveTo(m_destX, m_destY);
                m_movementplan--;
            }
            else m_movementplan=0;
            break;
    }
}

void SmartZombie::doSomething()
{
    if (!isAlive())
        return;
    m_paralysis=!m_paralysis;   //flip the paralysis state
    if (m_paralysis)    //if paralized, do nothing
        return;
    //determine the vomit coord
    switch (getDirection())
    {
        case right:
            m_vomitX=getX()+SPRITE_WIDTH;
            m_vomitY=getY();
            break;
        case left:
            m_vomitX=getX()-SPRITE_WIDTH;
            m_vomitY=getY();
            break;
        case up:
            m_vomitX=getX();
            m_vomitY=getY()+SPRITE_HEIGHT;
            break;
        case down:
            m_vomitX=getX();
            m_vomitY=getY()-SPRITE_HEIGHT;
            break;
    }
    
    //if a person is in front of it in the direction it is facing
    if (getWorld()->GoingToVomitAt(m_vomitX, m_vomitY))
    {
        int chance=randInt(1, 3);
        if (chance==1)  //1 in 3 chance that the smart zombie will vomit
        {
            getWorld()->addVomit(m_vomitX,m_vomitY,getDirection());
            getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
            return;
        }
    }
    //check to see if it needs a new movement plan
    if (m_movementplan==0)
    {
        //Pick a new random movement plan distance in the range 3 through 10 inclusive.
        m_movementplan=randInt(3, 10);
        
        //Smartly find the direction
        /*
        Select the person (Penelope or a citizen) closest to the smart zombie, i.e., the one whose Euclidean distance from the zombie is the smallest. If more than one person is the same smallest distance away, select one of them.
        Set its direction to a random direction:
            i. If the distance to the selected nearest person is more than 80 pixels away, the direction is chosen from up, down, left, and right.
            ii. Otherwise, the nearest person is less than or equal to 80 pixels away, the direction is chosen to be one that would cause the zombie to get closer to the person:
                1. If the zombie is on the same row or column as the person, choose the (only) direction that gets the zombie closer.
                2. Otherwise, choose randomly between the two directions (one horizontal and one vertical) that get the zombie closer.
         */
        
        
        setDirection(getWorld()->findClosestHuman(getX(), getY()));
    }
    
        //determine a destination coordinate, if movable, update the location and decrease the movement plan by 1, otherwise, set the movement plan to 0
    switch (getDirection())
    {
        case right:
            m_destX=getX()+1;
            m_destY=getY();
            if (getWorld()->isMovableAt(m_destX,m_destY,this))
            {
                moveTo(m_destX, m_destY);
                m_movementplan--;
            }
            else m_movementplan=0;
            break;
        case left:
            m_destX=getX()-1;
            m_destY=getY();
            if (getWorld()->isMovableAt(m_destX,m_destY,this))
            {
                moveTo(m_destX, m_destY);
                m_movementplan--;
            }
            else m_movementplan=0;
            break;
        case up:
            m_destX=getX();
            m_destY=getY()+1;
            if (getWorld()->isMovableAt(m_destX,m_destY,this))
            {
                moveTo(m_destX, m_destY);
                m_movementplan--;
            }
            else m_movementplan=0;
            break;
        case down:
            m_destX=getX();
            m_destY=getY()-1;
            if (getWorld()->isMovableAt(m_destX,m_destY,this))
            {
                moveTo(m_destX, m_destY);
                m_movementplan--;
            }
            else m_movementplan=0;
            break;
    }
}


void Citizen::doSomething()
{
    if (!isAlive())
        return;
    if (isInfected())
    {
        increaseInfection();
        if (getInfectionLevel()>=500)
        {
            //if infection level reaches 500, turn it into a zombie by killing the person and adding a new zombie to the same position
            setDead();
            getWorld()->addZombie(getX(), getY());
            return;
        }
    }
    m_paralysis=!m_paralysis;   //flip the paralysis state
    if (m_paralysis)
        return;
    getWorld()->citizenMove(getX(), getY(),this);   //move the citizen using StudentWorld
}
