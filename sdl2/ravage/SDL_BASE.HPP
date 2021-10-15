
#ifndef AH_SDL_BASE
#define AH_SDL_BASE

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "XLIB.HPP"
#include "ERROR.HPP"
#include <map>

using namespace std;

extern class SDLWindow window;
extern class SDLRenderer render;

extern class SDLSpritePool sPool;
extern class SDLSpritePool shPool;
extern class SDLSpritePool sfPool;
extern class SDLTilePool tPool;

//Game window class
class SDLWindow {
	SDL_Window *window;
	bool mouseFocus;
	bool inputFocus;

public:
	SDLWindow() {
		window = NULL;
		mouseFocus = false;
		inputFocus = false;
	}

	~SDLWindow() { if(window) error("Window not released correctly"); }

	int create(const char *title, int width, int height);

	int destroy(void)
	{
		if(window)
		{
			SDL_DestroyWindow(window);
			window = NULL;
		}


		return 0;
	}

	SDL_Surface *getScreen(void) { return SDL_GetWindowSurface(window); }

	SDL_Window *getWindow() { return window; };
	bool isFocus() { return (inputFocus && mouseFocus); }; // window is in focus if both mouse and input is in focus
	void enableInputFocus(void) { inputFocus = true; }
	void disableInputFocus(void) { inputFocus = false; }
	void enableMouseFocus(void) { mouseFocus = true; }
	void disableMouseFocus(void) {mouseFocus = false; }
};

class SDLTexture {
	SDL_Texture *tex;
	int width;
	int height;

	bool freeTexture;

	void setup(bool reqFree);		//Fill in description attributes

public:
	SDLTexture() { tex = NULL; }
	~SDLTexture() {
        destroy();
	}

    int create(int width, int height, Uint32 pixelFormat);

	int loadFile(char *fileName);
	int loadMemory(void *memory, int memSize);

	int loadPointer(SDL_Texture *texture, bool reqFree) {
		tex = texture;
		setup(reqFree);

		return 0;
	}

	int getWidth(void) { return width; }
	int getHeight(void) { return height; }
	SDL_Texture *getBase(void) { return tex; }

	int setAlpha(int a)
	{
		if(a > SDL_ALPHA_OPAQUE) a = SDL_ALPHA_OPAQUE;
		if(a < SDL_ALPHA_TRANSPARENT) a = SDL_ALPHA_TRANSPARENT;
		return SDL_SetTextureAlphaMod(tex, a);
	}

	void destroy(void) {
		if(freeTexture == true && tex != NULL) 
		{
			SDL_DestroyTexture(tex);
		}
		tex = NULL;
	}
};

//Sprite class to replace functionality of the previous Sprite structure
class SDLSprite {
	Sprite *sprite;
	bool converted;
	bool owner; // The class looks after the Sprite structure's memory
	SDLTexture texture;
	int numFrames;

public:
	SDLSprite() { sprite = NULL; converted = false; owner = false;}
	~SDLSprite() { if(sprite) error("Sprite not released correctly"); }
	void copy(SDLSprite *cSprite);

	void load(Sprite *sp, bool isOwner);
	void loadFlash(Sprite *sp, bool isOwner);
	void convert(void);
	void convertFlash(void);

	Sprite *getBase() { return sprite; }
	SDLTexture *getTexture(void) { return &texture; }

	SDL_Rect getFrameRect(int n);

	int getNumFrames(void) { return numFrames; }

	bool shouldDraw(void) { return converted; }

	void destroy(void) {
		texture.destroy();

		if(owner)
			free(sprite);

		sprite = NULL;
	}
};

class SDLSpritePool
{
    int numSprites;
    std::map<Sprite *, SDLSprite *> sprites;

public:
    SDLSpritePool() { numSprites = 0; }
    ~SDLSpritePool() { if(numSprites > 0) error("SDL Sprite Pool not destroyed properly");}

    void destroy();

    bool inPool(Sprite *spriteAddress)
    {
        return sprites.count(spriteAddress) > 0 ? true : false;
    }

    bool inPool(SDLSprite *check)
    {
		  if(check == NULL) return false;
        return (sprites.count(check->getBase()) == 1) && (sprites[check->getBase()] == check) ? true : false;
    }

    //Return pointer if in pool
    SDLSprite *get(Sprite *spriteAddress)
    {
        return sprites.count(spriteAddress) > 0 ? sprites[spriteAddress] : NULL;
    }

    //Return copy of sprite if in pool
    SDLSprite *getCopy(Sprite *spriteAddress);

    int add(SDLSprite *sSprite, Sprite *spriteAddress);
};

void destroy_SpritePools(void);


class SDLTile {
	SDLTexture tex;
	long *id;
	bool assigned;

public:

	SDLTile() {
		assigned = false;
		id = NULL;
	}

	~SDLTile() {
		if(assigned) error("Tile not released properly");
	}

	SDLTexture *getBase() { return &tex; }

	void load(SDL_Texture *tileTexture, bool owner, long *addressID)
	{
		id = addressID;
		tex.loadPointer(tileTexture, owner);

		if(tileTexture != NULL) assigned = true;

		return;
	}

	long *getID() { return id; }

	bool drawable()
	{
		return assigned;
	}

	void destroy()
	{
		id = NULL;
		tex.destroy();
		assigned = false;
		return;
	}

};

class SDLTilePool
{
    int numTiles;
    std::map<long *, SDLTile *> tiles;

public:
    SDLTilePool() { numTiles = 0; }
    ~SDLTilePool() { if(numTiles > 0) error("SDL Tile Pool not destroyed properly");}

    void destroy();

    bool inPool(long *TileAddress)
    {
        return tiles.count(TileAddress) > 0 ? true : false;
    }

    bool inPool(SDLTile *check)
    {
		  if(check == NULL) return false;
        return (tiles.count(check->getID()) == 1) && (tiles[check->getID()] == check) ? true : false;
    }

    //Return pointer if in pool
    SDLTile *get(long *TileAddress)
    {
        return tiles.count(TileAddress) > 0 ? tiles[TileAddress] : NULL;
    }


    int add(SDLTile *tile, long *tileAddress);
};

class SDLMapLayer {

		int numTiles;
		int length;
		int pixLength;
		bool shadows;
		SDLTile **tiles;

public:

	SDLMapLayer() {
		numTiles = 0;
		tiles = NULL;
	}

	void init(int nTiles, int pLength, bool shadowEnabled)
	{
		if(tiles)
			destroy();

		numTiles = nTiles;
		length = (nTiles/XTILES)*TILEYS - YMAX;
		pixLength = pLength;
		shadows = shadowEnabled;

		tiles = new SDLTile * [numTiles];
	}

	~SDLMapLayer() {
		destroy();
	}

	SDLTile *createTile(int arrayIndex, long *tileData);

	void convert(SDLTile *tile, long *tileData); //convert the pixels from tileData to a texture in tile.

	SDLTile *getTile(int x, int y)
	{
		return *(tiles + x + (XTILES * y));
	}

	SDLTile *getTile(int i)
	{
		return *(tiles + i);
	}

	int getSize()
	{
		return numTiles;
	}

	int getLength()
	{
		return length;
	}

	int getPixelLength()
	{
		return pixLength;
	}

	void destroy()
	{
		SDLTile *tile = NULL;
		int x = 0;

		for(x = 0; x < numTiles; x++)
		{
			tile = *(tiles+x);

			if(tile != NULL && !tPool.inPool(tile))
            {
                tile->destroy();
                delete tile;
            }
		}

		if(tiles)
		{
			delete[] tiles;
		}

		tiles = NULL;
		numTiles = 0;
	}

};


//Drawing functions
class SDLRenderer {
	SDL_Renderer *renderer;
	SDLTexture renderTexture; //Render onto a texture first before rendering to the screen
	SDL_Rect viewport;		//Used to track whether the view is stretched
	float xRatio;				//x and y ratios between original display dimensions and viewport
	float yRatio;				
	SDLTexture fadeTexture; //Black fading texture

	
	void resetRenderTexture(void); //Create a new target texture to render to. Used to update stretching.

public:

	SDLRenderer() {
		renderer = NULL;
		xRatio = 1.0f;
		yRatio = 1.0f;
		viewport.x = viewport.y = 0;
		viewport.w = HWXMAX;
		viewport.h = HWYMAX;
	}

	~SDLRenderer() {
		if(renderer) error("Renderer not released correctly");
	}

	void init(SDLWindow *window, int logicalWidth, int logicalHeight);

	void stretch(int width, int height);

	float getXRatio(void)
	{
		return xRatio;
	}

	float getYRatio(void)
	{
		return yRatio;
	}

	int getViewportX(void)
	{
		return viewport.x;
	}

	int getViewportY(void)
	{
		return viewport.y;
	}

	void setLinearScaling(bool enable);

	SDLTexture *createScreenTexture();

	SDLTexture *getScreenTexture()
	{
		return &renderTexture;
	}


	void x_drawsprite(SDLSprite *sprite, int x, int y, int n); // n refers to the frame of the sprite
	void drawTile(SDLTile *tile, int x, int y);
	void drawTile(SDLTile *tile, int x, int y, const SDL_Rect *clip);
	void drawMap(SDLMapLayer *map, int yOffset);

	void drawRect(SDL_Rect *rect, int r, int g, int b);

	void drawTexture(SDLTexture *texture, int x, int y, SDL_Rect *clip);
	void drawTexture(SDLTexture *texture, int x, int y, int cx, int cy, int cw, int ch);
	void drawTexture(SDLTexture *texture, int x, int y);

	void fade(SDLTexture *background, int a);

	SDL_Renderer *getBase(void) { return renderer; }

	void update(void);

	void setTarget(SDLTexture *target) {
		if(target == NULL) SDL_SetRenderTarget(renderer, renderTexture.getBase());
		else SDL_SetRenderTarget(renderer, target->getBase());

		return;
	}

	void clear(void) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
	}

	void clear(Uint8 r, Uint8 g, Uint8 b) {
		SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
	}

	void destroy(void)
	{
		if(renderer)
		{
			renderTexture.destroy();
			fadeTexture.destroy();

			SDL_DestroyRenderer(renderer);
			renderer = NULL;
		}
	}
};

class Font {
   long     *anchor;          // Pointer to all data.
   long     *xs;              // Pointer to xs array.(proportional schrift)

public:
	Sprite   *sprite;          // Pointer to font sprite array.
	SDLSprite fontSprite;		// SDL version of the sprite
   Font(const char *file);
   ~Font();
   void     print(int x, int y, const char *text);
   void     print_c(int x, int y, const char *text);
   void     vanilla(int x, int y, const char *text, int c);
   void     vanilla_c(int x, int y, const char *text, int c);
   void     vanilla_char(int x, int y, char ch, int c);
   int      textlen(const char *text);
   int      charlen(char ch)
   {
      return xs[ch - anchor[FD_BASE]] + anchor[FD_PITCH];
   }
   int      width(void) { return sprite->xs; }
   int      height(void) { return sprite->ys; }
};

void beauty(Font *, int &, int &, char *);

// Background class.
// This class is intended to help building small animations like
// cursors or mouse pointers.
#pragma pack(4)
class Back {
   int      valid;
   int      x, y;             // Coordinates of saved background area to update.
   int      xs, ys;           // Size of background area.
   SDL_Rect clip;
   SDLTexture background;

public:
   Back(int dxs, int dys);
   ~Back();
   void     save(int x, int y);
   void     restore();
   void     invalidate(void) { valid = 0; }
};


#endif


