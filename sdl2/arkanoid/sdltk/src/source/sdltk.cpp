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

// SDL Toolkit
#include <sdltk.h>

//Using SDL

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>


#include <cmath>
#include <string>
#include <cstdlib>
#include <sstream>


#include <iostream>
#include <cmath>

// Default values
const int SDLTK_DEFAULT_WIN_WIDTH = 800;
const int SDLTK_DEFAULT_WIN_HEIGHT = 600;

namespace sdltk
{
  EventData ConvertEvent(SDL_Event sdl_evt)
  {
    EventData evt;
    evt.Point.X = sdl_evt.motion.x;
    evt.Point.Y = sdl_evt.motion.y;
    switch(sdl_evt.type)
    {
    case SDL_MOUSEBUTTONDOWN:
      evt.Type = MOUSE;
      evt.Action = DOWN;
      break;

    case SDL_MOUSEMOTION:
      evt.Type = MOUSE;
      evt.Action = MOVE;
      break;

    case SDL_KEYDOWN:
      evt.Type = KEY;
      evt.Action = DOWN;
      evt.Button = sdl_evt.key.keysym.sym;
      break;

    case SDL_KEYUP:
      evt.Type = KEY;
      evt.Action = UP;
      evt.Button = sdl_evt.key.keysym.sym;
      break;

    case SDL_QUIT:
      evt.Type = APP;
      evt.Action = QUIT;
      break;
    }
    if (evt.Type==MOUSE)
    {
      switch(sdl_evt.button.button)
      {
      case 0:
        evt.Button = LEFT;
        break;
      case 1:
        evt.Button = MIDDLE;
        break;
      case 2:
        evt.Button = RIGHT;
        break;
      }
    }

    return evt;
  }
}


using namespace sdltk;


// SdlApp
//-----------------------------------------------------------------------------

SdlApp::SdlApp()
{
  mAutoDeleteEventCB = false;
  mEventCB = nullptr;
  mMusic = 0;
  //std::cout << __FUNCTION__ << std::endl;
}

SdlApp::~SdlApp()
{
  //std::cout << __FUNCTION__ << std::endl;
}


void SdlApp::ExitSdl()
{
  if (mAutoDeleteEventCB && mEventCB!=nullptr)
  {
    delete mEventCB;
  }
  mEventCB = nullptr;

  // destroy all the fonts in the map
  auto fnt_itr=mFont.begin();
  while (fnt_itr!=mFont.end())
  {
    TTF_CloseFont( fnt_itr->second );
    mFont.erase(fnt_itr->first);
    fnt_itr=mFont.begin();
  }

  // destroy all the sounds in the map
  auto snd_itr=mSoundFx.begin();
  while (snd_itr!=mSoundFx.end())
  {
    Mix_FreeChunk( snd_itr->second );
    mSoundFx.erase(snd_itr->first);
    snd_itr=mSoundFx.begin();
  }

  // destroy all the musics
  auto mus_itr=mPlaylist.begin();
  while (!mPlaylist.empty())
  {
    Mix_FreeMusic( *mus_itr );
    mPlaylist.erase(mus_itr);
    mus_itr=mPlaylist.begin();
  }

  // destroy all the bonus surfaces in the map
  auto itr=mSurface.begin();
  while (itr!=mSurface.end())
  {
    DestroySdlSurface(itr->second);
    mSurface.erase(itr->first);
    itr=mSurface.begin();
  }

  // destroy all the surface matrices in the map
  auto mat_itr=mSurfaceMatrix.begin();
  while (mat_itr!=mSurfaceMatrix.end())
  {
    delete mat_itr->second;
    mSurfaceMatrix.erase(mat_itr->first);
    mat_itr=mSurfaceMatrix.begin();
  }

  mWin.Destroy();

  //Quit SDL subsystems
	TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

bool SdlApp::CreateWindow(std::string title, bool fullscreen, int width, int height, int x, int y)
{
  if (!mWin.Create(title,fullscreen,width,height,x,y)) return false;
  return true;
}

// Timer
//-----------------------------------------------------------------------------


Timer::Timer()
{
  //Initialize the variables
  mStartMs = 0;
  mStarted = false;
}

void Timer::Start()
{
  //Start the timer
  mStarted = true;

  //Get the current clock time
  mStartMs = SDL_GetTicks();
}

void Timer::Stop()
{
  //Stop the timer
  mStarted = false;
}

unsigned int Timer::GetTimeMs()
{
  //If the timer is running
  if( mStarted )
  {
    //Return the current time minus the start time
    return SDL_GetTicks() - mStartMs;
  }

  //If the timer isn't running
  return 0;
}



// Window
//-----------------------------------------------------------------------------

SdlApp::Window::Window()
{
  mSdlWin = nullptr;
  mSdlWinSurf = nullptr;
  mWinW = SDLTK_DEFAULT_WIN_WIDTH;
  mWinH = SDLTK_DEFAULT_WIN_HEIGHT;


  mPixelFormat = nullptr;

  //mFont = nullptr;

}

bool SdlApp::Window::Create(std::string title, bool fullscreen, int width, int height, int x, int y)
{
  Uint32 win_flags = SDL_WINDOW_SHOWN;
  if (x<0) x = SDL_WINDOWPOS_UNDEFINED;
  if (y<0) y = SDL_WINDOWPOS_UNDEFINED;
  if (fullscreen)
  {
    SDL_Rect screen_rect;
    SDL_GetDisplayBounds(0, &screen_rect);
    width = screen_rect.w;
    height = screen_rect.h;
    x = screen_rect.x;
    y = screen_rect.y;
    win_flags = SDL_WINDOW_SHOWN|SDL_WINDOW_FULLSCREEN|SDL_WINDOW_MOUSE_FOCUS|SDL_WINDOW_BORDERLESS;
  }

  if(title.empty()) title = ""; // ensure c_str() does not return a null pointer

  //Create window
  mSdlWin = SDL_CreateWindow(title.c_str(), x, y, width, height, win_flags);

  if (mSdlWin == nullptr)
  {
    printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  //Get window surface
  mSdlWinSurf = SDL_GetWindowSurface(mSdlWin);

  mPixelFormat = mSdlWinSurf->format;
  //mWinW = width;
  //mWinH = height;
  SDL_GetWindowSize(mSdlWin, &mWinW, &mWinH);
  return true;
}

void SdlApp::Window::Destroy()
{
  //Destroy window
  SDL_DestroyWindow( mSdlWin );
  mSdlWin = nullptr;
}


// SurfaceMatrix implementation
SdlApp::SurfaceMatrix::SurfaceMatrix(SDL_Surface* surf, int rows, int cols)
{
  mMatrixSurf = nullptr;
  mRows = 1;
  mCols = 1;
  SetSdlSurface(surf,rows,cols);
}

void SdlApp::SurfaceMatrix::SetSdlSurface(SDL_Surface* surf, int rows, int cols)
{
  if (cols<=0 || rows<=0) return;
  mMatrixSurf = surf;
  if (mMatrixSurf==nullptr)
  {
    mElemW=mElemH=mRows=mCols=0;
    return;
  }
  mElemW = mMatrixSurf->w/cols;
  mElemH = mMatrixSurf->h/rows;
}

void SdlApp::SurfaceMatrix::ApplySdlSurface(int row, int col, SDL_Surface * dst, int dst_x, int dst_y)
{
  if (mMatrixSurf==nullptr) return;

  SDL_Rect clip;
  clip.x = col * mElemW;
  clip.y = row * mElemH;
  clip.w = mElemW;
  clip.h = mElemH;

  SDL_Rect place;
  place.x = dst_x;
  place.y = dst_y;
  place.w = mElemW;
  place.h = mElemH;

  SDL_BlitSurface(mMatrixSurf, &clip, dst, &place);
}

// Text implementation
//-----------------------------------------------------------------------
SdlApp::Text::Text( ColorRGBA color, ColorRGBA shadow_color, int shadow_offset)
{
  mShadowOffset = shadow_offset;
  mTextSurf = nullptr;
  mTextShadowSurf = nullptr;
  mTextColor = color;
  mShadowColor = shadow_color;
  mTextChanged = false;
  mFont = nullptr;

 mHorizOffset = 0;
 mHorizAlign = LEFT;
 mVertOffset = 0;
 mVertAlign = TOP;
}


void SdlApp::Text::SetFont(TTF_Font* font)
{
  mTextChanged = (mFont!=font);
  mFont = font;
  //int line_spacing = TTF_FontLineSkip(font);
  //TTF_SetFontStyle(font,TTF_STYLE_BOLD);
}


void SdlApp::Text::SetAlignment(int horiz, int vert)
{
  mHorizAlign = horiz;
  mVertAlign = vert;
}

void SdlApp::Text::UpdateAlignment()
{
  if (mTextSurf==nullptr) return;

  switch (mHorizAlign)
  {
  case LEFT:
    mHorizOffset = 0;
    break;
  case CENTER:
    mHorizOffset = -mTextSurf->w/2;
    break;
  case RIGHT:
    mHorizOffset = -mTextSurf->w;
    break;
  }

  switch (mVertAlign)
  {
  case BOTTOM:
    mVertOffset = -mTextSurf->h;
    break;
  case MIDDLE:
    mVertOffset = -mTextSurf->h/2;
    break;
  case TOP:
    mVertOffset = 0;
    break;
  }
}


void SdlApp::Text::SetText(const std::string& text)
{
  mTextChanged = (mTextString!=text);
  mTextString = text;
}


int SdlApp::Text::TextHeight()
{
  if (mTextSurf==nullptr) return 0;
  return mTextSurf->h;
}

void SdlApp::Text::Update()
{
  if (!mTextChanged) return;
  if (mTextString.empty()) return;
  if (mFont==nullptr) return;

  if (mTextSurf!=nullptr) SDL_FreeSurface(mTextSurf);
  mTextSurf=nullptr;
  if (mTextShadowSurf!=nullptr) SDL_FreeSurface(mTextShadowSurf); 
  mTextShadowSurf=nullptr;

  SDL_Color text_color = {mTextColor.R, mTextColor.G, mTextColor.B, mTextColor.A};
	SDL_Color shadow_color = {mShadowColor.R, mShadowColor.G, mShadowColor.B, mShadowColor.A};
  mTextSurf = TTF_RenderText_Blended( mFont, mTextString.c_str(), text_color );//TTF_RenderText_Solid
	if (mTextSurf==nullptr)
	{
		printf( "Failed to render text!\n" );
	}
  if (mShadowColor.A>0)
  {
    mTextShadowSurf = TTF_RenderText_Blended( mFont, mTextString.c_str(), shadow_color );//TTF_RenderText_Solid
  }
  UpdateAlignment();
}


void SdlApp::Text::Render(SDL_Surface* surf, int x, int y)
{
  Update();
  if (mTextSurf==nullptr) return;

  x += mHorizOffset;
  y += mVertOffset;

  //Make a temporary rectangle to hold the offsets
  SDL_Rect offset;
  if (mTextShadowSurf!=nullptr)
  {
    offset.x = x+mShadowOffset;
    offset.y = y+mShadowOffset;
    SDL_BlitSurface(mTextShadowSurf, nullptr, surf, &offset);
    offset.x = x-mShadowOffset;
    offset.y = y+mShadowOffset;
    SDL_BlitSurface(mTextShadowSurf, nullptr, surf, &offset);
    offset.x = x+mShadowOffset;
    offset.y = y-mShadowOffset;
    SDL_BlitSurface(mTextShadowSurf, nullptr, surf, &offset);
    offset.x = x-mShadowOffset;
    offset.y = y-mShadowOffset;
    SDL_BlitSurface(mTextShadowSurf, nullptr, surf, &offset);
    offset.x = x+mShadowOffset;
    offset.y = y;
    SDL_BlitSurface(mTextShadowSurf, nullptr, surf, &offset);
    offset.x = x-mShadowOffset;
    offset.y = y;
    SDL_BlitSurface(mTextShadowSurf, nullptr, surf, &offset);
    offset.x = x;
    offset.y = y-mShadowOffset;
    SDL_BlitSurface(mTextShadowSurf, nullptr, surf, &offset);
    offset.x = x;
    offset.y = y+mShadowOffset;
    SDL_BlitSurface(mTextShadowSurf, nullptr, surf, &offset);
  }
  //Give the offsets to the rectangle
  offset.x = x;
  offset.y = y;

  SDL_BlitSurface(mTextSurf, nullptr, surf, &offset);
}


// SdlApp implementation
//-----------------------------------------------------------------------
int SdlApp::WinWidth()
{
  return mWin.mSdlWinSurf? mWin.mSdlWinSurf->w : 0;
}


int SdlApp::WinHeight()
{
  return mWin.mSdlWinSurf? mWin.mSdlWinSurf->h : 0;
}


bool SdlApp::InitSdl()
{
  //Initialize all SDL subsystems
  if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
  {
    return false;
  }

  //Set up the screen
  //Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
  {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return false;
  }

  //Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if ( !( IMG_Init( imgFlags ) & imgFlags ) )
	{
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
		return false;
	}

	if ( TTF_Init() == -1 )
	{
		printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
		return false;
	}
		//Initialize SDL_mixer
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
	{
		printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
		return false;
	}

  //Random number generator seed
  srand(SDL_GetTicks());

  return true;
}


SDL_Surface* SdlApp::CreateSdlSurface(int w, int h)
{
  return SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0, 0, 0, 0);
}


int SdlApp::GetSurfWidth(const std::string& name)
{
  if (name!="" && !ExistsSurface(name)) return -1;
  return mSurface[name]->w;
}


int SdlApp::GetSurfHeight(const std::string& name)
{
  if (name!="" && !ExistsSurface(name)) return -1;
  return mSurface[name]->h;
}


void SdlApp::DestroySdlSurface(SDL_Surface*& surf)
{
  if (surf==nullptr) return;
  SDL_FreeSurface(surf);
  surf = nullptr;
}


void SdlApp::DestroySurface(const std::string& name)
{
  if (!ExistsSurface(name)) return;
  SDL_FreeSurface(mSurface[name]);
  mSurface.erase(name);
}


void SdlApp::DestroySurfaceMatrix(const std::string& name)
{
  if (!ExistsSurfaceMatrix(name)) return;
  delete mSurfaceMatrix[name];
  mSurfaceMatrix.erase(name);
}


bool SdlApp::ApplySurf(int x, int y, const std::string& source, const std::string& destination)
{
  if (!ExistsSurface(source)) return false;
  if (destination!="" && !ExistsSurface(destination)) return false;
  SDL_Surface* dest_surf = mWin.mSdlWinSurf;
  if (destination!="") dest_surf = mSurface[destination];
  ApplySdlSurface( x,  y, mSurface[source], dest_surf);
  return true;
}


bool SdlApp::ExistsSurfaceMatrix(const std::string& name) const
{
  auto found = mSurfaceMatrix.find(name);
  // if a surface already exists with the given name return false<
  return found!=mSurfaceMatrix.end();
}


bool SdlApp::SetupSurfMatrix(const std::string& surf_mat, const std::string& filename, int rows, int cols)
{
  SDL_Surface* surf = LoadImageAsSurface(filename);
  if (surf==NULL) return false;
  if (!ExistsSurfaceMatrix(surf_mat))
  {
    mSurfaceMatrix[surf_mat] = new SurfaceMatrix(surf,rows,cols);
  }
  else
  {
    mSurfaceMatrix[surf_mat]->SetSdlSurface(surf,rows,cols);
  }
  return true;
}


bool SdlApp::ApplySurfMatrixElem(const std::string& surf_mat, int row, int col, int x, int y, const std::string& destination)
{
  if (!ExistsSurfaceMatrix(surf_mat)) return false;
  if (destination!="" && !ExistsSurface(destination)) return false;
  SDL_Surface* dest_surf = mWin.mSdlWinSurf;
  if (destination!="") dest_surf = mSurface[destination];
  mSurfaceMatrix[surf_mat]->ApplySdlSurface(row,col,dest_surf,x,dest_surf->h-y);
  return true;
}


bool SdlApp::GetSurfMatrixElemSize(const std::string& surf_mat, int& w, int& h)
{
  if (!ExistsSurfaceMatrix(surf_mat)) return false;
  w = mSurfaceMatrix[surf_mat]->GetElemW();
  h = mSurfaceMatrix[surf_mat]->GetElemH();
  return true;
}

void SdlApp::ApplySdlSurface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect *src_rect)
{
  if (destination==nullptr) destination = mWin.mSdlWinSurf;
  //Make a temporary rectangle to hold the offsets
  SDL_Rect offset;
  //Give the offsets to the rectangle
  offset.x = x;
  offset.y = destination->h - y - source->h;
  //Blit the surface
  SDL_BlitSurface(source, src_rect, destination, &offset);
}


bool SdlApp::TileSurf(int u, int v, const std::string& source, const std::string& destination)
{
  if (!ExistsSurface(source) || !ExistsSurface(destination)) return false;
  TileSdlSurface(u,v,mSurface[source], mSurface[destination]);
  return true;
}


bool SdlApp::NewSurface(const std::string& name, int width, int height)
{
  if (ExistsSurface(name) || name.empty()) return false;
  SDL_Surface* surf = CreateSdlSurface(width,height);
  if (surf==nullptr) return false;
  mSurface[name]=surf;
  return true;
}


void SdlApp::TileSdlSurface(int u, int v, SDL_Surface* source, SDL_Surface* destination)
{
  if (source==nullptr || destination==nullptr) return;
  while (u>0) u -= destination->w;
  while (v>0) v -= destination->h;
  for (int x = u; x<destination->w; x+=source->w)
  {
    for (int y = v; y<destination->h; y+=source->h)
    {
      ApplySdlSurface(x,y,source,destination);
    }
  }
}


bool SdlApp::ExistsText(const std::string& name)
{
  return mText.find(name)!=mText.end();
}

bool SdlApp::CreateText(
  const std::string& text_name,
  const std::string& font_name,
  const ColorRGBA& col,
  const ColorRGBA& shadowcol,
  int shadow_offset)
{
  if (!ExistsFont(font_name)) return false;
  if (ExistsText(text_name)) return false;

  mText[text_name]=std::move(Text(col,shadowcol,shadow_offset));
  mText[text_name].SetFont(mFont[font_name]);
  mText[text_name].SetAlignment(Text::LEFT,Text::BOTTOM);
  return true;
}

void SdlApp::SetText(const std::string& text_name, const std::string& text)
{
  if (!ExistsText(text_name)) return;
  mText[text_name].SetText(text);
}

void SdlApp::RenderText(const std::string& text_name, int x, int y, const std::string& dest_surface)
{
  if (dest_surface!="" && !ExistsSurface(dest_surface)) return;
  if (!ExistsText(text_name)) return;
  SDL_Surface* surf = nullptr;
  if (dest_surface!="") mSurface[dest_surface];
  else                  surf = mWin.mSdlWinSurf;
  Text& txt = mText[text_name];
  txt.Render(surf,x,surf->h-y);
}


void SdlApp::UpdateWindow()
{
  SDL_UpdateWindowSurface(mWin.mSdlWin);
}


void SdlApp::Delay(unsigned int ms)
{
  SDL_Delay(ms);
}


bool SdlApp::HandleEvents()
{
  SDL_Event evt;
  SDL_Delay(1);
  if (SDL_PollEvent(&evt))
  {
    if (mEventCB!=nullptr)
    {
      EventData evt_data = ConvertEvent(evt);
      return (*mEventCB)(evt_data);
    }
  }

  return false;
}



SDL_Surface* SdlApp::LoadImageAsSurface(const std::string& file_name)
{
  std::string file_path = mImagesPath + file_name;

  //Load image at specified path
  SDL_Surface* loaded_surface = IMG_Load(file_path.c_str());
  if (loaded_surface == nullptr)
  {
    printf("Unable to load image %s! SDL_image Error: %s\n", file_path.c_str(), IMG_GetError());
  }

  return loaded_surface;
}


bool SdlApp::ExistsSurface(const std::string& name) const
{
  auto found = mSurface.find(name);
  // if a surface already exists with the given name return false<
  return found!=mSurface.end();
}


bool SdlApp::DeleteSurface(const std::string& name)
{
  if (!ExistsSurface(name)) return false;
  DestroySdlSurface(mSurface[name]);
  mSurface.erase(name);
  return true;
}


bool SdlApp::LoadSurface(const std::string& name, const std::string& filename)
{
  // if an empty name is provided return false;
  if (name.empty()) return false;
  // if a surface already exists with the given name return false
  if (ExistsSurface(name)) return false;
  SDL_Surface* surf = LoadImageAsSurface(filename);
  if (surf==nullptr) return false;
  mSurface[name]=surf;
  return true;
}


bool SdlApp::ExistsFont(const std::string& name)
{
  return mFont.find(name)!=mFont.end();
}


bool SdlApp::LoadFont(const std::string& name, std::string path, int size)
{
  // if an empty name is provided return false;
  if (name.empty()) return false;
  // if a font already exists with the given name return false
  if (ExistsFont(name)) return false;
  if (path.empty()) path = name+".ttf";

  std::string file_path = mFontsPath + path;
  TTF_Font* font = TTF_OpenFont(file_path.c_str(), size);
  if (font==nullptr)
  {
		printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
    return false;
  }
  mFont[name]=font;
  return true;
}



bool SdlApp::LoadSound(const std::string& name, std::string path)
{
  if (path.empty()) path = name+".wav";
  if (!mSoundPath.empty()) path = mSoundPath+path;

  Mix_Chunk * snd = Mix_LoadWAV( path.c_str() );
  if (snd==nullptr) return false;
  mSoundFx[name] = snd;
  return true;
}


void SdlApp::PlaySound(const std::string& name)
{
  if (mAudioEnabled) Mix_PlayChannel( -1, mSoundFx[name], 0 );
}


void SdlApp::SetSoundsVolume(int vol)
{
  Mix_Volume( -1,vol );
  //for (auto snd_itr=mSoundFx.begin(); snd_itr!=mSoundFx.end(); ++snd_itr)
  //{
  //  Mix_VolumeChunk( snd_itr->second,vol );
  //}
}


int SdlApp::GetSoundsVolume(int vol)
{
  if (mSoundFx.empty()) return 0;
  return Mix_VolumeChunk( mSoundFx.begin()->second,-1 );
}


bool SdlApp::LoadMusic(const std::string& name, std::string path)
{
  if (path.empty()) path = name+".wav";
  if (!mMusicPath.empty()) path = mMusicPath+path;

  Mix_Music* mus = Mix_LoadMUS( path.c_str() );
  if (mus==nullptr) return false;
	mPlaylist.push_back( mus );

  return true;
}

void SdlApp::PlayMusic(unsigned int index)
{
  if (!mAudioEnabled || mPlaylist.empty() || index>=mPlaylist.size()) return;

  mMusic = index;

  if( Mix_PlayingMusic() == 0 )
	{
    //Play the music
		Mix_PlayMusic( mPlaylist[mMusic], -1 );
	}
}


void SdlApp::PlayMusic()
{
  if (!mAudioEnabled || mPlaylist.empty()) return;
  if( Mix_PausedMusic() != 0 )
	{
    Mix_ResumeMusic();
    return;
  }
  if( Mix_PlayingMusic() == 0 )
	{
		//Play the music
		Mix_PlayMusic( mPlaylist[mMusic], -1 );
		//Select the next music
    ++mMusic;
    if (mMusic>=mPlaylist.size()) mMusic = 0;
	}
}


void SdlApp::SetMusicVolume(int vol)
{
  Mix_VolumeMusic( vol );
}


int SdlApp::GetMusicVolume()
{
  return Mix_VolumeMusic( -1 );
}


void SdlApp::SwitchAudio(bool on)
{
  if (!on && Mix_PlayingMusic()) Mix_PauseMusic();

  mAudioEnabled=on;
  if (on) PlayMusic();
}


#pragma message ("\nPlease note that there is still a non critical issue to be solved:\n"\
  "when you link this library you get some warnings (LNK4006,LNK4221)\n")
