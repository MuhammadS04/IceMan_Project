#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GraphObject.h"
#include "GameWorld.h"
#include <string>
#include <vector>
//changes 06/06
class Actor;
class Iceman;
class Boulder;
class Ice;
class Squirt;
class Protestor;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();

    virtual int init();
    virtual int move();
    virtual void cleanUp();

    // Add an actor to the world.
    void addActor(Actor* a);

    // Clear a 4x4 region of Ice.
    void clearIce(int x, int y);

    // Can actor move to x,y?
    bool canActorMoveTo(Actor* a, int x, int y) const;

    // Annoy all other actors within radius of annoyer, returning the
    // number of actors annoyed.
    int annoyAllNearbyActors(Actor* annoyer, int points, int radius);

    // Reveal all objects within radius of x,y.
    void revealAllNearbyObjects(int x, int y, int radius);

    // If the IceMan is within radius of a, return a pointer to the
    // IceMan, otherwise null.
    Actor* findNearbyIceMan(Actor* a, int radius) const;

    // If at least one actor that can pick things up is within radius of a,
    // return a pointer to one of them, otherwise null.
    Actor* findNearbyPickerUpper(Actor* a, int radius) const;

    // Annoy the IceMan.
    void annoyIceMan();

    // Give IceMan some sonar charges.
    void giveIceManSonar();

    // Give IceMan some water.
    void giveIceManWater();

    // Is the Actor a facing toward the IceMan?
    bool facingTowardIceMan(Actor* a) const;

    // If the Actor a has a clear line of sight to the IceMan, return
    // the direction to the IceMan, otherwise GraphObject::none.
    GraphObject::Direction lineOfSightToIceMan(Actor* a) const;

    // Return whether the Actor a is within radius of IceMan.
    bool isNearIceMan(Actor* a, int radius) const;

    // Determine the direction of the first move a quitting protester
    // makes to leave the oil field.
    GraphObject::Direction determineFirstMoveToExit(int x, int y);

    // Determine the direction of the first move a hardcore protester
    // makes to approach the IceMan.
    GraphObject::Direction determineFirstMoveToIceMan(int x, int y);

    bool withinRadius(int x1, int y1, int x2, int y2, int radius) const;
    void checkCollissionWithBoulder(Boulder* boulder);
    Iceman* getIceman() const;
    //   vector<Protestor*> getProtestors() const;
    bool isIceAt(int x, int y);
    bool isBoulderAt(int x, int y) const;

};

#endif // STUDENTWORLD_H_