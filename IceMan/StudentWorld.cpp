#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <algorithm>
#include <string>
#include <iomanip>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(std::string assetDir)
    : GameWorld(assetDir), m_iceman(nullptr) {
    // Initialize the ice field
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 64; y++) {
            m_iceField[x][y] = nullptr;
        }
    }
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init()
{
    m_iceman = new Iceman(this);

    // Initialize Ice objects and add to the ice field array
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 60; y++) {
            if (x < 30 || x > 33 || y < 4) {
                m_iceField[x][y] = new Ice(x, y, this);
            }
        }
    }

    // Add initial Boulders, Gold Nuggets, and other actors
    addActors();

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    updateDisplayText();

    m_iceman->doSomething();
    for (auto actor : m_actors) {
        if (actor->isAlive()) {
            actor->doSomething();
        }
    }

    removeDeadGameObjects();

    if (!m_iceman->isAlive()) return GWSTATUS_PLAYER_DIED;

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() 
{
    delete m_iceman;
    m_iceman = nullptr;

    for (auto actor : m_actors) {
        delete actor;
    }
    m_actors.clear();

    // Clean up the ice field
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 64; y++) {
            delete m_iceField[x][y];
            m_iceField[x][y] = nullptr;
        }
    }
}


void StudentWorld::updateDisplayText() {
    // Update game status line
}

void StudentWorld::removeDeadGameObjects() {
    m_actors.erase(std::remove_if(m_actors.begin(), m_actors.end(), [](Actor* actor) {
        if (!actor->isAlive()) {
            delete actor;
            return true;
        }
        return false;
        }), m_actors.end());
}

void StudentWorld::playFinishedLevelSound() {
    // Play sound when the level is finished
}

void StudentWorld::addActors() {
    // Add Boulders
    int B = std::min((int)getLevel() / 2 + 2, 9);
    for (int i = 0; i < B; i++) {
        int x = rand() % 60;
        int y = 20 + rand() % 36;
        m_actors.push_back(new Boulder(x, y, this));
    }

    // Add Gold Nuggets
    int G = std::max(5 - (int)getLevel() / 2, 2);
    for (int i = 0; i < G; i++) {
        int x = rand() % 61;
        int y = rand() % 57;
        m_actors.push_back(new GoldNugget(x, y, false, this));
    }

    // Add Regular Protesters
    int P = std::min(15, int(2 + getLevel() * 1.5));
    for (int i = 0; i < P; i++) {
        if (rand() % 100 < (std::min(90, (int)getLevel() * 10 + 30))) {
            m_actors.push_back(new HardcoreProtester(60, 60, this));
        }
        else {
            m_actors.push_back(new RegularProtester(60, 60, this));
        }
    }

    // Add Sonar Kits
    int S = std::max(1, (int)getLevel() / 2);
    for (int i = 0; i < S; i++) {
        int x = rand() % 60;
        int y = rand() % 60;
        m_actors.push_back(new SonarKit(x, y, this));
    }

    // Add Water Pools
    int W = std::max(1, (int)getLevel() / 2);
    for (int i = 0; i < W; i++) {
        int x = rand() % 60;
        int y = rand() % 60;
        m_actors.push_back(new WaterPool(x, y, this));
    }

    // Add other actors similarly...
}

void StudentWorld::addActor(Actor* actor) {
    m_actors.push_back(actor);
}

void StudentWorld::revealObjects(int x, int y, int radius) {
    for (auto actor : m_actors) {
        int dx = actor->getX() - x;
        int dy = actor->getY() - y;
        if (dx * dx + dy * dy <= radius * radius) {
            actor->setVisible(true);
        }
    }
}

void StudentWorld::increaseScore(int points) {
    GameWorld::increaseScore(points);
}

bool StudentWorld::isBlocked(int x, int y) const {
    for (const auto& actor : m_actors) {
        if (actor->getX() == x && actor->getY() == y && dynamic_cast<Boulder*>(actor)) {
            return true;
        }
    }
    return false;
}

bool StudentWorld::isIceBelow(int x, int y) const {
    if (y > 0 && m_iceField[x][y - 1] != nullptr) {
        return true;
    }
    return false;
}

bool StudentWorld::annoyIcemanOrProtester(Actor* a) {
    if (m_iceman->getX() == a->getX() && m_iceman->getY() == a->getY()) {
        m_iceman->setDead();
        return true;
    }
    for (auto& actor : m_actors) {
        if (actor->getX() == a->getX() && actor->getY() == a->getY() && dynamic_cast<Protester*>(actor)) {
            dynamic_cast<Protester*>(actor)->getAnnoyed(100);
            return true;
        }
    }
    return false;
}

bool StudentWorld::annoyProtesterAt(int x, int y, int points) {
    for (auto& actor : m_actors) {
        if (actor->getX() == x && actor->getY() == y && dynamic_cast<Protester*>(actor)) {
            dynamic_cast<Protester*>(actor)->getAnnoyed(points);
            return true;
        }
    }
    return false;
}

int StudentWorld::getIcemanX() const {
    return m_iceman->getX();
}

int StudentWorld::getIcemanY() const {
    return m_iceman->getY();
}

bool StudentWorld::removeIce(int x, int y)
{
    if (x < 0 || x >= 64 || y < 0 || y >= 60) {
        return false;
    }
    if (m_iceField[x][y] != nullptr) {
        delete m_iceField[x][y];
        m_iceField[x][y] = nullptr;
        return true;
    }
    return false;
}

bool StudentWorld::isIceAt(int x, int y) const {
    if (x < 0 || x >= 64 || y < 0 || y >= 60) {
        return false;
    }
    return m_iceField[x][y] != nullptr;
}

bool StudentWorld::isBoulderAt(int x, int y, double radius) const {
    for (const auto& actor : m_actors) {
        if (Boulder* boulder = dynamic_cast<Boulder*>(actor)) {
            int boulderX = boulder->getX();
            int boulderY = boulder->getY();
            double distance = std::sqrt(std::pow(boulderX - x, 2) + std::pow(boulderY - y, 2));
            if (distance <= radius) {
                return true;
            }
        }
    }
    return false;
}