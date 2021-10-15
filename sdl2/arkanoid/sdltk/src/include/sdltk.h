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
/// @brief SDL Toolkit
/// @file sdltk.h
/// @author Giovanni Paolo Vigano'

#pragma once
#ifndef SDLTK_H
#define SDLTK_H

#include <string>

#include <string>
#include <vector>
#include <map>



// forward declaration for SDL data
struct SDL_Surface;
struct SDL_Window;
struct SDL_PixelFormat;
struct SDL_Rect;
struct SDL_Color;
struct SDL_Renderer;
struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;
struct Mix_Chunk;
struct _Mix_Music;
typedef struct _Mix_Music Mix_Music;

namespace sdltk
{
  /// @addtogroup sdltk
  /// @{

  /// Event type (see EventData)
  enum EventType
  {
    UNDEFINED,  ///< Event not defined
    APP,        ///< Application event
    MOUSE,      ///< Mouse event
    KEY,        ///< Keyboard event
  };

  /// Event action type (see EventData)
  enum EventAction
  {
    NONE,      ///< No action
    DOWN,      ///< Button/key down
    UP,        ///< Button/key up
    MOVE,      ///< Mouse move
    QUIT,      ///< Application quit
  };

  /// Mouse/keyboard button (see EventData)
  enum Button
  {
    LEFT=0,    ///< Mouse left button
    MIDDLE=1,  ///< Mouse middle button
    RIGHT=2,   ///< Mouse right button

    ESC=27,    ///< Escape key (ASCII code)
    ENTER=13,  ///< Enter key  (ASCII code)
    DEL=127,   ///< Delete key (ASCII code)
    BACK=8,    ///< Backspace key (ASCII code)
    TAB=9,     ///< Tabulation key (ASCII code)
  };


  /// Coordinates at which the event occurred (see EventData)
  struct Coords
  {
    int X;
    int Y;
    Coords()
    {
      X=Y=0;
    }
  };

  /// Event data structure
  struct EventData
  {
    /// Event type (see EventType)
    EventType Type;

    /// Coordinates at which the event occurred (see Point)
    Coords Point;

    /// Mouse/keyboard button (see Button)
    int Button;

    /// Event action type (see EventAction)
    EventAction Action;

    /// Constructor: set class attributes to default values
    EventData()
    {
      Type=UNDEFINED;
      Button=-1;
      Action=NONE;
    }
  };

  /// Functor class for events handling
  class EventFunctor
  {
  public:
    /// Functor operator
    virtual bool operator ()(const sdltk::EventData& evt_data) = 0;
  };

  /// Utility timer (e.g. for dynamics calculations)
  class Timer
  {
  private:
    unsigned int mStartMs;

    bool mStarted;

  public:
    /// Constructor
    Timer();

    /// Start the timer
    void Start();

    /// Stop the timer
    void Stop();

    /// Get the elapsed time in milliseconds
    unsigned int GetTimeMs();
  };

  /// Color data structure (red, green, blue, alpha=opacity)
  struct ColorRGBA
  {
    unsigned char R; ///< Red component (0..255)
    unsigned char G; ///< Green component (0..255)
    unsigned char B; ///< Blue component (0..255)
    unsigned char A; ///< Alpha (opacity) component (0..255)

    /// Constructor (all parameters are optional, default value is 0,0,0,0)
    /// @param r Red component (0..255)
    /// @param g Green component (0..255)
    /// @param b Blue component (0..255)
    /// @param a Alpha (opacity) component (0..255)
    ColorRGBA(unsigned char r=0, unsigned char g=0, unsigned char b=0, unsigned char a=0)
    {
      R=r;
      G=g;
      B=b;
      A=a;
    }
  };

  /// SDL application manager
  class SdlApp
  {

    // these classes are defined here, inside the privatre section of SdlApp
    // to hide the internal implementation for external users of this class

    class Window
    {
    public:
      Window();
      SDL_Window* mSdlWin;
      SDL_Surface *mSdlWinSurf;
      SDL_PixelFormat *mPixelFormat;
      int mWinPosX;
      int mWinPosY;
      int mWinW;
      int mWinH;
      bool Create(std::string title, bool fullscreen, int width, int height, int x=-1, int y=-1);
      void Destroy();
    };


    class SurfaceMatrix
    {
      SDL_Surface* mMatrixSurf;
      int mElemW;
      int mElemH;
      int mRows;
      int mCols;
      void UpdateClips();
    public:
      SurfaceMatrix(SDL_Surface* surf=nullptr, int rows=0, int cols=0);
      void SetSdlSurface(SDL_Surface* surf, int row, int col);
      void ApplySdlSurface(int row, int col, SDL_Surface * dst, int dst_x, int dst_y);
      int GetElemW() { return mElemW; }
      int GetElemH() { return mElemH; }
    };


    class Text
    {
      bool mTextChanged;
      int mShadowOffset;
      SDL_Surface* mTextSurf;
      SDL_Surface* mTextShadowSurf;
      TTF_Font* mFont;
      ColorRGBA mTextColor;
      ColorRGBA mShadowColor;
      std::string mTextString;
      int mHorizOffset;
      int mHorizAlign;
      int mVertOffset;
      int mVertAlign;
      void Update();
      void UpdateAlignment();
    public:
      enum {LEFT=-1,CENTER=0,RIGHT=1};
      enum {TOP=-1,MIDDLE=0,BOTTOM=1};
      Text( ColorRGBA color=ColorRGBA(255,255,255,255), ColorRGBA shadow_color=ColorRGBA(), int shadow_offset=1 );
      void Render(SDL_Surface* surf, int x, int y);
      void SetText(const std::string& text);
      void SetFont(TTF_Font* font);
      void SetAlignment(int horiz, int vert);
      int TextHeight();
    };
  public:

    /// Constructor: just initialize data members
    SdlApp();

    /// Just trace the deletion of this message with a message
    virtual ~SdlApp();

    /// Create teh application window
    bool CreateWindow(std::string title, bool fullscreen=true, int width=-1, int height=-1, int x=-1, int y=-1);

    /// Redraw window surface
    void UpdateWindow();

    /// Handle events (mouse, keyboard)
    bool HandleEvents();

    /// Connect the events with the given handler (@see HandleEvents())
    /// @param func handler for events (@see EventFunctor)
    /// @param auto_delete automatically delete the given functor along with the application
    void SetEventFunc(EventFunctor* func, bool auto_delete=true)
    {
      mEventCB = func;
      mAutoDeleteEventCB = auto_delete;
    }

    /// Access (read/change) the path containing the font files
    std::string& FontsPath() { return mFontsPath; }

    /// Access (read/change) the path containing the image files
    std::string& ImagesPath() { return mImagesPath; }

    /// Access (read/change) the path containing the sound files
    std::string& SoundsPath() { return mSoundPath; }

    /// Access (read/change) the path containing the music files
    std::string& MusicPath() { return mMusicPath; }

    /// Get the width of the window surface
    int WinWidth();

    /// Get the height of the window surface
    int WinHeight();

    /// Initialize SDL library
    bool InitSdl();

    /// Destroy all and quit SDL library
    void ExitSdl();

    /// Check if a named surface exists
    bool ExistsSurface(const std::string& name) const;

    /// Create a new surface with the given name and size
    /// @return Return true on success, false on error
    bool NewSurface(const std::string& name, int width, int height);

    /// Delete an existing surface
    /// @return Return true on success, false the named surface does't exist
    bool DeleteSurface(const std::string& name);

    /// Create a surface loaded from a file
    /// @param name identifier that will be used for the new surface
    /// @param filename name of the file to load (@see ImagesPath())
    bool LoadSurface(const std::string& name, const std::string& filename);

    /// Get the width of the named surface
    int GetSurfWidth(const std::string& name);

    /// Get the width of the named surface
    int GetSurfHeight(const std::string& name);

    /// Apply at the given coordinates the named surface
    /// @param x horizontal coordinate in pixels
    /// @param y vertical coordinate in pixels
    /// @param source name of the surface to be copied
    /// @param destination name of the surface where to copy (if not specified it is the main window)
    /// @return Return true on success, false if something went wrong
    bool ApplySurf(int x, int y, const std::string& source, const std::string& destination="");

    /// Fill a surface using the given surface as tile
    /// @param u horizontal offset of the tiles
    /// @param v vertical offset of the tiles
    /// @param source name of the surface used as tile
    /// @param destination name of the surface that must be filled
    bool TileSurf(int u, int v, const std::string& source, const std::string& destination);

    /// Destroy a previously created surface
    void DestroySurface(const std::string& name);

    /// Create or setup a surface matrix using an image table stored in the given file
    /// @param surf_mat name of the surface matrix to create or update
    /// @param filename name of the file to load (@see ImagesPath())
    /// @param rows number of rows in the loaded image (vertical subdivision)
    /// @param cols number of columns in the loaded image (horizontal subdivision)
    /// @return Return true on success, false the named file can't be loaded
    bool SetupSurfMatrix(const std::string& surf_mat, const std::string& filename, int rows, int cols);

    /// Apply a matrix element (sub-image) of the named surface matrix
    /// @param surf_mat name of the surface matrix used
    /// @param row,col row and column of the matrix element to use
    /// @param x,y where to appy the element (horizontal and vertical position in pixels)
    /// @param destination name of the surface where to copy (if not specified it is the main window)
    /// @return Return true on success, false the named surface does't exist
    bool ApplySurfMatrixElem(const std::string& surf_mat, int row, int col, int x, int y, const std::string& destination="");

    /// Get the size of an element of the named matrix surface
    /// @param surf_mat name of the surface matrix
    /// @param[out] w width of the element (not changed if the matrix was not found)
    /// @param[out] h height of the element (not changed if the matrix was not found)
    /// @return Return true on success, false the named surface does't exist
    bool GetSurfMatrixElemSize(const std::string& surf_mat, int& w, int& h);

    /// Check if a named surface matrix exists
    bool ExistsSurfaceMatrix(const std::string& name) const;

    /// Destroy a previously created surface matrix
    void DestroySurfaceMatrix(const std::string& name);

    // Check if a font with the given name was created
    bool ExistsFont(const std::string& name);

    /// Load a font
    /// @param name name used to identify the font
    /// @param path file path of the true type font to load
    /// @param size chosen size for the font
    /// @return Return true on success, false on error
    bool LoadFont(const std::string& name, std::string path, int size);

    // Check if a text with the given name was created
    bool ExistsText(const std::string& name);

    /// Create a new text (object)
    /// @param text_name name used to identify the text
    /// @param font_name name of the font used
    /// @param col color of the text
    /// @param shadowcol (optional) color of the shadow (default = no shadow)
    /// @param shadow_offset offset of the shadow in pixels
    /// @return Return true on success, false on error
    bool CreateText(
      const std::string& text_name,
      const std::string& font_name,
      const ColorRGBA& col,
      const ColorRGBA& shadowcol=ColorRGBA(),
      int shadow_offset=1);

    /// Set the text content for the named text
    void SetText(const std::string& text_name, const std::string& text);

    /// Render the named text
    /// @param text_name name of the text to be rendered (not the text string itself)
    /// @param x,y coordinates where to render the text
    /// @param dest_surface (optional) destination surface (default = main window)
    void RenderText(const std::string& text_name, int x, int y, const std::string& dest_surface="");

    /// Load a named sound from a WAV file
    /// @return Return true on success, false on error
    bool LoadSound(const std::string& name, std::string path="");

    /// Play the named sound
    void PlaySound(const std::string& name);

    /// Set the volume for sounds (0 to 128)
    void SetSoundsVolume(int vol);

    /// Get the volume for sounds (0 to 128)
    int GetSoundsVolume(int vol);

    /// Load a named music from a WAV file
    /// @return Return true on success, false on error
    bool LoadMusic(const std::string& name, std::string path="");

    /// Play the music with the given index
    void PlayMusic(unsigned int index);

    /// Play the music (start playing, continue, cycle thruogh the playlist)
    void PlayMusic();

    /// Set the music volume (0 to 128)
    void SetMusicVolume(int vol);

    /// Get the music volume (0 to 128)
    int GetMusicVolume();

    /// Switch the audio on (true) or off (false)
    void SwitchAudio(bool on);

    /// Check if the audio is on (true) or off (false)
    bool AudioEnabled() { return mAudioEnabled; }

    /// Delay (suspend) the execution for the given milliseconds
    void Delay(unsigned int ms);

  protected:

    // internal window data

    Window mWin;

    // paths for loading resource files

    std::string mFontsPath;
    std::string mImagesPath;
    std::string mSoundPath;
    std::string mMusicPath;

    // Audio (music/sound) enabling flag

    bool mAudioEnabled;

    // Fonts
    std::map<std::string,TTF_Font*> mFont;

    // Images
    std::map<std::string,SDL_Surface*> mSurface;

    // Image matrices (tables of images)
    std::map<std::string,SurfaceMatrix*> mSurfaceMatrix;

    // Sounds
    std::map<std::string,Mix_Chunk*> mSoundFx;

    // Music
    unsigned mMusic;
    std::vector<Mix_Music*> mPlaylist;

    // Text
    std::map<std::string,Text> mText;

    // reference to the external event manager

    EventFunctor* mEventCB;
    bool mAutoDeleteEventCB;

    // utility methods

    SDL_Surface* CreateSdlSurface(int w, int h);
    SDL_Surface* LoadImageAsSurface(const std::string& filename);
    void ApplySdlSurface(int x, int y, SDL_Surface* source, SDL_Surface* destination=nullptr, SDL_Rect *src_rect=nullptr);
    void TileSdlSurface(int u, int v, SDL_Surface* source, SDL_Surface* destination);
    void DestroySdlSurface(SDL_Surface*& surf);
  };

  /// @}

}//namespace sdltk


#endif//SDLTK_H
