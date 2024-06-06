#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Actor class implementation
Actor::Actor(int imageID, int startX, int startY, Direction dir, float size, unsigned int depth, StudentWorld* world)
    : GraphObject(imageID, startX, startY, dir, size, depth), m_alive(true), m_world(world) {
    setVisible(true);
}

Actor::~Actor() {}

bool Actor::isAlive() const {
    return m_alive;
}

void Actor::setDead() {
    m_alive = false;
}

StudentWorld* Actor::getWorld() const {
    return m_world;
}

// Ice class implementation
Ice::Ice(int startX, int startY, StudentWorld* world)
    : Actor(IID_ICE, startX, startY, right, 0.25, 3, world) {}

Ice::~Ice() {}

void Ice::doSomething() {
    // Ice does nothing
}

Iceman::Iceman(StudentWorld* world)
    : Actor(IID_PLAYER, 30, 60, right, 1.0, 0, world), m_hitPoints(10), m_water(5), m_gold(0), m_sonar(1) {}

Iceman::~Iceman() {}

void Iceman::doSomething() {
    // 1. Check if the Iceman is alive
    if (!isAlive()) return;

    // 2. Remove ice in the 4x4 area occupied by the Iceman
    bool iceRemoved = false;
    for (int x = getX(); x < getX() + 4; x++) {
        for (int y = getY(); y < getY() + 4; y++) {
            if (getWorld()->removeIce(x, y)) {
                iceRemoved = true;
            }
        }
    }
    if (iceRemoved) {
        getWorld()->playSound(SOUND_DIG);
    }

    // 3. Check for key presses
    int ch;
    if (getWorld()->getKey(ch)) {
        switch (ch) {
            // a. Handle escape key
        case KEY_PRESS_ESCAPE:
            //getWorld()->playSound(KEY_PRESS_ESCAPE);
            setDead();
            return;

            // b. Handle space bar to fire squirt
        case KEY_PRESS_SPACE:
            if (m_water > 0) {
                m_water--;
                int x = getX(), y = getY();
                Direction dir = getDirection();
                switch (dir) {
                case left:
                    x -= 1;
                    break;
                case right:
                    x += 1;
                    break;
                case up:
                    y += 1;
                    break;
                case down:
                    y -= 1;
                    break;
                }
                if (!getWorld()->isBlocked(x, y) && !getWorld()->isIceAt(x, y) && !getWorld()->isBoulderAt(x, y, 3.0)) {
                    getWorld()->addActor(new Squirt(x, y, dir, getWorld()));
                }
                getWorld()->playSound(SOUND_PLAYER_SQUIRT);
            }
            break;

            // c. Handle turning directions
        case KEY_PRESS_LEFT:
            if (getDirection() != left) {
                setDirection(left);
            }
            else if (getX() > 0 && !getWorld()->isBlocked(getX() - 1, getY()) && !getWorld()->isBoulderAt(getX() - 1, getY(), 3.0)) {
                moveTo(getX() - 1, getY());
            }
            break;
        case KEY_PRESS_RIGHT:
            if (getDirection() != right) {
                setDirection(right);
            }
            else if (getX() < 60 && !getWorld()->isBlocked(getX() + 1, getY()) && !getWorld()->isBoulderAt(getX() + 1, getY(), 3.0)) {
                moveTo(getX() + 1, getY());
            }
            break;
        case KEY_PRESS_UP:
            if (getDirection() != up) {
                setDirection(up);
            }
            else if (getY() < 60 && !getWorld()->isBlocked(getX(), getY() + 1) && !getWorld()->isBoulderAt(getX(), getY() + 1, 3.0)) {
                moveTo(getX(), getY() + 1);
            }
            break;
        case KEY_PRESS_DOWN:
            if (getDirection() != down) {
                setDirection(down);
            }
            else if (getY() > 0 && !getWorld()->isBlocked(getX(), getY() - 1) && !getWorld()->isBoulderAt(getX(), getY() - 1, 3.0)) {
                moveTo(getX(), getY() - 1);
            }
            break;

            // e. Handle sonar charge
        case 'z':
        case 'Z':
            if (m_sonar > 0) {
                m_sonar--;
                getWorld()->revealObjects(getX(), getY(), 12);
            }
            break;

            // f. Handle dropping gold nugget
        case KEY_PRESS_TAB:
            dropGold();

        default:
            break;
        }
    }
}

int Iceman::getWater() { return m_water; }
int Iceman::getSonarCharge() { return m_sonar; }
int Iceman::getGoldNugget() { return m_gold; }
int Iceman::getOil() { return m_oilLeft; }
int Iceman::getHP() { return m_hitPoints; }
void Iceman::setGoldNugget(int val) { m_gold = val; }

void Iceman::dropGold()
{
    if (getWorld()->getIceman()->getGoldNugget() > 0)
    {
        getWorld()->dropGold();
        getWorld()->getIceman()->setGoldNugget(getWorld()->getIceman()->getGoldNugget() - 1);
    }
}

// Boulder class implementation
Boulder::Boulder(int startX, int startY, StudentWorld* world)
    : Actor(IID_BOULDER, startX, startY, down, 1.0, 1, world), m_state(STABLE), m_waitingTicks(30) {}

Boulder::~Boulder() {}

void Boulder::doSomething() {
    if (!isAlive()) return;

    switch (m_state) {
    case STABLE:
        if (!getWorld()->isIceBelow(getX(), getY())) {
            m_state = WAITING;
        }
        break;
    case WAITING:
        if (m_waitingTicks > 0) {
            m_waitingTicks--;
        }
        else {
            m_state = FALLING;
            getWorld()->playSound(SOUND_FALLING_ROCK);
        }
        break;
    case FALLING:
        if (getY() > 0 && !getWorld()->isBlocked(getX(), getY() - 1)) {
            moveTo(getX(), getY() - 1);
            if (getWorld()->annoyIcemanOrProtester(this)) {
                setDead();
            }
        }
        else {
            setDead();
        }
        break;
    }
}

// Squirt class implementation
Squirt::Squirt(int startX, int startY, Direction dir, StudentWorld* world)
    : Actor(IID_WATER_SPURT, startX, startY, dir, 1.0, 1, world), m_distanceTraveled(0) {}

Squirt::~Squirt() {}

void Squirt::doSomething() {
    if (!isAlive()) return;

    if (m_distanceTraveled >= 4) {
        setDead();
    }
    else {
        int x = getX(), y = getY();
        switch (getDirection()) {
        case left:
            if (!getWorld()->isBlocked(x - 1, y)) {
                moveTo(x - 1, y);
            }
            else {
                setDead();
            }
            break;
        case right:
            if (!getWorld()->isBlocked(x + 1, y)) {
                moveTo(x + 1, y);
            }
            else {
                setDead();
            }
            break;
        case up:
            if (!getWorld()->isBlocked(x, y + 1)) {
                moveTo(x, y + 1);
            }
            else {
                setDead();
            }
            break;
        case down:
            if (!getWorld()->isBlocked(x, y - 1)) {
                moveTo(x, y - 1);
            }
            else {
                setDead();
            }
            break;
        }
        m_distanceTraveled++;
        if (getWorld()->annoyProtesterAt(getX(), getY(), 2)) {
            setDead();
        }
    }
}


//============================================CHANGED=====================================================
// GoldNugget class implementation
GoldNugget::GoldNugget(int startX, int startY, bool temporary, StudentWorld* world)
    : Actor(IID_GOLD, startX, startY, right, 1.0, 2, world), m_temporary(temporary), m_ticksLeft(100), m_visible(false) {
    setVisible(temporary); // Initially invisible
}

GoldNugget::~GoldNugget() {}

void GoldNugget::setVisible(bool visible) {
    m_visible = visible;
    Actor::setVisible(visible); // Call the base class function to set visibility
}


void GoldNugget::doSomething() {
    if (!isAlive()) return;

    if (m_isPickupable) {
        if (!isVisible() && getWorld()->calculateDistance(getX(), getY(), getWorld()->getIcemanX(), getWorld()->getIcemanY()) <= 5) {
            setVisible(true);
            return;
        }

        if (isVisible() && getWorld()->calculateDistance(getX(), getY(), getWorld()->getIcemanX(), getWorld()->getIcemanY()) <= 2) {
            setDead();
            getWorld()->playSound(SOUND_GOT_GOODIE);
            getWorld()->increaseScore(10); // Adjust score as needed
            getWorld()->getIceman()->setGoldNugget(getWorld()->getIceman()->getGoldNugget() + 1);
            return;
        }
    }
    else {
        if (m_ticksLeft == 0) {
            setDead();
            return;
        }

        //if (getWorld()->anyProtesterPickUpGold(this)) {
        //    Protester* protester = getWorld()->anyProtesterPickUpGold(this);
        //    setDead();
        //    protester->getBribed(); // Assuming Protester has a method to handle getting bribed
        //    return;
        //}

        --m_ticksLeft;
    }
}


//========================================================================================================



// Protester class implementation
Protester::Protester(int imageID, int startX, int startY, StudentWorld* world, int hitPoints)
    : Actor(imageID, startX, startY, left, 1.0, 0, world), m_hitPoints(hitPoints), m_restingTicks(0), m_shoutCooldown(0), m_numSquaresToMoveInCurrentDirection(0), m_leaveOilField(false) {}

Protester::~Protester() {}

void Protester::getAnnoyed(int points) {
    m_hitPoints -= points;
    if (m_hitPoints <= 0) {
        setDead();
        m_leaveOilField = true;
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        getWorld()->increaseScore(100);
    }
    else {
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    }
}

void Protester::moveToNextPosition() {
    if (m_leaveOilField) {
        moveToExit();
        return;
    }

    if (m_numSquaresToMoveInCurrentDirection <= 0) {
        pickNewDirection();
    }
    else {
        Direction dir = getDirection();
        int x = getX(), y = getY();
        switch (dir) {
        case left:
            if (canMoveInDirection(left)) {
                moveTo(x - 1, y);
                m_numSquaresToMoveInCurrentDirection--;
            }
            else {
                m_numSquaresToMoveInCurrentDirection = 0;
            }
            break;
        case right:
            if (canMoveInDirection(right)) {
                moveTo(x + 1, y);
                m_numSquaresToMoveInCurrentDirection--;
            }
            else {
                m_numSquaresToMoveInCurrentDirection = 0;
            }
            break;
        case up:
            if (canMoveInDirection(up)) {
                moveTo(x, y + 1);
                m_numSquaresToMoveInCurrentDirection--;
            }
            else {
                m_numSquaresToMoveInCurrentDirection = 0;
            }
            break;
        case down:
            if (canMoveInDirection(down)) {
                moveTo(x, y - 1);
                m_numSquaresToMoveInCurrentDirection--;
            }
            else {
                m_numSquaresToMoveInCurrentDirection = 0;
            }
            break;
        }
    }
}

void Protester::setRestingTicks(int ticks) {
    m_restingTicks = ticks;
}

bool Protester::isResting() const {
    return m_restingTicks > 0;
}

void Protester::shoutIfCloseToIceman() {
    if (m_shoutCooldown > 0) {
        m_shoutCooldown--;
        return;
    }

    // Logic to check if Iceman is within 4 units and in the same direction
    // If so, shout and reset shout cooldown
    int icemanX = getWorld()->getIcemanX();
    int icemanY = getWorld()->getIcemanY();
    int x = getX(), y = getY();
    if ((getDirection() == left && x - icemanX <= 4 && y == icemanY) ||
        (getDirection() == right && icemanX - x <= 4 && y == icemanY) ||
        (getDirection() == up && icemanY - y <= 4 && x == icemanX) ||
        (getDirection() == down && y - icemanY <= 4 && x == icemanX)) {
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        m_shoutCooldown = 15;
    }
}

bool Protester::canMoveInDirection(Direction dir) const {
    int x = getX(), y = getY();
    switch (dir) {
    case left:
        return x > 0 && !getWorld()->isBlocked(x - 1, y);
    case right:
        return x < 60 && !getWorld()->isBlocked(x + 1, y);
    case up:
        return y < 60 && !getWorld()->isBlocked(x, y + 1);
    case down:
        return y > 0 && !getWorld()->isBlocked(x, y - 1);
    default:
        return false;
    }
}

void Protester::pickNewDirection() {
    // Logic to pick a new random direction and set m_numSquaresToMoveInCurrentDirection
    m_numSquaresToMoveInCurrentDirection = 8 + rand() % 53;
    Direction dir;
    do {
        dir = static_cast<Direction>(rand() % 4);
    } while (!canMoveInDirection(dir));
    setDirection(dir);
}

void Protester::moveToExit() {
    // Logic to move towards the exit
    // If the protester reaches the exit, set it as dead
}

// RegularProtester class implementation
RegularProtester::RegularProtester(int startX, int startY, StudentWorld* world)
    : Protester(IID_PROTESTER, startX, startY, world, 5) {}

RegularProtester::~RegularProtester() {}

void RegularProtester::doSomething() {
    if (!isAlive()) return;

    if (isResting()) {
        setRestingTicks(m_restingTicks - 1);
        return;
    }

    moveToNextPosition();
    shoutIfCloseToIceman();
    setRestingTicks(3);
}

// HardcoreProtester class implementation
HardcoreProtester::HardcoreProtester(int startX, int startY, StudentWorld* world)
    : Protester(IID_HARD_CORE_PROTESTER, startX, startY, world, 20) {}

HardcoreProtester::~HardcoreProtester() {}

void HardcoreProtester::doSomething() {
    if (!isAlive()) return;

    if (isResting()) {
        setRestingTicks(m_restingTicks - 1);
        return;
    }

    moveToNextPosition();
    shoutIfCloseToIceman();
    setRestingTicks(3);
    // Additional logic for hardcore protester behavior
}

// SonarKit class implementation
SonarKit::SonarKit(int startX, int startY, StudentWorld* world)
    : Actor(IID_SONAR, startX, startY, right, 1.0, 5, world) {}

SonarKit::~SonarKit() {}

void SonarKit::doSomething() {
    if (!isAlive()) return;

    // Logic to check if Iceman picks up Sonar Kit
    if (getWorld()->getIcemanX() == getX() && getWorld()->getIcemanY() == getY()) {
        getWorld()->increaseScore(75);
        setDead();
    }
}

// WaterPool class implementation
WaterPool::WaterPool(int startX, int startY, StudentWorld* world)
    : Actor(IID_WATER_POOL, startX, startY, right, 1.0, 2, world), m_ticksLeft(100) {}

WaterPool::~WaterPool() {}

void WaterPool::doSomething() {
    if (!isAlive()) return;

    if (m_ticksLeft > 0) {
        m_ticksLeft--;
    }
    else {
        setDead();
    }
    // Logic to check if Iceman picks up Water Pool
    if (getWorld()->getIcemanX() == getX() && getWorld()->getIcemanY() == getY()) {
        getWorld()->increaseScore(100);
        setDead();
    }
}


//=====================================================================================


// Barrel class implementation
Barrel::Barrel(int startX, int startY, StudentWorld* world)
    : Actor(IID_BARREL, startX, startY, right, 1.0, 2, world) {
    setVisible(false); // Initially invisible
}

Barrel::~Barrel() {}

void Barrel::doSomething() {
    if (!isAlive()) return;

    // Check if the Iceman is within 5 radius distance
    int icemanX = getWorld()->getIcemanX();
    int icemanY = getWorld()->getIcemanY();
    int distance = getWorld()->calculateDistance(getX(), getY(), icemanX, icemanY);

    if (!isVisible() && distance <= 5) {
        setVisible(true); // Make the barrel visible
    }

    // Logic to handle barrel being picked up
    if (isVisible() && icemanX == getX() && icemanY == getY()) {
        setDead(); // Barrel picked up, set it as dead
        getWorld()->increaseScore(1000); // Increase score for picking up barrel
        getWorld()->playSound(SOUND_FOUND_OIL); // Play sound effect
    }
}

//============================================================================================
