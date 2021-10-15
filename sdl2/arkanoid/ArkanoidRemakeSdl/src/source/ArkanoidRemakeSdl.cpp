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
#include <ArkanoidRemakeSdl.h>

#include <cmath>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <fstream>


#include <iostream>

//const int DEFAULT_WIN_WIDTH = 800;
//const int DEFAULT_WIN_HEIGHT = 600;

// Initialize the default configuration.
// Note that using a struct without methods nor custom constructors
// you can use an initializer list {...} for a static or local variable.
// With a compiler supporting the complete C++11 standard you can do this
// also with other classes or struct without this restriction.
ArkanoidRemakeSdl::Configuration ArkanoidRemakeSdl::smDefaultConfig = {false,800,512,true,16,32};


ArkanoidRemakeSdl::ArkanoidRemakeSdl()
  : mConfig(smDefaultConfig)
  // the default configuration in smDefaultConfig is used to initialize the 
  // mConfig attribute, because a default copy constructor is automatically
  // provided by the compiler for the Configuration struct
{
  mPrevTime = 0;
  mCurrTime = 0;

  mShipSurf = "ship";
  mBallSurf = "ball";
  mPlayAreaSurf = "playarea";
  mBkgndSurf = "background";
  mSnowfallSurf = "snowfall";
  mBkgndScroll = 0;
  mBkgndColor = 0x072777;
}

bool ArkanoidRemakeSdl::Init()
{
  LoadConfiguration("config.txt");
  mBall.SetDefaultSpeed(360.0);
  return InitSdl() && LoadFiles() && ArkanoidRemake::Init();
}


bool ArkanoidRemakeSdl::InitSdl()
{
  mSdlApp.InitSdl();

  //Create window
  if (!mSdlApp.CreateWindow("Arkanoid remake", mConfig.FullScreen, mConfig.WindowWidth, mConfig.WindowHeight))
  {
    return false;
  }

  //The attributes of the screen
  int screen_width = mSdlApp.WinWidth();
  int screen_height = mSdlApp.WinHeight();
  int width = 580;
  int border_l = 20;
  int border_r = screen_width-width;
  int border_b = 32;
  int border_t = 32;

  mPlayArea.Pos().X = border_l;
  mPlayArea.Pos().Y = border_b;
  mPlayArea.Size().X = width;
  mPlayArea.Size().Y = screen_height-border_b-border_t;

  mSdlApp.SetEventFunc(new AppEventFunctor(this));

  //If everything initialized fine
  return true;
}


void ArkanoidRemakeSdl::PlaySound(const std::string& name)
{
  mSdlApp.PlaySound(name);
}


void ArkanoidRemakeSdl::DrawBlocks()
{
  unsigned int row;
  unsigned int col;

  for (row = 0; row < mBlock.Rows(); row++)
  {
    for (col = 0; col < mBlock.Cols(); col++)
    {
      Block& b = mBlock(row,col);
      if (b.Visible())
      {
        int x = mPlayArea.Pos().X + b.MinX();
        int y = mPlayArea.Pos().Y + b.MinY();
        mSdlApp.ApplySurfMatrixElem(mBlockMatrixName,b.Type(),b.State(),x,y);
      }
    }
  }
}

bool ArkanoidRemakeSdl::SetBackground(const std::string& file_name)
{
  if (!mSdlApp.LoadSurface("tmp",file_name)) return false;
  mSdlApp.TileSurf(0, 0, "tmp", mBkgndSurf);
  mSdlApp.DestroySurface("tmp");
  return true;
}


bool ArkanoidRemakeSdl::LoadFiles()
{
	//Load music
  mSdlApp.MusicPath() = "data/music/";

  mSdlApp.LoadMusic("silent-night-disco");
  //mSdlApp.LoadMusic("music2");

	//Load sound effects
  mSdlApp.SoundsPath() = "data/sound/";
  mSdlApp.LoadSound("ballhit");
  mSdlApp.LoadSound("balllost");
  mSdlApp.LoadSound("brickhit");
  mSdlApp.LoadSound("brickdestroy");
  mSdlApp.LoadSound("bonus1");
  mSdlApp.LoadSound("bonus2");
  mSdlApp.LoadSound("bonus3");
  mSdlApp.LoadSound("bonuslost");
  mSdlApp.LoadSound("level_complete");


  mSdlApp.SetMusicVolume(mConfig.MusicVolume);

  mSdlApp.SetSoundsVolume(mConfig.SoundsVolume);

  mSdlApp.SwitchAudio(mConfig.Audio);

  //Load fonts
  mSdlApp.FontsPath() = "data/font/";

	//Open the font
	if( !mSdlApp.LoadFont("default", "Action_Man.ttf", 24) )
	{
		return false;
	}

  //Load images
  mSdlApp.ImagesPath() = "data/images/";

  if (!mSdlApp.LoadSurface(mSnowfallSurf,"snowfall.jpg")) return false;

  //mSdlApp.LoadSurface(mSnowfallSurf,"snowfall.jpg");
  mSdlApp.NewSurface(mBkgndSurf, mSdlApp.WinWidth(), mSdlApp.WinHeight());
  //SetBackground("snowfield1.jpg");

  //Define mPlayAreaSurf
  mSdlApp.NewSurface( mPlayAreaSurf, mPlayArea.Size().X, mPlayArea.Size().Y );
  if (!mSdlApp.ExistsSurface(mPlayAreaSurf)) return false;

  mSdlApp.LoadSurface("snowflake", "snowflake.png");
  mSdlApp.LoadSurface("snowflake2", "snowflake2.png");
  mSdlApp.LoadSurface("snowflake_1", "snowflake_1.png");
  mSdlApp.LoadSurface("snowflake_2", "snowflake_2.png");
  mSdlApp.LoadSurface("snowflake_3", "snowflake_3.png");
  mSdlApp.LoadSurface("snowflake_4", "snowflake_4.png");
  mSdlApp.LoadSurface("xmasball", "xmasball.png");
  mSdlApp.LoadSurface("gift", "gift.png");
  mSdlApp.LoadSurface("gift_red", "gift_red.png");
  mSdlApp.LoadSurface(mShipSurf,"ship.png");
  mSdlApp.LoadSurface(mBallSurf,"ball.png");
  mSdlApp.SetupSurfMatrix(mBlockMatrixName,"blocksmatrix.png",11,3);
  //If there was an error in loading the image

  //if(mShipSurf == nullptr || ball == nullptr)
  //  return false;

  // Text boxes creation
  sdltk::ColorRGBA col(80, 255, 255,255);
  sdltk::ColorRGBA shadow_col(0, 0, 0, 255 );
  mSdlApp.CreateText("Score","default",col,shadow_col,2);

  mBall.Size().X = mSdlApp.GetSurfWidth(mBallSurf);
  mBall.Size().Y = mSdlApp.GetSurfHeight(mBallSurf);
  mShip.Size().X = mSdlApp.GetSurfWidth(mShipSurf);
  mShip.Size().Y = mSdlApp.GetSurfHeight(mShipSurf);
  mBall.Origin().X = -mBall.HSizeX();
  mBall.Origin().Y = -mBall.HSizeY();
  mShip.Origin().X = -mShip.HSizeX();
  mShip.Origin().Y = -mShip.HSizeY();

  // for each bonus set its origin based on the surface size
  // (its position will be set using its center)
  for (int i=0; i<(int)mBonus.size()-1; ++i)
  {
    mBonus[i].Origin()=-mBonus[i].HSize();
  }

  //If everything loaded fine
  return true;
}


void ArkanoidRemakeSdl::DrawBackground()
{
  mSdlApp.ApplySurf(0, 0, mBkgndSurf);
}

void ArkanoidRemakeSdl::DrawPlayArea()
{
  mSdlApp.TileSurf(-mPlayArea.Pos().X, mPlayArea.Pos().Y-mBkgndScroll, mSnowfallSurf, mPlayAreaSurf);

  mSdlApp.ApplySurf(mPlayArea.Pos().X, mPlayArea.Pos().Y, mPlayAreaSurf);
}

void ArkanoidRemakeSdl::DrawText()
{
  mSdlApp.RenderText("Score", mPlayArea.Pos().X, mPlayArea.MaxY()+5);
}


void ArkanoidRemakeSdl::DrawBall()
{
  int cur_x = mPlayArea.Pos().X + mBall.MinX();
  int cur_y = mPlayArea.Pos().Y + mBall.MinY();
  mBall.PrevPos() = mBall.Pos();

  mSdlApp.ApplySurf(cur_x, cur_y, mBallSurf);
}

void ArkanoidRemakeSdl::DrawShip()
{
  int x = mPlayArea.Pos().X + mShip.MinX();
  int y = mPlayArea.Pos().Y + mShip.MinY();
  mShip.PrevPos().X = mShip.Pos().X;
  mSdlApp.ApplySurf(x, y, mShipSurf);
}


void ArkanoidRemakeSdl::DrawBonus()
{
  for (size_t i=0; i<mBonus.size(); ++i)
  {
    Bonus& b = mBonus[i];
    int x = mPlayArea.Pos().X + b.Pos().X + b.Origin().X;
    int y = mPlayArea.Pos().Y + b.Pos().Y + b.Origin().Y;
    mSdlApp.ApplySurf(x,y,b.Type());
  }
}


void ArkanoidRemakeSdl::CleanUp()
{

  mSdlApp.ExitSdl();
}


void ArkanoidRemakeSdl::AddLevel()
{
  ArkanoidRemake::AddLevel();
  Level& new_level = mLevel.back();

  // configure each level (for now only 1)
  switch (new_level.mIndex)
  {
  case 1:
    new_level.mBackgroundScene = "snowfield1.jpg";
    break;
  }
}


bool ArkanoidRemakeSdl::SetCurrentLevel(unsigned int n)
{
  if (!ArkanoidRemake::SetCurrentLevel(n)) return false;
  SetBackground(mLevel[n-1].mBackgroundScene);
  int blocksize_x = 0;
  int blocksize_y = 0;
  mSdlApp.GetSurfMatrixElemSize(mBlockMatrixName,blocksize_x,blocksize_y);

  for (unsigned int row = 0; row < mBlock.Rows(); row++)
  {
    for (unsigned int col = 0; col < mBlock.Cols(); col++)
    {
      Block& block = mBlock(row,col);
      block.Size().X = blocksize_x;
      block.Size().Y = blocksize_y;
      block.Pos().X = col * blocksize_x+blocksize_x/2;
      block.Pos().Y = row * blocksize_y+blocksize_y/2;
      block.Origin().X = -blocksize_x/2;
      block.Origin().Y = -blocksize_y/2;
    }
  }
  DrawBlocks();

  return true;
}

void ArkanoidRemakeSdl::Start()
{
  //Set initial values
  mShip.Pos().X = -mShip.Origin().X;
  mShip.Pos().Y = -mShip.Origin().Y;
  mShip.PrevPos().X = mShip.Pos().X;

  ArkanoidRemake::Start();

  //Update the screen
  mSdlApp.UpdateWindow();

  //Start Time
  mTimer.Start();

  mCurrTime = GetTime();

  mSdlApp.PlayMusic();
}


double ArkanoidRemakeSdl::GetTime()
{
  return mTimer.GetTimeMs()/1000.0;
}


void ArkanoidRemakeSdl::HandleEvents()
{
  mSdlApp.HandleEvents();
}


void ArkanoidRemakeSdl::Update()
{
  ArkanoidRemake::Update();

  mSdlApp.PlayMusic(0);
}


void ArkanoidRemakeSdl::Frame()
{
  if (mSdlApp.ExistsSurface(mSnowfallSurf))
  {
    // Scroll the background
    mBkgndScroll += 20.0*(mCurrTime-mPrevTime);
    if (mBkgndScroll>0) mBkgndScroll=-mSdlApp.GetSurfHeight(mSnowfallSurf);
  }
  DrawBackground();
  DrawPlayArea();
  DrawBlocks();
  DrawBall();
  DrawShip();
  DrawBonus();
  DrawText();
  mSdlApp.UpdateWindow();
}


void ArkanoidRemakeSdl::Stop()
{
  mTimer.Stop();
  SaveConfiguration("config.txt");
}


ArkanoidRemakeSdl::AppEventFunctor::AppEventFunctor(ArkanoidRemakeSdl* app)
  : mApp(app)
{
}

bool ArkanoidRemakeSdl::AppEventFunctor::operator() (const sdltk::EventData& evt_data)
{
  switch(evt_data.Type)
  {
    case sdltk::MOUSE:
      switch (evt_data.Action)
      {
        case sdltk::DOWN:
          mApp->LaunchBall();
          break;
        case sdltk::MOVE:
          mApp->CalculateShipPosition(evt_data.Point.X);
          break;
      }
      break;
    case sdltk::KEY:
      switch (evt_data.Action)
      {
        case sdltk::DOWN:
          switch (evt_data.Button)
          {
            case sdltk::ESC:
              //mApp->Quit();
              mApp->mQuit = true;
              break;
          }
          break;
        case sdltk::UP:
          switch (evt_data.Button)
          {
            case 'a':
            case 'A':
              mApp->mSdlApp.SwitchAudio(!mApp->mSdlApp.AudioEnabled());
              mApp->mConfig.Audio = mApp->mSdlApp.AudioEnabled();
              std::cout<<"Audio "<<(mApp->mConfig.Audio ? "on" : "off")<<std::endl;
              break;
          }
          break;
      }
      break;

    case sdltk::APP:
      switch (evt_data.Action)
      {
        case sdltk::QUIT:
          //mApp->Quit();
          mApp->mQuit = true;
          break;
      }
      break;
  }
  return true;
}





void ArkanoidRemakeSdl::HitBlock(Block& block, int x, int y)
{
  ArkanoidRemake::HitBlock(block, x, y);
  PlaySound( block.Visible() ? "brickhit" : "brickdestroy" );
}


void ArkanoidRemakeSdl::HitShip()
{
  ArkanoidRemake::HitShip();
  PlaySound( "ballhit" );
}


void ArkanoidRemakeSdl::GetBonus(const Bonus& bonus)
{
  ArkanoidRemake::GetBonus(bonus);
  std::string bonus_snd;
  if (bonus.Score()>15) bonus_snd = "bonus3";
  else if (bonus.Score()>10) bonus_snd = "bonus2";
  else bonus_snd = "bonus1";
  PlaySound(bonus_snd);
}


bool ArkanoidRemakeSdl::DropBonus(const std::string& name, const VecXy& pos)
{
  if ( !ArkanoidRemake::DropBonus(name,pos) ) return false;
  Bonus& b = mBonus.back();
  //b.SetVelY(-200);
  b.Size().X = mSdlApp.GetSurfWidth(name);
  b.Size().Y = mSdlApp.GetSurfHeight(name);
  b.Origin().X = -b.Size().X/2.0;
  b.Origin().Y = -b.Size().Y/2.0;

  return true;
}


void ArkanoidRemakeSdl::BonusLost(const Bonus& bonus)
{
  ArkanoidRemake::BonusLost(bonus);
  PlaySound("bonuslost");
}

void ArkanoidRemakeSdl::BallLost()
{
  ArkanoidRemake::BallLost();
  PlaySound("balllost");
}

void ArkanoidRemakeSdl::UpdateScoreText()
{
  ArkanoidRemake::UpdateScoreText();
  mSdlApp.SetText("Score",mScoreText);
}


void ArkanoidRemakeSdl::AddScore(int n)
{
  ArkanoidRemake::AddScore(n);
}


void ArkanoidRemakeSdl::GoToNextLevel()
{
  ArkanoidRemake::GoToNextLevel();
  PlaySound("level_complete");
}


bool ArkanoidRemakeSdl::LoadConfiguration(const std::string& file_path)
{
  std::ifstream ifs(file_path.c_str());
  if (!ifs.good())
  {
    std::cout << "Error loading configuration from " << file_path << std::endl;
    return false;
  }
  // skip data description (skip the first line)
  ifs.ignore(std::numeric_limits<int>::max(),'\n');
  ifs.setf(std::ios::boolalpha); // enable reading true and false as bool
  ifs >> mConfig.FullScreen;
  ifs >> mConfig.WindowWidth;
  ifs >> mConfig.WindowHeight;
  ifs >> mConfig.Audio;
  ifs >> mConfig.MusicVolume;
  ifs >> mConfig.SoundsVolume;
  // the file is automatically closed on exit
  return true;
}


bool ArkanoidRemakeSdl::SaveConfiguration(const std::string& file_path)
{
  std::ofstream ofs(file_path.c_str());
  if (!ofs.good())
  {
    std::cout << "Error saving configuration to " << file_path << std::endl;
    return false;
  }
  // add a text description to make the file human-readable
  ofs << "Fullscreen Width Height Audio Music Sound" << std::endl;
  ofs << ' ' << std::boolalpha << mConfig.FullScreen;
  ofs << ' ' << mConfig.WindowWidth;
  ofs << ' ' << mConfig.WindowHeight;
  ofs << ' ' << mConfig.Audio;
  ofs << ' ' << mConfig.MusicVolume;
  ofs << ' ' << mConfig.SoundsVolume;
  ofs << std::endl;
  // the file is automatically closed on exit
  return true;
}

