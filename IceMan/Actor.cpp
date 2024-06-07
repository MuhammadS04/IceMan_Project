#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Actor class 
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

// Ice class 
Ice::Ice(int startX, int startY, StudentWorld* world)
    : Actor(IID_ICE, startX, startY, right, 0.25, 3, world) {}

Ice::~Ice() {}

void Ice::doSomething() {
    // Ice does 
}

Iceman::Iceman(StudentWorld* world)
    : Actor(IID_PLAYER, 30, 60, right, 1.0, 0, world), m_hitPoints(10), m_water(5), m_gold(0), m_sonar(1) {}

Iceman::~Iceman() {}

void Iceman::doSomething() {
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
                getWorld()->revealObjects(getX(), getY(), 12);  // reveal objects within a radius of 12 units
                getWorld()->playSound(SOUND_PLAYER_GIVE_UP);

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
void Iceman::increaseSonarKit() {
    m_sonar++;
}
void Iceman::increaseWater(int amount) {
    m_water += amount;
}



void Iceman::dropGold()
{
    if (getWorld()->getIceman()->getGoldNugget() > 0)
    {
        getWorld()->dropGold();
        getWorld()->getIceman()->setGoldNugget(getWorld()->getIceman()->getGoldNugget() - 1);

    }
}

// Boulder class 
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

            // Annoy Protester or Iceman within a radius of 3.0 units
            for (auto actor : getWorld()->getActors()) {
                if (Protester* protester = dynamic_cast<Protester*>(actor)) {
                    if (getWorld()->calculateDistance(getX(), getY(), protester->getX(), protester->getY()) <= 3.0) {
                        protester->getAnnoyed(100);  // Annoy Protester with 100 points
                    }
                }
            }

            // Check if Iceman is within a radius of 3.0 units
            if (getWorld()->calculateDistance(getX(), getY(), getWorld()->getIceman()->getX(), getWorld()->getIceman()->getY()) <= 3.0) {
                getWorld()->getIceman()->setDead();  // Set Iceman to dead
            }

            // Check if Boulder should be set to dead
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

// Squirt class 
Squirt::Squirt(int startX, int startY, Direction dir, StudentWorld* world)
    : Actor(IID_WATER_SPURT, startX, startY, dir, 1.0, 1, world), m_distanceTraveled(0)
{
    setVisible(true);
}
Squirt::~Squirt() {}

void Squirt::doSomething() {
    if (!isAlive()) return;

    // Check if Squirt is within radius of 3.0 units from any Protester
    for (auto actor : getWorld()->getActors()) {
        Protester* protester = dynamic_cast<Protester*>(actor);
        if (protester && getWorld()->calculateDistance(getX(), getY(), protester->getX(), protester->getY()) <= 3.0) {
            protester->getAnnoyed(2); // Annoy Protester with 2 points
            setDead();
            return;
        }
    }

    // Check if Squirt has traveled its full distance
    if (m_distanceTraveled >= 4) {
        setDead();
        return;
    }

    // Determine next position and check for obstacles
    int x = getX(), y = getY();
    switch (getDirection()) {
    case left:
        if (!getWorld()->isIceAt(x - 1, y) && !getWorld()->isBoulderAt(x - 1, y, 0.5)) {
            moveTo(x - 4, y);
        }
        else {
            setDead();
        }
        break;
    case right:
        if (!getWorld()->isIceAt(x + 1, y) && !getWorld()->isBoulderAt(x + 1, y, 0.5)) {
            moveTo(x + 4, y);
        }
        else {
            setDead();
        }
        break;
    case up:
        if (!getWorld()->isIceAt(x, y + 1) && !getWorld()->isBoulderAt(x, y + 1, 0.5)) {
            moveTo(x, y + 4);
        }
        else {
            setDead();
        }
        break;
    case down:
        if (!getWorld()->isIceAt(x, y - 1) && !getWorld()->isBoulderAt(x, y - 1, 0.5)) {
            moveTo(x, y - 4);
        }
        else {
            setDead();
        }
        break;
    }
    m_distanceTraveled++;
}


//============================================CHANGED=====================================================
// GoldNugget class implementation
GoldNugget::GoldNugget(int startX, int startY, bool temporary, bool isPickupableByIceman, StudentWorld* world)
    : Actor(IID_GOLD, startX, startY, right, 1.0, 2, world), m_temporary(temporary), m_ticksLeft(temporary ? 100 : -1), m_isPickupableByIceman(isPickupableByIceman) {
    setVisible(!isPickupableByIceman); // Buried nuggets start invisible, dropped nuggets start visible
}

GoldNugget::~GoldNugget() {}

void GoldNugget::setVisible(bool visible) {
    m_visible = visible;
    Actor::setVisible(visible); // Call the base class function to set visibility
}

void GoldNugget::doSomething() {
    if (!isAlive()) return;

    int icemanX = getWorld()->getIcemanX();
    int icemanY = getWorld()->getIcemanY();
    double distanceToIceman = getWorld()->calculateDistance(getX(), getY(), icemanX, icemanY);

    // If the nugget is invisible and Iceman is within 4.0 units, make it visible
    if (!isVisible() && distanceToIceman <= 4.0) {
        setVisible(true);
        return;
    }

    // If the nugget is pickup-able by Iceman and within 3.0 units, handle pickup by Iceman
    if (m_isPickupableByIceman && distanceToIceman <= 3.0) {
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(10);
        getWorld()->getIceman()->setGoldNugget(getWorld()->getIceman()->getGoldNugget() + 1);
        return;
    }

    // If the nugget is pickup-able by Protesters and within 3.0 units, handle pickup by Protesters
    if (!m_isPickupableByIceman) {
        Protester* protester = getWorld()->anyProtesterPickUpGold(this);
        if (protester && getWorld()->calculateDistance(getX(), getY(), protester->getX(), protester->getY()) <= 3.0) {
            setDead();
            getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
            protester->getBribed(); // Assume Protester has a getBribed method
            getWorld()->increaseScore(25);
            return;
        }
    }

    // If the nugget is temporary and its lifetime has elapsed, set it to dead
    if (m_temporary && m_ticksLeft > 0) {
        m_ticksLeft--;
        if (m_ticksLeft == 0) {
            setDead();
        }
    }
}






//========================================================================================================


//=======================================================UNFINISHED!!!!!===========================================
// Protester class 
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

void Protester::getBribed() {
    // Logic for handling the bribery
    setDead();
    m_leaveOilField = true;
    getWorld()->increaseScore(25);  // Increase score for bribing
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);  // Play sound effect
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
}
//======================================================================================================================

// SonarKit class ----------------------------------
SonarKit::SonarKit(int startX, int startY, StudentWorld* world)
    : Actor(IID_SONAR, startX, startY, right, 1.0, 2, world),
    m_ticksLeft(std::max(100, 300 - 10 * (int)world->getLevel())) {
    setVisible(true);
}
SonarKit::~SonarKit() {}

void SonarKit::doSomething() {
    if (!isAlive()) return;

    if (getWorld()->calculateDistance(getX(), getY(), getWorld()->getIcemanX(), getWorld()->getIcemanY()) <= 3.0) {
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getIceman()->increaseSonarKit();
        getWorld()->increaseScore(75);
    }
    m_ticksLeft--;
    if (m_ticksLeft <= 0) {
        setDead();
    }
}
//---------------------------------------------------------------------

WaterPool::WaterPool(int startX, int startY, StudentWorld* world)
    : Actor(IID_WATER_POOL, startX, startY, right, 1.0, 2, world),
    m_ticksLeft(std::max(100, 300 - 10 * (int)world->getLevel())) {
    setVisible(true);
}

WaterPool::~WaterPool() {}

void WaterPool::doSomething() {
    if (!isAlive()) return;

    // Check if within radius of 3.0 units from Iceman
    if (getWorld()->calculateDistance(getX(), getY(), getWorld()->getIcemanX(), getWorld()->getIcemanY()) <= 3.0) {
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getIceman()->increaseWater(5); // Method to increase Iceman's water
        getWorld()->increaseScore(100);
    }

    // Check if lifetime has elapsed
    if (m_ticksLeft > 0) {
        m_ticksLeft--;
    }
    else {
        setDead();
    }
}


//=====================================================================================


Barrel::Barrel(int startX, int startY, StudentWorld* world)
    : Actor(IID_BARREL, startX, startY, right, 1.0, 2, world) {
    setVisible(false); // Initially invisible
}

Barrel::~Barrel() {}

void Barrel::doSomething() {
    if (!isAlive()) return;

    // Check if the Barrel is within 4.0 radius distance for visibility
    int icemanX = getWorld()->getIcemanX();
    int icemanY = getWorld()->getIcemanY();
    double distance = getWorld()->calculateDistance(getX(), getY(), icemanX, icemanY);

    if (!isVisible() && distance <= 4.0) {
        setVisible(true); // Make the barrel visible
        return; // Return immediately after making visible
    }

    // Check if the Barrel is within 3.0 radius distance for pickup
    if (distance <= 3.0) {
        setDead(); // Barrel picked up, set it as dead
        getWorld()->playSound(SOUND_FOUND_OIL); // Play sound effect
        getWorld()->increaseScore(1000); // Increase score for picking up barrel

        // Inform StudentWorld if necessary (e.g., to track barrels picked up)
        getWorld()->barrelPickedUp();
    }
}

//============================================================================================