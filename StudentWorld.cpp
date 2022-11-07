#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>
#include "Actor.h" // is this ok??
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}



StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_level(assetPath), m_gameWon(false), m_finishedLevel(false)
{
}

int StudentWorld::init()
{
    m_gameWon = false;
    m_finishedLevel = false;
    
    ostringstream levelFile;
    levelFile.setf(ios::fixed);
    levelFile.fill('0');
    levelFile << "level" << setw(2) << getLevel() << ".txt";
    if (m_level.loadLevel(levelFile.str()) != Level::load_success)
        return GWSTATUS_LEVEL_ERROR;
    
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            const int realX = x*SPRITE_WIDTH;
            const int realY = y*SPRITE_HEIGHT;
//            empty, peach, koopa, goomba, piranha, block, star_goodie_block, mushroom_goodie_block, flower_goodie_block, pipe, flag, mario
            switch (m_level.getContentsOf(x, y)) {
                    
                case Level::peach:
                    m_peach = new Peach(this, realX, realY);
                    break;
                    
                    
                case Level::block:
                    m_actors.push_back(new Block(this, realX, realY, NO_GOODIE));
                    break;
                case Level::star_goodie_block:
                    m_actors.push_back(new Block(this, realX, realY, STAR_GOODIE));
                    break;
                case Level::mushroom_goodie_block:
                    m_actors.push_back(new Block(this, realX, realY, MUSHROOM_GOODIE));
                    break;
                case Level::flower_goodie_block:
                    m_actors.push_back(new Block(this, realX, realY, FLOWER_GOODIE));
                    break;
                    
                case Level::pipe:
                    m_actors.push_back(new Pipe(this, realX, realY));
                    break;
                    
                    
                case Level::koopa:
                    m_actors.push_back(new Koopa(this, realX, realY));
                    break;
                case Level::goomba:
                    m_actors.push_back(new Goomba(this, realX, realY));
                    break;
                case Level::piranha:
                    m_actors.push_back(new Piranha(this, realX, realY));
                    break;
                    
                    
                case Level::flag:
                    m_actors.push_back(new Flag(this, realX, realY));
                    break;
                case Level::mario:
                    m_actors.push_back(new Mario(this, realX, realY));
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    updateStatusLine();
    if (m_gameWon)
        return GWSTATUS_PLAYER_WON;
    if (m_finishedLevel)
        return GWSTATUS_FINISHED_LEVEL;
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    m_peach->doSomething();
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++)
        if ((*i)->isAlive())
            (*i)->doSomething();
    
    // Clean up deads
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); )
        if (!(*i)->isAlive())
        {
            delete *i;
            i = m_actors.erase(i);
        }
        else
            i++;
    if (!m_peach->isAlive())
    {
        decLives();
        if (getLives() > 0)
            playSound(SOUND_PLAYER_DIE);
        else
            playSound(SOUND_GAME_OVER);
        return GWSTATUS_PLAYER_DIED;
    }
    
    //    decLives();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
//    vector<Actor*>::iterator i = m_actors.begin();
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); )
    {
        delete *i;
        i = m_actors.erase(i);
    }
    delete m_peach;
}

//vector<Actor*>::iterator StudentWorld::overlapAt(int x, int y)
bool StudentWorld::movementBlockedAt(bool bonkerIsInvinciblePeach, double x, double y, bool shouldBonkIfEmpty, bool shouldBonkIfFilled)
{
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++)
    {
        if ( ( (*i)->getX()   >=   (x - (SPRITE_WIDTH -1))   &&   (*i)->getX()   <=   (x + (SPRITE_WIDTH -1))  )
          && ( (*i)->getY()   >=   (y - (SPRITE_HEIGHT-1))   &&   (*i)->getY()   <=   (y + (SPRITE_HEIGHT-1))  ) )
        {
            if (shouldBonkIfEmpty)
                (*i)->getBonked(bonkerIsInvinciblePeach);

            if ((*i)->blocksMovement())
            {
                if (shouldBonkIfFilled && !shouldBonkIfEmpty)
                    (*i)->getBonked(bonkerIsInvinciblePeach);
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::damageActorAt(double x, double y)
{
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++)
    {
        if ( ( (*i)->getX()   >=   (x - (SPRITE_WIDTH -1))   &&   (*i)->getX()   <=   (x + (SPRITE_WIDTH -1))  )
          && ( (*i)->getY()   >=   (y - (SPRITE_HEIGHT-1))   &&   (*i)->getY()   <=   (y + (SPRITE_HEIGHT-1))  ) )
            if ((*i)->isDamageable() &&  (*i)->isAlive())
        {
            (*i)->recieveDamage();
            return true;
        }
    }
    return false;
}

bool StudentWorld::overlapsWithPeach(double x, double y)
{
    double peachX = m_peach->getX();
    double peachY = m_peach->getY();
    return ( x   >=   (peachX - (SPRITE_WIDTH -1))   &&   x   <=   (peachX + (SPRITE_WIDTH -1))  )
    && ( y       >=   (peachY - (SPRITE_HEIGHT-1))   &&   y   <=   (peachY + (SPRITE_HEIGHT-1))  );
}

bool StudentWorld::damagePeachAt(double x, double y)
{
    double peachX = m_peach->getX();
    double peachY = m_peach->getY();
    if ( ( peachX   >=   (x - (SPRITE_WIDTH -1))   &&   peachX   <=   (x + (SPRITE_WIDTH -1))  )
      && ( peachY   >=   (y - (SPRITE_HEIGHT-1))   &&   peachY   <=   (y + (SPRITE_HEIGHT-1))  ) )
    {
        m_peach->recieveDamage();
        return true;
    }
    return false;
}

bool StudentWorld::isAboveAir(int x, int y, int direction)
{
    if (direction == 0)
        x += (SPRITE_WIDTH-1);
    y--;
    
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++)
    {
        if ( ( (*i)->getX() + (SPRITE_WIDTH-1)   >=   x   &&   (*i)->getX()   <=   x  )
          && ( (*i)->getY()   >=   (y - (SPRITE_HEIGHT-1))   &&   (*i)->getY()   <=   (y + (SPRITE_HEIGHT-1))  )
            && (*i)->blocksMovement())
            return false;
    }
    return true;
}

void StudentWorld::spawnFloater(double x, double y, int floaterType, int dir)
{
    switch (floaterType) {
        case STAR_GOODIE:
            m_actors.push_back(new Star(this, x, y));
            break;
        case FLOWER_GOODIE:
            m_actors.push_back(new Flower(this, x, y));
            break;
        case MUSHROOM_GOODIE:
            m_actors.push_back(new Mushroom(this, x, y));
            break;
        case PEACH_FIREBALL:
            m_actors.push_back(new Peach_Fireball(this, x, y, m_peach->getDirection()));
            break;
        case PIRANHA_FIREBALL:
            m_actors.push_back(new Piranha_Fireball(this, x, y, dir));
            break;
        case SHELL_FLOATER:
            m_actors.push_back(new Shell(this, x, y, dir));
            break;
            
        default:
            break;
    }
}

void StudentWorld::updateStatusLine()
{
    ostringstream status;
    string hasJumpPower = "";
    string hasShootPower = "";
    string hasStarPower = "";
    if (m_peach->hasShootingPower())
        hasShootPower = " ShootPower!";
    if (m_peach->hasJumpPower())
        hasJumpPower = " JumpPower!";
    if (m_peach->hasStarPower())
        hasStarPower = " StarPower!";
    status << "Lives: " << getLives() << "  Level: " << getLevel() << "  Points: " << getScore() << hasStarPower << hasShootPower << hasJumpPower;
    setGameStatText(status.str());
}
