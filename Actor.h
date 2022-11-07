#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
//#include "StudentWorld.h"
class StudentWorld;


const int NO_GOODIE = 0;
const int STAR_GOODIE = 1;
const int FLOWER_GOODIE = 2;
const int MUSHROOM_GOODIE = 3;
const int PEACH_FIREBALL = 4;
const int PIRANHA_FIREBALL = 5;
const int SHELL_FLOATER = 6;

const int MONSTER_MOVE_DISTANCE = 1;
const int GOODIE_MOVE_DISTANCE = 2;
const int FIREBALL_MOVE_DISTANCE = 2;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* sw, int imageID, int startX, int startY, int dir = 0, int depth = 0, double size = 1.0);
    virtual ~Actor();
    virtual void doSomething() = 0;
    virtual void getBonked(bool bonkerIsInvinciblePeach = false) {}
    bool isAlive() {return m_alive;}
    void setDead() {m_alive = false;}
    virtual bool blocksMovement() {return false;}
    virtual bool isDamageable() {return false;}
    virtual void recieveDamage() {}
    
    StudentWorld* worldPtr() {return m_world;}
    
private:
    StudentWorld* m_world;
    bool m_alive;
};




class Peach : public Actor
{
public:
    Peach(StudentWorld* sw, int startX, int startY);
    virtual void doSomething();
    virtual void getBonked(bool bonkerIsInvinciblePeach = false);
    virtual bool isDamageable() {return true;}
    bool hasStarPower() {return m_has_star_power;};
    bool isInvincible() {return m_invincibility_counter >= 1;}
    bool hasJumpPower() {return m_has_jump_power;}
    bool hasShootingPower() {return m_has_shoot_power;}
    void recievePower(int powerType); // star power 150 game ticks
    virtual void recieveDamage() { getBonked(); } // Make sure it's correct
private:
    void gainInvincibility(int m_ticks) {m_invincibility_counter = m_ticks;}
    bool isReadyToShoot() {return m_time_to_recharge_before_next_fire == 0;}
    void moveUp();
    void fall();
    void setHP(int hp) {m_hp = hp;}
    int m_hp;
    bool m_has_star_power;
    int m_invincibility_counter;
    bool m_has_shoot_power;
    int m_time_to_recharge_before_next_fire;
    bool m_has_jump_power;
    int m_remaining_jump_distance;
};





class Monster : public Actor
{
public:
    Monster(StudentWorld* sw, int startX, int startY, int imageID);
    virtual void doSomething();
    virtual void getBonked(bool bonkerIsInvinciblePeach = false); // TODO: impliment getBonked
    virtual bool isDamageable() {return true;}
    virtual void recieveDamage();
    
private:
    virtual bool canWalkAround() {return true;}
    virtual bool canShootAtPeach() {return false;}
    virtual bool canDropShell() {return false;}
    void moveIfPossible();
    void attemptToShootFireball();
    int m_firing_delay;
};

class Koopa : public Monster
{
public:
    Koopa(StudentWorld* sw, int startX, int startY);
private:
    virtual bool canDropShell() {return true;}
};

class Goomba : public Monster
{
public:
    Goomba(StudentWorld* sw, int startX, int startY);
};

class Piranha : public Monster
{
public:
    Piranha(StudentWorld* sw, int startX, int startY);
private:
    virtual bool canWalkAround() {return false;}
    virtual bool canShootAtPeach() {return true;}
};




class Block_Base : public Actor
{
public:
    Block_Base(StudentWorld* sw, int startX, int startY, int goodieType);
    virtual void doSomething() {}
    virtual bool blocksMovement() {return true;}
};

class Block : public Block_Base
{
public:
    Block(StudentWorld* sw, int startX, int startY, int goodieType);
    virtual void getBonked(bool bonkerIsInvinciblePeach = false);
private:
    int m_goodieType;
};

class Pipe : public Block_Base
{
public:
    Pipe(StudentWorld* sw, int startX, int startY);
};





class End_Object : public Actor
{
public:
    End_Object(StudentWorld* sw, int startX, int startY, int imageID);
    virtual void doSomething();
private:
    virtual void initiateNextStep() = 0;
};

class Flag : public End_Object
{
public:
    Flag(StudentWorld* sw, int startX, int startY);
private:
    virtual void initiateNextStep();
};

class Mario : public End_Object
{
public:
    Mario(StudentWorld* sw, int startX, int startY);
private:
    virtual void initiateNextStep();
};





class Floater : public Actor
{
public:
    Floater(StudentWorld* sw, int startX, int startY, int dir, int imageID);
    virtual void doSomething();
private:
    virtual bool diesWhenHittingWall() {return false;}
    virtual void doUniqueAction() = 0;
};

class Flower : public Floater
{
public:
    Flower(StudentWorld* sw, int startX, int startY);
private:
    virtual void doUniqueAction();
};

class Mushroom : public Floater
{
public:
    Mushroom(StudentWorld* sw, int startX, int startY);
private:
    virtual void doUniqueAction();
};

class Star : public Floater
{
public:
    Star(StudentWorld* sw, int startX, int startY);
private:
    virtual void doUniqueAction();
};




class Piranha_Fireball : public Floater
{
public:
    Piranha_Fireball(StudentWorld* sw, int startX, int startY, int dir);
private:
    virtual void doUniqueAction();
    virtual bool diesWhenHittingWall() {return true;}
};

class Peach_Fireball : public Floater
{
public:
    Peach_Fireball(StudentWorld* sw, int startX, int startY, int dir);
private:
    virtual void doUniqueAction();
    virtual bool diesWhenHittingWall() {return true;}
};

class Shell : public Floater
{
public:
    Shell(StudentWorld* sw, int startX, int startY, int dir);
private:
    virtual void doUniqueAction();
    virtual bool diesWhenHittingWall() {return true;}
};

#endif // ACTOR_H_
