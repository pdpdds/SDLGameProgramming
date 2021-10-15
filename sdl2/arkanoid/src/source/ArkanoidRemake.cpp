/*
ArkanoidRemakeSDL
a sample of object-oriented game programming with C++. 
version 1.0, May 2016

Copyright (c) 2016 Giovanni Paolo Vigano'

The source code of the SDL library used by ArkanoidRemakeSDL can be found here:
https://www.libsdl.org/
---
ArkanoidRemakeSDL source code is licensed with the same zlib license:
(http://www.gzip.org/zlib/zlib_license.html)

This software is provided 'as-is', without any express or implied warranty. In 
no event will the authors be held liable for any damages arising from the use of
this software.

Permission is granted to anyone to use this software for any purpose, including 
commercial applications, and to alter it and redistribute it freely, subject to 
the following restrictions:

    1. The origin of this software must not be misrepresented; you must not 
    claim that you wrote the original software. If you use this software in a 
    product, an acknowledgment in the product documentation would be appreciated
    but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#include <ArkanoidRemake.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <cmath>

#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288
#endif



// ArkanoidRemake implementation
//-------------------------------------------------------------
ArkanoidRemake::ArkanoidRemake()
{
  mCurrLevel = 0; // 0=invalid level
  mLevelScore = 0;
  mTotalScore = 0;
}

void ArkanoidRemake::AddLevel()
{
  // build a game level (the only one for now)
  // to do: load levels from configuration files

  // level number (1-based)
  unsigned int n = (unsigned int) mLevel.size()+1;
  // rows from bottom to top
  int row_blocktype[] = {0,0,0,0,0,0,0,0,0,0,1,2,5,4,7,8,6,3,9,10,0};
  // number of rows in the above array
  unsigned int nrows = sizeof(row_blocktype)/sizeof(int);

  // number of columns, for now hard-coded
  unsigned int ncols = 13;//mPlayArea.Size().X/block_size;

  // create a new level with the given size
  Level new_level(n,nrows,ncols);

  // configure each level (for now only 1)
  switch (n)
  {
  case 1:
    for (unsigned int row = 0; row <= nrows && row < new_level.mBlockMatrix.Rows(); ++row)
    {
      for (unsigned int i = 0; i < new_level.mBlockMatrix.Cols(); i++)
      {
        int type = row_blocktype[row];
        int state = 0;
        int score = 1;
        std::string bonus;
        bool visible = true;
        Block& block = new_level.mBlockMatrix(row,i);
        switch (type)
        {
        case 0:
          visible = false;
          break;
        case 3:
          state = 2;
          score = 2;
          bonus = "snowflake";
          break;
        case 6:
          bonus = "snowflake2";
          break;
        case 8:
          state = 2;
          score = 5;
          bonus = "gift_red";
          break;
        case 9:
          state = 1;
          score = 5;
          bonus = "xmasball";
          break;
        case 10:
          state = 2;
          score = 5;
          bonus = "gift";
          break;

        default:
          {
            std::string choice[] = {"snowflake_1","snowflake_2","snowflake_3","snowflake_4"};
            bonus = choice[rand()*4/RAND_MAX];
          }
          break;
        }
        block.Set(type,state,score,bonus);
        if (!visible) block.Hide();
      }
    }
    break;
  }

  // add the new level
  mLevel.push_back( std::move( new_level ) );
}

bool ArkanoidRemake::SetCurrentLevel(unsigned int n)
{
  if (n>mLevel.size()) return false;
  if (n==0) n=mCurrLevel;
  mBlock = mLevel[n-1].mBlockMatrix;
  mBlocksLeft = mBlock.CountVisibleBlocks();
  mCurrLevel = n;
  return true;
}


void ArkanoidRemake::GoToNextLevel()
{
  //++mCurrLevel; // next level
  //SetCurrentLevel(mCurrLevel);
  SetCurrentLevel(1); // for now there is only one level...
  mLevelScore = 0;
  AddScore(100);
  mBall.Stop();
  MoveBallWithShip();
}


bool ArkanoidRemake::Init()
{
  AddLevel();
  mBonusDef["snowflake"] = Bonus("snowflake", VecXy(0,-150), 15);
  mBonusDef["snowflake2"] = Bonus("snowflake2", VecXy(0,-200), 20);
  mBonusDef["snowflake_1"] = Bonus("snowflake_1", VecXy(20,-50), 1);
  mBonusDef["snowflake_2"] = Bonus("snowflake_2", VecXy(10,-55), 1);
  mBonusDef["snowflake_3"] = Bonus("snowflake_3", VecXy(-30,-60), 1);
  mBonusDef["snowflake_4"] = Bonus("snowflake_4", VecXy(-15,-65), 1);
  mBonusDef["xmasball"] = Bonus("xmasball", VecXy(0,-200), 20);
  mBonusDef["gift"] = Bonus("gift", VecXy(0,-200), 20);
  mBonusDef["gift_red"] = Bonus("gift_red", VecXy(0,-250), 25);
  return true;
}

void ArkanoidRemake::Start()
{
  // reset the score
  mLevelScore = 0;
  mTotalScore = 0;

  // start with level 1
  SetCurrentLevel(1);
  // Place the ball on the ship
  MoveBallWithShip();
  //// // and launch it
  //// LaunchBall();
  // initialize the score text
  UpdateScoreText();
}


void ArkanoidRemake::Update()
{
  HandleEvents();
  
  mPrevTime = mCurrTime;
  mCurrTime = GetTime();

  CalculateBallPosition();
  CalculateBonusPosition();

  Frame();
}


void ArkanoidRemake::MoveBallWithShip()
{
  mShip.DragBall(mBall);
}


void ArkanoidRemake::LaunchBall()
{
  if (mBall.Held()) mShip.LaunchBall(mBall);
}

bool ArkanoidRemake::CalculateShipPosition(int x)
{
  if (x < 0 ) // adjust the current position (e.g. size was changed)
  {
    x = (int) mShip.Pos().X;
  }

  // check ship position on the left
  if (x < mShip.HSizeX())
  {
    mShip.Pos().X = mShip.HSizeX();
    return false;
  }
  
  // check ship position on the right
  if (x > mPlayArea.Size().X - mShip.HSizeX())
  {
    mShip.Pos().X = mPlayArea.Size().X - mShip.HSizeX();
    return false;
  }

  // set the ship position, relative to the play area
  mShip.Pos().X = x;

  return true;
}


void ArkanoidRemake::CalculateBonusPosition()
{
  // use the elapsed time to calcukate the new position
  double elapsed_time = mCurrTime-mPrevTime;

  // for each bonus:
  //  move it
  //  handle collisions with the side walls
  for (size_t i=0; i<mBonus.size(); ++i)
  {
    mBonus[i].Move(elapsed_time);
    if (mBonus[i].MinX()<0.0f) mBonus[i].InvertVelX();
    if (mBonus[i].MaxX()>mPlayArea.Size().X) mBonus[i].InvertVelX();
  }
  // for each bonus (in reverse order, to avoid problems when it's removed)
  for (int i=(int)mBonus.size()-1; i>=0; --i)
  {
    // check for collisions with the bottom wall
    if (mBonus[i].MinY()<=mShip.MaxY())
    {
      double d = abs(mBonus[i].Pos().X-mShip.Pos().X);
      double collision_dist = (mShip.Size().X+mBonus[i].Size().X)/2;
      // check collisions with the ship
      if (d<collision_dist)
      {
        // on collision with ship get the bonus
        GetBonus(mBonus[i]);
      }
      else
      {
        BonusLost(mBonus[i]);
      }
      // let the bonus disappear
      mBonus.erase(mBonus.begin()+i);
    }
  }
}


bool ArkanoidRemake::CalculateBallPosition()
{
  // ensure the ball does not stop somewhere, getting lost
  if (mBall.Speed()==0.0)
  {
    MoveBallWithShip();
    return true;
  }

  // calculate the elapsed time interval
  double interval = (mCurrTime-mPrevTime);

  // calculate the shift (in general and along axes)
  double shift = interval * mBall.Speed();
  double shift_y = interval * mBall.Vel().Y;
  double shift_x = interval * mBall.Vel().X;

  // calculate the sub-unit increment
  double inc_x = shift_x/shift;
  double inc_y = shift_y/shift;

  // track the ball trajectory unit by unit
  bool collision = false;
  for (double i = 1.0; i <= shift; i+=1.0)
  {
    // calculate the intermediate position
    double x = mBall.Pos().X + inc_x*i;
    double y = mBall.Pos().Y + inc_y*i;
    
    // check the collision at the current coordinates
    collision = CheckBallCollision(x, y);
    if (collision)
    {
      // stop the ball at the collision time
      interval = (shift-i)/shift*interval;
      break;
    }
  }

  // move the ball according to the computed interval
  mBall.Move(interval);

  // constrain the ball position inside the play area
  double max_y = mPlayArea.Size().Y+mBall.Origin().Y-1;
  double max_x = mPlayArea.Size().X+mBall.Origin().X-1;
  if (mBall.Pos().Y>max_y)  mBall.Pos().Y = max_y;
  if (mBall.Pos().X>max_x)  mBall.Pos().X = max_x;


  // handle special cases

  if ( mBall.MinY() <= 0.0 && mBall.Vel().Y < 0)
  {
    BallLost();
    return true;
  }

  // prevent horizontal endless bouncing (let the ball fall down)
  if (!collision && abs(mBall.Vel().Y)<10.0 && mBall.Vel().X!=0.0)
  {
    mBall.SetVelY( mBall.Vel().Y - 10);
  }

  return collision;
}



bool ArkanoidRemake::CheckBallBlockCollision(const Block& block, int x, int y)
{
  // no collision if the block is not present
  if (!block.Visible()) return false;

  // distance between the block and the given coordinates
  double dx = block.Pos().X-x;
  double dy = block.Pos().Y-y;

  // contact distance between the block and the ball
  double cont_dx = mBall.HSizeX()+block.HSizeX();
  double cont_dy = mBall.HSizeY()+block.HSizeY();

  // no collision if the distance is greater than the contact distance
  if ( abs(dx)>cont_dx || abs(dy)>cont_dy ) return false;

  // move the ball at the given coordinates
  mBall.Pos().X = x;
  mBall.Pos().Y = y;

  // calculate bouncing

  // if hit on top/bottom
  if (abs(dy)>block.HSizeY())
  {
    // if vertical offset and velocity have the same sign invert vertical velocity
    if (dy*mBall.VelY()>0.0) mBall.InvertVelY();
  }
  else
  {
    // if horizontal offset and velocity have the same sign invert horizontal velocity
    if (dx*mBall.VelX()>0.0) mBall.InvertVelX();
  }

  return true;
}


bool ArkanoidRemake::CheckBallCollision(int x, int y)
{
  // collide with a block (scan the block matrix by rows and columns)
  for (unsigned int row=0; row<mBlock.Rows(); ++row)
  {
    for (unsigned int col=0; col<mBlock.Cols(); ++col)
    {
      // if colliding with the block with the current coordinates in the matrix
      if (CheckBallBlockCollision(mBlock(row,col),x,y))
      {
        // handle block collision
        HitBlock( mBlock(row,col),x,y );
        // collision detected
        return true;
      }
    }
  }

  //hits left wall
  if (x <= -mBall.Origin().X && mBall.VelX()<0.0)
  {
    // invert horizontal velocity
    mBall.InvertVelX();
    // collision detected
    return true;
  }

  //hits right wall
  if ( x >= (mPlayArea.Size().X - mBall.Size().X) && mBall.VelX()>0.0)
  {
    // invert horizontal velocity
    mBall.InvertVelX();
    // collision detected
    return true;
  }


  //hits upper wall
  if (y >= mPlayArea.Size().Y+mBall.Origin().Y-1 && mBall.Vel().Y > 0.0)
  {
    // invert vertical velocity
    mBall.InvertVelY();
    // collision detected
    return true;
  }


  // x coordinate of the ship center
  double pad_xcenter = mShip.Pos().X+mShip.Origin().X+mShip.HSizeX();

  // x coordinates difference (ship-ball)
  double dif = pad_xcenter - x;

  // horizontal distance
  double h_dist = abs(dif);

  // horizontal contact distance
  double hcontact_dist = mShip.HSizeX() + mBall.HSizeX();

  // 1st contidion for a contact: vertical position lower enough
  bool vert_contact = y <= mShip.MaxY()+mBall.HSizeY();

  // 2nd contidion for a contact: horizontal distance lower enough
  bool horiz_contact = h_dist <= hcontact_dist;

  // 3rd contidion for a contact: the ball is falling
  bool falling = mBall.Vel().Y < 0.0;

  if (vert_contact && horiz_contact && falling)
  {
    // calculate the ball trajectory angle
    // vertical velocity is <0 for the above "falling" condition
    double ball_angle = atan(mBall.Vel().X/-mBall.Vel().Y);

    // set the maximum angle a little bit less than pi/2
    double max_angle = M_PI/2.0 - 0.2;

    // the ship cannot be tilted, thus let's simulate a similar effect
    // according to the position hit by the ball on the ship
    double pad_angle = 0.0;
    double max_pad_angle = max_angle/2.0;
    if (h_dist>0.01) pad_angle = -dif/mShip.HSizeX()*max_pad_angle;

    // calculate the bouncing angle related to the vertical axis
    double angle = ball_angle-pad_angle;

    // deny a perfectly horizontal direction
    // to avoid an endless bouncing between the side walls
    if (angle>max_angle) angle = max_angle;
    if (angle<-max_angle) angle = -max_angle;

    // claculate the bouncing velocity
    double xv = mBall.Speed() * -sin(angle);
    double yv = mBall.Speed() * cos(angle);

    // set the calculated velocity
    mBall.SetVel(xv,yv);

    // handle the collsion with the ship
    HitShip();

    return true;
  }

  return false;
}


void ArkanoidRemake::UpdateScoreText()
{
  // compose the text string and assign it to the class attribute
  std::stringstream ss;
  ss << "LEVEL " << std::setw(2) << mCurrLevel << " Score: " << std::setw(5) << mTotalScore << std::ends;
  mScoreText = ss.str();
}

void ArkanoidRemake::AddScore(int n)
{
  if ((int)mLevelScore+n<0) return; // avoid negative scores

  mLevelScore += n;
  mTotalScore += n;
  UpdateScoreText();
}


void ArkanoidRemake::HitBlock(Block& block, int x, int y)
{
  // hit the block and get the related score
  int score = block.Hit();
  
  // if the block was destroyed
  if (!block.Visible())
  {
    // drop the realted bonus starting from the block position
    DropBonus(block.Bonus(), block.Pos());
    // decrease the number of visible blocks
    mBlocksLeft--;
    // if this was the last block the level is complete, go to the next level
    if (mBlocksLeft==0) GoToNextLevel();
  }

  // in any case add the score
  AddScore(score);
}


void ArkanoidRemake::HitShip()
{
  // action related to the event "ship hit the ball"
  // it can be overridden by derived classes
  // the bouncing effect is already computed by CheckBallCollision()
}


void ArkanoidRemake::BallLost()
{
  // if the ship missed the ball

  // put the ball on the ship
  MoveBallWithShip();

  // add a penalty score
  AddScore(-50);
}


void ArkanoidRemake::GetBonus(const Bonus& bonus)
{
  // if got a bonus...

  // add the score related to the bonus
  AddScore(bonus.Score());

  // (handle bonus effect...)
}


void ArkanoidRemake::BonusLost(const Bonus& bonus)
{
  // if missed a bonus...

  // some bonus type could have a penalty

  // (handle bonus effect...)
}


bool ArkanoidRemake::DropBonus(const std::string& name, const VecXy& pos)
{
  if (name.empty()) return false;
  //if (mBonusDef.find(name)==mBonusDef.end()) return;
  Bonus b = mBonusDef[name];
  b.Pos()=pos;
  mBonus.push_back(b);

  return true;
}
