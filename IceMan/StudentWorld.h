#ifndef STUDENTWORLD_H
#define STUDENTWORLD_H

#include "GameWorld.h"
#include <vector>
#include "Actor.h";

class StudentWorld : public GameWorld {
public:
    StudentWorld(std::string assetDir);
       

    virtual ~StudentWorld();

    virtual int init();
    virtual int move();
    virtual void cleanUp();

    void addActor(Actor* actor);
    void revealObjects(int x, int y, int radius);
    void increaseScore(int points);
    bool isBlocked(int x, int y) const;
    bool isIceBelow(int x, int y) const;
    bool annoyIcemanOrProtester(Actor* a);
    bool annoyProtesterAt(int x, int y, int points);
    int getIcemanX() const;
    int getIcemanY() const;
    bool removeIce(int x, int y);
    bool isIceAt(int x, int y) const;
    bool isBoulderAt(int x, int y, double radius) const;

private:
    Iceman* m_iceman;
    Ice* m_iceField[64][64]; // Ice field array
    std::vector<Actor*> m_actors;

    void updateDisplayText();
    void removeDeadGameObjects();
    void playFinishedLevelSound();
    void addActors();
};

#endif // STUDENTWORLD_H

