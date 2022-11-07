#include "Actor.h"
#include "StudentWorld.h"

Actor::Actor(StudentWorld* sw, int imageID, int startX, int startY, int dir, int depth, double size)
: GraphObject(imageID, startX, startY, dir, depth, size), m_world(sw), m_alive(true)
{}




Block_Base::Block_Base(StudentWorld* sw, int startX, int startY, int imageID)
: Actor(sw, imageID, startX, startY, 0, 2)
{}

Block::Block(StudentWorld* sw, int startX, int startY, int goodieType)
: Block_Base(sw, startX, startY, IID_BLOCK), m_goodieType(goodieType)
{}

Pipe::Pipe(StudentWorld* sw, int startX, int startY)
: Block_Base(sw, startX, startY, IID_PIPE)
{}




End_Object::End_Object(StudentWorld* sw, int startX, int startY, int imageID)
: Actor(sw, imageID, startX, startY, 0, 1)
{}

Flag::Flag(StudentWorld* sw, int startX, int startY)
: End_Object(sw, startX, startY, IID_FLAG)
{}

Mario::Mario(StudentWorld* sw, int startX, int startY)
: End_Object(sw, startX, startY, IID_MARIO)
{}



Monster::Monster(StudentWorld* sw, int startX, int startY, int imageID)
: Actor(sw, imageID, startX, startY, randInt(0, 1)*180), m_firing_delay(0)
{}

Koopa::Koopa(StudentWorld* sw, int startX, int startY)
: Monster(sw, startX, startY, IID_KOOPA)
{}

Goomba::Goomba(StudentWorld* sw, int startX, int startY)
: Monster(sw, startX, startY, IID_GOOMBA)
{}

Piranha::Piranha(StudentWorld* sw, int startX, int startY)
: Monster(sw, startX, startY, IID_PIRANHA)
{}





Floater::Floater(StudentWorld* sw, int startX, int startY, int dir, int imageID)
: Actor(sw, imageID, startX, startY, dir, 1)
{}

Flower::Flower(StudentWorld* sw, int startX, int startY)
: Floater(sw, startX, startY, 0, IID_FLOWER)
{}

Mushroom::Mushroom(StudentWorld* sw, int startX, int startY)
: Floater(sw, startX, startY, 0, IID_MUSHROOM)
{}

Star::Star(StudentWorld* sw, int startX, int startY)
: Floater(sw, startX, startY, 0, IID_STAR)
{}


Piranha_Fireball::Piranha_Fireball(StudentWorld* sw, int startX, int startY, int dir)
: Floater(sw, startX, startY, dir, IID_PIRANHA_FIRE)
{}

Peach_Fireball::Peach_Fireball(StudentWorld* sw, int startX, int startY, int dir)
: Floater(sw, startX, startY, dir, IID_PEACH_FIRE)
{}


Shell::Shell(StudentWorld* sw, int startX, int startY, int dir)
: Floater(sw, startX, startY, dir, IID_SHELL)
{}





Peach::Peach(StudentWorld* sw, int startX, int startY) // move if no block, setDirection() and moveTo()
: Actor(sw, IID_PEACH, startX, startY), m_hp(1), m_invincibility_counter(0), m_time_to_recharge_before_next_fire(0), m_remaining_jump_distance(0), m_has_shoot_power(false), m_has_jump_power(false), m_has_star_power(false)
{}

Actor::~Actor()
{
}



void Peach::doSomething()
{
    if (!isAlive())
        return;
    if (isInvincible())
        m_invincibility_counter--;
    if (m_invincibility_counter == 0)
        m_has_star_power = false; 
    if (hasShootingPower() && !isReadyToShoot())
        m_time_to_recharge_before_next_fire--;
    
    if (m_remaining_jump_distance > 0)
        moveUp();
    else
        fall();
    
    int keyPressed;
    if (worldPtr()->getKey(keyPressed) == true)
    {
        int newX = getX();
        int newY = getY();
        switch (keyPressed) {
            case KEY_PRESS_LEFT:
                setDirection(180);
                newX -= 4;
                break;
            case KEY_PRESS_RIGHT:
                setDirection(0);
                newX += 4;
                break;
            case KEY_PRESS_UP:
                if (!worldPtr()->movementBlockedAt(hasStarPower(), getX(), getY()-1, false, false))
                    break;
                if (hasJumpPower())
                    m_remaining_jump_distance = 12;
                else
                    m_remaining_jump_distance = 8;
                worldPtr()->playSound(SOUND_PLAYER_JUMP);
                break;
            case KEY_PRESS_SPACE:
                if (!hasShootingPower() || !isReadyToShoot()) break;
                worldPtr()->playSound(SOUND_PLAYER_FIRE);
                m_time_to_recharge_before_next_fire = 8;
                worldPtr()->spawnFloater(getX() + 4 - 8*(getDirection()==180), getY(), PEACH_FIREBALL);
                break;

            default:
                break;
        }
        
        
        if (!worldPtr()->movementBlockedAt(hasStarPower(), newX, newY, true, true)) // This bonks if its blocked
            moveTo(newX, newY);
        
    }
}

void Peach::getBonked(bool bonkerIsInvinciblePeach)
{

    if (isInvincible()) return;
    m_hp--;
    gainInvincibility(10);
    m_has_shoot_power = false;
    m_has_jump_power = false;
    if (m_hp >= 1)
    {
        worldPtr()->playSound(SOUND_PLAYER_HURT);
    }
    else
    {
        setDead();
    }
}

void Peach::moveUp()
{
    int newX = getX();
    int newY = getY() + 4;
//#error figure out if this is false true or true true
    if (worldPtr()->movementBlockedAt(hasStarPower(), newX, newY, false, true))
    {
        worldPtr()->playSound(SOUND_PLAYER_BONK); // need to play sound when goodie gotten
        m_remaining_jump_distance = 0;
    }
    else
    {
        moveTo(newX, newY);
        m_remaining_jump_distance--;
    }
}

void Peach::fall()
{
    int newX = getX();
    int newY = getY() - 4;
//#warning make sure its not false true
    if (!worldPtr()->movementBlockedAt(hasStarPower(), newX, newY+1, false, false))
        moveTo(newX, newY);
}


void Block::getBonked(bool bonkerIsInvinciblePeach)
{

    if (m_goodieType == NO_GOODIE)
    {
        worldPtr()->playSound(SOUND_PLAYER_BONK);
        return;
    }
    worldPtr()->playSound(SOUND_POWERUP_APPEARS);
    worldPtr()->spawnFloater(getX(), getY()+8, m_goodieType);
    m_goodieType = NO_GOODIE;
    
}

void Peach::recievePower(int powerType)
{
    switch (powerType) {
        case STAR_GOODIE:
            gainInvincibility(150);
            m_has_star_power = true;
            break;
        case FLOWER_GOODIE:
            setHP(2);
            m_has_shoot_power = true;
            break;
        case MUSHROOM_GOODIE:
            setHP(2);
            m_has_jump_power = true;
            break;
            
        default:
            break;
    }
}

void End_Object::doSomething()
{
    if (!isAlive())
        return;
    if (worldPtr()->overlapsWithPeach(getX(), getY()))
    {
        worldPtr()->increaseScore(1000);
        setDead();
        initiateNextStep();
    }
}

void Mario::initiateNextStep() { worldPtr()->playSound(SOUND_FINISHED_LEVEL); worldPtr()->gameWon(); }
void Flag::initiateNextStep()  { worldPtr()->playSound(SOUND_FINISHED_LEVEL); worldPtr()->finishedLevel(); }

void Monster::doSomething()
{
    if (!isAlive())
        return;
    if (worldPtr()->overlapsWithPeach(getX(), getY()))
    {
        worldPtr()->bonkPeach();
        return;
    }
    if (canWalkAround())
        moveIfPossible();
    else
        increaseAnimationNumber();
    
    attemptToShootFireball();
}

void Monster::moveIfPossible()
{
    // Switch direction
    double newX = getX();
    double newY = getY();
    getPositionInThisDirection(getDirection(), MONSTER_MOVE_DISTANCE, newX, newY);
    if (worldPtr()->movementBlockedAt(false, newX, newY, false, false))
        setDirection(getDirection()-180);
    else if (worldPtr()->isAboveAir(newX, newY, getDirection()))
        setDirection(getDirection()-180);
    
    // Move or return
    newX = getX();
    newY = getY();
    getPositionInThisDirection(getDirection(), MONSTER_MOVE_DISTANCE, newX, newY);
    if (worldPtr()->movementBlockedAt(false, newX, newY, false, false))
        return;
    moveForward();
}

void Monster::attemptToShootFireball()
{
    const double peachY = worldPtr()->getPeachY();
    const double peachX = worldPtr()->getPeachX();
    const double fireballYRange = 1.5*SPRITE_HEIGHT;
    const double fireballXRange = 8*SPRITE_WIDTH;
    if (!canShootAtPeach() || !(peachY >= getY()-fireballYRange && peachY <= getY()+fireballYRange))
        return;
    setDirection( (peachX < getX()) * 180 );
    if (m_firing_delay > 0)
    {
        m_firing_delay--;
        return;
    }
    if (peachX >= getX()+fireballXRange || peachX <= getX()-fireballXRange)
        return;
    worldPtr()->spawnFloater(getX(), getY(), PIRANHA_FIREBALL, getDirection());
    worldPtr()->playSound(SOUND_PIRANHA_FIRE);
    m_firing_delay = 40;
}

void Monster::getBonked(bool bonkerIsInvinciblePeach)
{
    if (!bonkerIsInvinciblePeach) return;
    worldPtr()->playSound(SOUND_PLAYER_KICK);
    worldPtr()->increaseScore(100);
    setDead();
    if (canDropShell())
        worldPtr()->spawnFloater(getX(), getY(), getDirection());
}

void Monster::recieveDamage()
{
    worldPtr()->increaseScore(100);
    setDead();
    if (canDropShell())
        worldPtr()->spawnFloater(getX(), getY(), SHELL_FLOATER, getDirection());
}





void Floater::doSomething()
{
    doUniqueAction();
    if (!isAlive())
        return;
    if (!worldPtr()->movementBlockedAt(false, getX(), getY()-2, false, false))
        moveTo(getX(), getY()-2);
    if (worldPtr()->movementBlockedAt(false, getX()-2+(getDirection() == 0)*2*2, getY(), false, false))
    {
        if (diesWhenHittingWall())
            setDead();
        setDirection(getDirection()-180);
        return;
    }
    moveForward(2);
}

void Flower::doUniqueAction()
{
    if (!(worldPtr()->overlapsWithPeach(getX(), getY())))
        return;
    worldPtr()->increaseScore(50);
    worldPtr()->givePeachPower(FLOWER_GOODIE);
    setDead();
    worldPtr()->playSound(SOUND_PLAYER_POWERUP);
}

void Mushroom::doUniqueAction()
{
    if (!(worldPtr()->overlapsWithPeach(getX(), getY())))
        return;
    worldPtr()->increaseScore(75);
    worldPtr()->givePeachPower(MUSHROOM_GOODIE);
    setDead();
    worldPtr()->playSound(SOUND_PLAYER_POWERUP);
}

void Star::doUniqueAction()
{
    if (!(worldPtr()->overlapsWithPeach(getX(), getY())))
        return;
    worldPtr()->increaseScore(100);
    worldPtr()->givePeachPower(STAR_GOODIE);
    setDead();
    worldPtr()->playSound(SOUND_PLAYER_POWERUP);
}




void Piranha_Fireball::doUniqueAction()
{
    if (!(worldPtr()->overlapsWithPeach(getX(), getY())))
        return;


    worldPtr()->damagePeach();
    setDead();
}

void Peach_Fireball::doUniqueAction() // TODO: needs to damage things
{
     if (worldPtr()->damageActorAt(getX(),getY()))
         setDead(); 
}

void Shell::doUniqueAction()
{
    if (worldPtr()->damageActorAt(getX(),getY()))
        setDead();
}
