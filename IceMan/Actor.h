#ifndef ACTOR_H
#define ACTOR_H

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject {
public:
    Actor(int imageID, int startX, int startY, Direction dir, float size, unsigned int depth, StudentWorld* world);
    virtual ~Actor();
    virtual void doSomething() = 0;
    bool isAlive() const;
    void setDead();
    StudentWorld* getWorld() const;

private:
    bool m_alive;
    StudentWorld* m_world;
};

class Ice : public Actor {
public:
    Ice(int startX, int startY, StudentWorld* world);
    virtual ~Ice();
    virtual void doSomething();
};

class Iceman : public Actor {
public:
    Iceman(StudentWorld* world);
    virtual ~Iceman();
    virtual void doSomething();

private:
    int m_hitPoints;
    int m_water;
    int m_gold;
    int m_sonar;
};

class Boulder : public Actor {
public:
    Boulder(int startX, int startY, StudentWorld* world);
    virtual ~Boulder();
    virtual void doSomething();

private:
    enum State { STABLE, WAITING, FALLING } m_state;
    int m_waitingTicks;
};

class Squirt : public Actor {
public:
    Squirt(int startX, int startY, Direction dir, StudentWorld* world);
    virtual ~Squirt();
    virtual void doSomething();

private:
    int m_distanceTraveled;
};

class GoldNugget : public Actor {
public:
    GoldNugget(int startX, int startY, bool temporary, StudentWorld* world);
    virtual ~GoldNugget();
    virtual void doSomething();

private:
    bool m_temporary;
    int m_ticksLeft;
};

class Protester : public Actor {
public:
    Protester(int imageID, int startX, int startY, StudentWorld* world, int hitPoints);
    virtual ~Protester();
    virtual void doSomething() = 0;
    void getAnnoyed(int points);
    void moveToNextPosition();
    void setRestingTicks(int ticks);
    bool isResting() const;
    void shoutIfCloseToIceman();
    bool canMoveInDirection(Direction dir) const;
    void pickNewDirection();
    void moveToExit();

protected:
    int m_hitPoints;
    int m_restingTicks;
    int m_shoutCooldown;
    int m_numSquaresToMoveInCurrentDirection;
    bool m_leaveOilField;
};

class RegularProtester : public Protester {
public:
    RegularProtester(int startX, int startY, StudentWorld* world);
    virtual ~RegularProtester();
    virtual void doSomething();
};

class HardcoreProtester : public Protester {
public:
    HardcoreProtester(int startX, int startY, StudentWorld* world);
    virtual ~HardcoreProtester();
    virtual void doSomething();
};

class SonarKit : public Actor {
public:
    SonarKit(int startX, int startY, StudentWorld* world);
    virtual ~SonarKit();
    virtual void doSomething();
};

class WaterPool : public Actor {
public:
    WaterPool(int startX, int startY, StudentWorld* world);
    virtual ~WaterPool();
    virtual void doSomething();

private:
    int m_ticksLeft;
};

#endif // ACTOR_H





//-----------------------------------------------------------------------------------------------------------------------------------------------------
//#ifndef ACTOR_H_
//#define ACTOR_H_
//
//#include "GraphObject.h"
//#include <cmath>
//
////TESTING COMMIT 06/04mm
//
//class StudentWorld;
//
//class Actor : public GraphObject
//{
//private:
//    StudentWorld* world;
//    bool m_Alive;
//public:
//    Actor(StudentWorld* world, int imageID, int startX, int startY, Direction startDir,
//       double size, int depth) :
//        GraphObject(imageID, startX, startY, startDir, size, depth)
//    {
//        this->world = world;
//        m_Alive = true;
//    }
//
//    // Action to perform each tick.
//    virtual void move() = 0;
//
//    StudentWorld* getWorld()
//    {
//        return world;
//    }
//
//    virtual ~Actor() {};
//
//    // Is this actor alive?
//    bool isAlive() const
//    {
//        return m_Alive;
//    }
//
//    // Mark this actor as dead.
//    void setDead()
//    {
//        m_Alive = false;
//    }
//
//    // Annoy this actor.
//    virtual bool annoy(unsigned int amt) = 0;
//
//    // Get this actor's world
//    StudentWorld* getWorld() const
//    {
//        return world;
//    }
//
//    // Can other actors pass through this actor?
//    virtual bool canActorsPassThroughMe() const
//    {
//        return false;
//    }
//
//    // Can this actor dig through Ice?
//    virtual bool canDigThroughIce() const
//    {
//        return false;
//    }
//
//    // Can this actor pick items up?
//    virtual bool canPickThingsUp() const
//    {
//        return false;
//    }
//
//    // Does this actor hunt the IceMan?
//    virtual bool huntsIceMan() const
//    {
//        return false;
//    }
//
//    // Can this actor need to be picked up to finish the level?
//    virtual bool needsToBePickedUpToFinishLevel() const
//    {
//        return false;
//    }
//
//    // Move this actor to x,y if possible, and return true; otherwise,
//    // return false without moving.
//    bool moveToIfPossible(int x, int y)
//    {
//        //make sure the x and y are within the screen
//        if (x > 60 || y > 60)
//        {
//            return false;
//        }
//        
//        ////make sure its not blocked
//        //if (getWorld()->isBlocked(x, y))
//        //    return false;
//
//
//        GraphObject::moveTo(x, y);
//        return true;
//    }
//};
//
//class Agent : public Actor
//{
//private:
//    unsigned int m_hitPoints;
//public:
//    Agent(StudentWorld* world, int startX, int startY, Direction startDir,
//        int imageID, unsigned int hitPoints, bool visible, double size, int depth)
//        : Actor(world, imageID, startX, startY, startDir, size, depth),
//        m_hitPoints(hitPoints)
//    {
//        setVisible(visible);
//    }
//
//    virtual void move() = 0;
//
//    // Pick up a gold nugget.
//    virtual void addGold() = 0;
//
//    // How many hit points does this actor have left?
//    unsigned int getHitPoints() const 
//    {
//        return m_hitPoints;
//    }
//
//    virtual bool annoy(unsigned int amount) override
//    {
//        if (m_hitPoints <= amount)
//        {
//            m_hitPoints = 0;
//            setDead();
//            return true;
//        }
//        else
//        {
//            m_hitPoints -= amount;
//            return false;
//        }
//    }
//
//    virtual bool canPickThingsUp() const override
//    {
//        return true;
//    }
//};
//
//class IceMan : public Agent
//{
//private:
//    unsigned int m_squirtAmount;
//    unsigned int m_sonarAmount;
//    unsigned int m_goldNuggets;
//    unsigned int m_hitPoints;
//public:
//    IceMan(StudentWorld* world, int startX, int startY)
//        : Agent(world, startX, startY, right, IID_PLAYER, 10, true, 1.0, 0),
//        m_squirtAmount(5), m_sonarAmount(1), m_goldNuggets(0)
//    {
//        setVisible(true);
//    }
//
//    virtual void move() override
//    {
//        if (!isAlive()) return;
//
//        int ch;
//        if(getWorld()->getKey(ch))
//            switch (ch)
//            {
//            case KEY_PRESS_ESCAPE:
//                setDead();
//                break;
//            case KEY_PRESS_SPACE:
//               // useSquirt();
//                m_squirtAmount--;
//                break;
//            case KEY_PRESS_LEFT:
//                if (getDirection() == left)
//                    moveToIfPossible(getX() - 1, getY());
//                else
//                    setDirection(left);
//                break;
//            case KEY_PRESS_RIGHT:
//                if (getDirection() == right)
//                    moveToIfPossible(getX() + 1, getY());
//                else
//                    setDirection(right);
//                break;
//            case KEY_PRESS_UP:
//                if (getDirection() == up)
//                    moveToIfPossible(getX(), getY() + 1);
//                else
//                    setDirection(up);
//                break;
//            case KEY_PRESS_DOWN:
//                if (getDirection() == down)
//                    moveToIfPossible(getX(), getY() - 1);
//                else
//                    setDirection(down);
//                break;
//            //case 'Z':
//            //case 'z':
//            //    if (m_sonarAmount > 0)
//            //    {
//            //        useSonar();
//            //        m_sonarAmount--;
//            //    }
//            //    break;
//            //case KEY_PRESS_TAB:
//            //    if (m_goldNuggets > 0)
//            //    {
//            //        dropGold();
//            //        m_goldNuggets--;
//            //    }
//            //    break;
//            }
//
//        getWorld()->clearIce(getX(), getY());
//    }
//
//    //void useSquirt()
//    //{
//    //    int x = getX();
//    //    int y = getY();
//    //    switch (getDirection())
//    //    {
//    //        case up: y += 4; break;
//    //        case down: y -= 4; break;
//    //        case left: x -= 4; break;
//    //        case right: x += 4; break;
//    //    }
//
//    //    if (!getWorld()->isBlocked(x, y))
//    //    {
//    //        getWorld()->addSquirt(x, y, getDirection());
//    //        getWorld()->playSound(SOUND_PLAYER_SQUIRT);
//
//    //    }
//    //    else
//    //    {
//    //        getWorld()->playSound(SOUND_PLAYER_SQUIRT);
//    //    }
//    //}
//
//    virtual bool annoy(unsigned int amount) override
//    {
//        if (m_hitPoints <= amount)
//        {
//            m_hitPoints = 0;
//            setDead();
//            getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
//            return true;
//        }
//        else
//        {
//            m_hitPoints -= amount;
//            return false;
//        }
//    }
//   /* 
//    void useSonar()
//    {
//        getWorld()->illuminateField(getX(), getY());
//    }
//    void dropGold()
//    {
//        getWorld()->addGold(getX(), getY(), true);
//        getWorld()->
//    }*/
//
//    virtual void addGold()
//    {
//        m_goldNuggets++;
//    }
//
//    virtual bool canDigThroughIce() const override
//    {
//        return true;
//    }
//
//    // Pick up a sonar kit.
//    void addSonar()
//    {
//        m_sonarAmount++;
//    }
//
//    // Pick up water.
//    void addWater()
//    {
//        m_squirtAmount++;
//    }
//
//    // Get amount of gold
//    unsigned int getGold() const
//    {
//        return m_goldNuggets;
//    }
//
//    // Get amount of sonar charges
//    unsigned int getSonar() const
//    {
//        return m_sonarAmount;
//    }
//
//    // Get amount of water
//    unsigned int getWater() const
//    {
//        return m_squirtAmount;
//    }
//};
//
////class Protester : public Agent
////{
////public:
////    Protester(StudentWorld* world, int startX, int startY, int imageID,
////        unsigned int hitPoints, Direction startDir, bool visible, int size, double depth)
////        : Agent(world, startX, startY, startDir, imageID, hitPoints, visible, size, depth)
////    {
////        setVisible(true);
////    }
////    virtual void move() {};
////    virtual bool annoy(unsigned int amount);
////    virtual void addGold();
////    virtual bool huntsIceMan() const;
////
////    // Set state to having gien up protest
////    void setMustLeaveOilField();
////
////    // Set number of ticks until next move
////    void setTicksToNextMove();
////};
////
////class RegularProtester : public Protester
////{
////public:
////    RegularProtester(StudentWorld* world, int startX, int startY, int imageID)
////        : Protester(world, startX, startY, IID_PROTESTER, 5,  )
////    virtual void move();
////    virtual void addGold();
////};
////
////class HardcoreProtester : public Protester
////{
////public:
////    HardcoreProtester(StudentWorld* world, int startX, int startY, int imageID);
////    virtual void move();
////    virtual void addGold();
////};
////
////
////class Ice : public Actor
////{
////public:
////    Ice(StudentWorld* world, int startX, int startY)
////        : Actor(world, IID_ICE, startX, startY, right, .25, 3)
////    {
////        setVisible(true);
////    }
////
////    virtual void move() {};
////};
////
////class Boulder : public Actor
////{
////private:
////    int m_state;
////    int m_waitTicks;
////
////    const int STATE_STABLE = 0;
////    const int STATE_WAITING = 1;
////    const int STATE_FALLING = 2;
////
////public:
////    Boulder(StudentWorld* world, int startX, int startY)
////        : Actor(world, IID_BOULDER, startX, startY, down, 1.0, 1)
////        , m_state(STATE_STABLE), m_waitTicks(0)
////    {
////        setVisible(true);
////    }
////
////    virtual void move() override
////    {
////        if (!isAlive()) return;
////
////        if (m_state == STATE_STABLE)
////        {
////            if (!isIceBelow())
////            {
////                m_state = STATE_WAITING;
////                m_waitTicks = 30; 
////            }
////        }
////        else if (m_state == STATE_WAITING)
////        {
////            if (m_waitTicks > 0)
////            {
////                m_waitTicks--;
////            }
////            else
////            {
////                m_state = STATE_FALLING;
////                getWorld()->playSound(SOUND_FALLING_ROCK);
////            }
////
////        }
////        else if (m_state = STATE_FALLING)
////        {
////            if (canFallTo(getX(), getY() - 1))
////            {
////                moveTo(getX(), getY() - 1);
////                
////                getWorld()->checkCollisionWithBoulder(this);
////            }
////            else
////            {
////                setDead();
////            }
////        }
////
////
////    }
////    virtual bool canActorsPassThroughMe() const override
////    {
////        return false;
////    }
////
////    bool isIceBelow()
////    {
////        int x = getX();
////        int y = getY() - 1;
////
////        for (int i = 0; i < 4; ++i)
////        {
////            if (getWorld()->isIceAt(x + i, y))
////            {
////                return true;
////            }
////        }
////        return false;
////    }
////
////    bool canFallTo(int x, int y) const
////    {
////        if (y < 0 || getWorld()->isIceAt(x, y))
////        {
////            return false;
////        }
////        return true;
////    }
////
////};
////
////class Squirt : public Actor
////{
////private:
////    int m_travelDis;
////public:
////    Squirt(StudentWorld* world, int startX, int startY, Direction startDir)
////        : Actor(world, IID_WATER_SPURT, startX, startY, startDir, 1.0, 1), m_travelDis(4)
////    {
////        setVisible(true);
////    }
////    virtual void move() override
////    {
////        if (!isAlive()) return;
////
////        //Check if it hit protestors
////        for (auto* protestor : getWorld()->getProtestors())
////        {
////            if (!protestor->isAlive() == false &&
////                getWorld()->withinRadius(getX(), getY(), protestor->getX(), protestor->getY(), 3))
////            {
////                protestor->annoy(2);
////                setDead();
////                return;
////            }
////
////            int nextX = getX();
////            int nextY = getY();
////
////            switch (getDirection())
////            {
////                case left:  nextX--; break;
////                case right: nextX++; break;
////                case up:    nextY++; break;
////                case down:  nextY--; break;
////                default:    break;
////            }
////
////            //check if it hit ice or boulders
////            if (getWorld()->isIceAt(nextX, nextY) || getWorld()->isBoulderAt(nextX, nextY))
////            {
////                setDead();
////                return;
////            }
////
////            int nextX = getX();
////            int nextY = getY();
////            
////            switch (getDirection()) 
////            {
////                case left:  nextX--; break;
////                case right: nextX++; break;
////                case up:    nextY++; break;
////                case down:  nextY--; break;
////                default:    break;
////            }
////
////            if (m_travelDis == 0)
////            {
////                setDead();
////                return;
////            }
////
////            moveTo(nextX, nextY);
////            m_travelDis--;
////        }
////
////    }
////};
////
////class ActivatingObject : public Actor
////{
////public:
////    ActivatingObject(StudentWorld* world, int startX, int startY, int imageID,
////        int soundToPlay, bool activateOnPlayer,
////        bool activateOnProtester, bool initiallyActive, Direction startdir, double size, int depth)
////        : Actor(world, imageID, startX, startY, startdir, size, depth)
////    {}
////    virtual void move() {};
////
////    // Set number of ticks until this object dies
////    void setTicksToLive();
////};
////
////class OilBarrel : public ActivatingObject
////{
////public:
////    OilBarrel(StudentWorld* world, int startX, int startY)
////        : ActivatingObject(world, startX, startY, IID_BARREL, SOUND_FOUND_OIL, true, false,
////            false, right, 1.0, 2)
////    {}
////    virtual void move() override
////    {
////        if (!isAlive()) return;
////
////        //making it visible
////        if (!isVisible() && getWorld()->withinRadius(getX(), getY(), getWorld()->getIceman()->getX(), getWorld()->getIceman()->getY(), 4))
////        {
////            setVisible(true);
////            return;
////        }
////
////        // If the Barrel is within a radius of 3.0 from the Iceman
////        if (getWorld()->isNearIceMan(this, 3));
////        {
////            setDead();
////            getWorld()->playSound(SOUND_FOUND_OIL);
////            getWorld()->increaseScore(1000);
////            getWorld()->pickUpOilBarrel();
////        }
////
////    }
////    virtual bool needsToBePickedUpToFinishLevel() const override
////    {
////        return true;
////    }
////};
////
////class GoldNugget : public ActivatingObject
////{
////private:
////    int m_isPickupableByIceman;
////    int m_isTemp;
////    int m_life;
////public:
////    GoldNugget(StudentWorld* world, int startX, int startY, bool isVisible, bool isPickupableByIceman, bool isTemp)
////        : ActivatingObject(world, startX, startY, IID_GOLD, SOUND_GOT_GOODIE, true, true, false, right, 1.0, 2)
////        , m_isPickupableByIceman(isPickupableByIceman), m_isTemp(isTemp), m_life(100)
////    {
////        setVisible(isVisible);
////    }
////    virtual void move() override
////    {
////        if (!isAlive()) return;
////
////        if (!isVisible() && getWorld()->isNearIceman(this, 4))
////        {
////            setVisible(true);
////            return;
////        }
////        else if (m_isPickupableByIceman && getWorld()->isNearIceman(this, 3))
////        {
////            setDead();
////            getWorld()->playSound(SOUND_GOT_GOODIE);
////            getWorld()->increaseScore(10);
////            getWorld()->getIceman()->addGold();
////            return;
////        }
////        else if (!m_isPickupableByIceman)
////        {
////            for (auto* protestor : getWorld()->getProtestors())
////            {
////                if (protestor->isAlive() && getWorld()->isNearIceMan(this, 3))
////                {
////                    setDead();
////                    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
////                    protestor->addGold();
////                    getWorld()->increaseScore(25);
////                    return;
////                }
////            }
////        }
////
////        if (m_isTemp)
////        {
////            if (m_life <= 0)
////                setDead();
////            else
////                m_life--;
////        }
////    }
////};
////
////class SonarKit : public ActivatingObject
////{
////private:
////    int m_life;
////public:
////    SonarKit(StudentWorld* world, int startX, int startY, int level)
////        : ActivatingObject(world, startX, startY, IID_SONAR, SOUND_GOT_GOODIE, true, false, true, right, 1.0, 2),
////        m_life(std::max(100,300 - 10 * level))
////    {
////        setVisible(true);
////    }
////
////    virtual void move() override
////    {
////        if (!isAlive()) return;
////
////        if (getWorld()->isNearIceman(this, 3))
////        {
////            setDead();
////            getWorld()->playSound(SOUND_GOT_GOODIE);
////            getWorld()->giveIceManSonar();
////            getWorld()->increaseScore(75);
////            return;
////        }
////
////        if (m_life <= 0)
////        {
////            setDead();
////        }
////        else
////        {
////            m_life--;
////        }
////    }
////};
////
////
////class WaterPool : public ActivatingObject
////{
////private:
////    int m_life;
////public:
////    WaterPool(StudentWorld* world, int startX, int startY, int level)
////        :ActivatingObject(world, startX, startY, IID_WATER_POOL, SOUND_GOT_GOODIE, true, false, true, right, 1.0, 2),
////        m_life(std::max(100, 300 - 10 * level))
////    {
////        setVisible(true);
////    }
////    virtual void move() override
////    {
////        if (!isAlive()) return;
////
////        if (getWorld()->isNearIceMan(this, 3))
////        {
////            setDead();
////            getWorld()->playSound(SOUND_GOT_GOODIE);
////            getWorld()->giveIceManWater();
////            getWorld()->increaseScore(100);
////            return;
////        }
////
////        if (m_life <= 0)
////            setDead();
////        else
////            m_life--;
////    }
////};
//
//#endif // ACTOR_H_