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
    int getWater();
    int getSonarCharge();
    int getGoldNugget();
    int getOil();
    int getHP();


private:
    int m_hitPoints;
    int m_water;
    int m_gold;
    int m_sonar;
    int m_oilLeft;
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

//====================================CHANGED=========================================
class GoldNugget : public Actor {
public:
    GoldNugget(int startX, int startY, bool temporary, StudentWorld* world);
    virtual ~GoldNugget();
    virtual void doSomething() override;
    void setVisible(bool visible);

private:
    bool m_temporary;
    int m_ticksLeft;
    bool m_visible;
};
//=====================================================================================

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

//========================================CHANGED=============================================

class Barrel : public Actor {
public:
    Barrel(int startX, int startY, StudentWorld* world);
    virtual ~Barrel();
    virtual void doSomething() override;

private:
    // Add any necessary private members
};

//============================================================================================


#endif // ACTOR_H