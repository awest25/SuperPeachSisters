#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>
#include "Actor.h"
//class Actor;
//class Peach;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld() {cleanUp();}
    virtual int init(); // load up the first level data file using our provided Level class, and then create Block objects and a Peach object and insert them into its data structures (based on level data file)
    virtual int move(); // each tick, it must ask Peach and other actors (just Blocks for now) to do something
    virtual void cleanUp(); // frees any dynamically allocated data that was allocated during calls to the init() method or the move() method
    bool movementBlockedAt(bool bonkerIsInvinciblePeach, double x, double y, bool shouldBonkIfEmpty, bool shouldBonkIfFilled); // returns false if no object
    bool damageActorAt(double x, double y);
    bool isAboveAir(int x, int y, int direction);
    bool overlapsWithPeach(double x, double y);
    double getPeachX() {return m_peach->getX();}
    double getPeachY() {return m_peach->getY();}
    void gameWon() {m_gameWon = true;}
    void finishedLevel() {m_finishedLevel = true;}
    void bonkPeach() { m_peach->getBonked(); }
    void spawnFloater(double x, double y, int floaterType, int dir = 0);
    void givePeachPower(int powerType) { m_peach->recievePower(powerType); }
    void damagePeach() {m_peach->getBonked();}
    bool damagePeachAt(double x, double y);
    
private: // Add any private data members to this class required to keep track of all game objects/actors
    void updateStatusLine();
    bool m_gameWon;
    bool m_finishedLevel;
    Level m_level;
    Peach* m_peach;
    std::vector<Actor*> m_actors;
};

#endif // STUDENTWORLD_H_
