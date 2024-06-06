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
#include <iomanip>
#include <sstream>

void StudentWorld::updateDisplayText()
{
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int health = m_iceman->getHP();
    int squirts = m_iceman->getWater();
    int gold = m_iceman->getGoldNugget();
    int sonar = m_iceman->getSonarCharge();
    int barrels = m_iceman->getOil();

    stringstream s;

    s.fill('0');
    s << "Scr: ";
    s << setw(6) << score;

    s.fill(' ');
    s << " Lvl: ";
    s << setw(2) << level;

    s << " Lives: ";
    s << setw(1) << lives;

    s << "  Hlth: ";
    s << setw(3) << health * 10;
    s << '%';

    s << "  Wtr: ";
    s << setw(2) << squirts;

    s << "  Gld: ";
    s << setw(2) << gold;

    s << "  Sonar: ";
    s << setw(2) << sonar;

    s << "  Oil Left: ";
    s << setw(2) << barrels;

    setGameStatText(s.str());

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

    // Add Barrels
    int H = std::min((int)getLevel() / 2 + 2, 9);
    for (int i = 0; i < B; i++) {
        int x = rand() % 60;
        int y = rand() % 56;
        m_actors.push_back(new Barrel(x, y, this));
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

//========================================CHANGED=============================================

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

bool StudentWorld::isNearIceman(int x, int y, double radius) const {
    // Calculate the distance between (x, y) and the Iceman's position
    int icemanX = m_iceman->getX();
    int icemanY = m_iceman->getY();
    double distance = sqrt(pow(icemanX - x, 2) + pow(icemanY - y, 2));

    // Check if the distance is less than or equal to the specified radius
    return distance <= radius;
}

//============================================================================================


double StudentWorld::calculateDistance(int x1, int y1, int x2, int y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}