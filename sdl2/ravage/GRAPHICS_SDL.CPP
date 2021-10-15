#include <stdio.h>
#include <math.h>
#include "COMPATIBILITY.HPP"
#include "SDL_BASE.HPP"
#include "ERROR.HPP"

SDLWindow window;
SDLRenderer render;

SDLSpritePool sPool;
SDLSpritePool shPool; //duplicate shadow pool
SDLSpritePool sfPool; //corresponding sprites when flashed

SDLTilePool tPool;

void destroy_SpritePools(void)
{
	sPool.destroy();
	shPool.destroy();
   sfPool.destroy();
   tPool.destroy();

	return;
}


/*---------------------------------------------------------
SDLSpriteSpool class methods
---------------------------------------------------------*/
int SDLSpritePool::add(SDLSprite *sSprite, Sprite *spriteAddress)
{
    if(spriteAddress != NULL && inPool(spriteAddress) == false)
    {
       sprites[spriteAddress] = sSprite;
		 numSprites ++;

		 return 1;
    }

    return 0;
}

//Instead of returning a pointer, this will return a new sprite which is a copy.
SDLSprite *SDLSpritePool::getCopy(Sprite *spriteAddress)
{
    SDLSprite *sSprite = NULL;

    if(inPool(spriteAddress) == false)
    {
        return NULL;
    }
    else
    {
        sSprite = new SDLSprite;
        sSprite->copy(sPool.get(spriteAddress));
    }

    return sSprite;
}

void SDLSpritePool::destroy()
{
   std::map<Sprite *, SDLSprite *>::iterator it;

   for (it = sprites.begin(); it != sprites.end(); ++it)
   {
      it->second->destroy();
      delete it->second;

      numSprites --;
   }

   sprites.clear();
   return;
}

/*---------------------------------------------------------
SDLSprite class methods
---------------------------------------------------------*/

void SDLSprite::load(Sprite *sp, bool isOwner)
{
	if(sprite) destroy();

	sprite = sp;
	owner = isOwner;

	numFrames = sprite->frames;

	convert();
}

void SDLSprite::loadFlash(Sprite *sp, bool isOwner)
{
	if(sprite) destroy();

	sprite = sp;
	owner = isOwner;

	numFrames = sprite->frames;

	convertFlash();
}

void SDLSprite::copy(SDLSprite *cSprite)
{
    SDLTexture *cTex = cSprite->getTexture();

    if(cSprite->sprite != NULL)
    {
        texture.create(cTex->getWidth(), cTex->getHeight(), SDL_PIXELFORMAT_RGB888);
        SDL_SetTextureBlendMode(texture.getBase(), SDL_BLENDMODE_BLEND);

        render.setTarget(&texture);
        //SDL_SetRenderDrawColor(render.getBase(), 0, 0, 0, SDL_ALPHA_OPAQUE/2);
        SDL_SetTextureBlendMode(cTex->getBase(), SDL_BLENDMODE_NONE);
        render.drawTexture(cTex, 0, 0);
        SDL_SetTextureBlendMode(cTex->getBase(), SDL_BLENDMODE_BLEND);
        render.setTarget(NULL);
    }


    sprite = cSprite->sprite;
    converted = cSprite->converted;
    owner = false;
    numFrames = cSprite->numFrames;

    return;
}

void SDLSprite::convert(void)
{
	int x, y, mod;
	char *bytePix = NULL;
	Uint32 *surfacePix = NULL;
	unsigned char pix;
	SDL_Surface *surface = NULL;

	if(!sprite)
	{
		converted = false;
		return;
	}

	//Create a blank surface
	surface = SDL_CreateRGBSurface(0, sprite->xs, sprite->ys_data * sprite->frames, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));


	//Prepare for direct pixel access
	SDL_LockSurface(surface);

	bytePix = (char *)sprite->data;
	surfacePix = (Uint32 *)surface->pixels;

	for(y = 0; y < sprite->ys_data * sprite->frames; y++) //For each row of the sprite map
	{
		for(x = 0; x < sprite->xs; x++) //For each column up to the width of the sprite
		{
			mod = x % 4; //Find the plane the requested pixel (x,y) is in

			pix = *(bytePix + (y*(sprite->xs_data) + ((sprite->xs_data*mod)/4) + (x/4))); //Get the pixel index colour from the correct address

			surfacePix[x + (y * sprite->xs)] = SDL_MapRGB(surface->format, palette.sdl_rgb[pix].r, palette.sdl_rgb[pix].g, palette.sdl_rgb[pix].b);
		}
	}

	SDL_UnlockSurface(surface);

	//Use first colour of palette as colour key.
	SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, palette.sdl_rgb[0].r, palette.sdl_rgb[0].g, palette.sdl_rgb[0].b));

	//Convert the surface into a texture and load it into the class
	texture.loadPointer(SDL_CreateTextureFromSurface(render.getBase(), surface), true);
   SDL_SetTextureBlendMode(texture.getBase(), SDL_BLENDMODE_BLEND);

	//Clean up
	SDL_FreeSurface(surface);

	converted = true;

	return;
}

void SDLSprite::convertFlash(void)
{
	int x, y, mod;
	char *bytePix = NULL;
	Uint32 *surfacePix = NULL;
	unsigned char pix;
	SDL_Surface *surface = NULL;
	SDL_Color colour;
	const Uint8 flashAmount = 80;

	if(!sprite)
	{
		converted = false;
		return;
	}

	//Create a blank surface
	surface = SDL_CreateRGBSurface(0, sprite->xs, sprite->ys_data * sprite->frames, 32, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));


	//Prepare for direct pixel access
	SDL_LockSurface(surface);

	bytePix = (char *)sprite->data;
	surfacePix = (Uint32 *)surface->pixels;

	for(y = 0; y < sprite->ys_data * sprite->frames; y++) //For each row of the sprite map
	{
		for(x = 0; x < sprite->xs; x++) //For each column up to the width of the sprite
		{
			mod = x % 4; //Find the plane the requested pixel (x,y) is in

			pix = *(bytePix + (y*(sprite->xs_data) + ((sprite->xs_data*mod)/4) + (x/4))); //Get the pixel index colour from the correct address

            colour.r = palette.sdl_rgb[pix].r;
            colour.g = palette.sdl_rgb[pix].g;
            colour.b = palette.sdl_rgb[pix].b;

            if(pix != 0)
            {
                if(255 - colour.r < flashAmount) colour.r = 255;
                else colour.r += flashAmount;

                if(255 - colour.g < flashAmount) colour.g = 255;
                else colour.g += flashAmount;

                if(255 - colour.b < flashAmount) colour.b = 255;
                else colour.b += flashAmount;
            }

            surfacePix[x + (y * sprite->xs)] = SDL_MapRGB(surface->format, colour.r, colour.g, colour.b);
		}
	}

	SDL_UnlockSurface(surface);

	//Use first colour of palette as colour key.
	SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, palette.sdl_rgb[0].r, palette.sdl_rgb[0].g, palette.sdl_rgb[0].b));

	//Convert the surface into a texture and load it into the class
	texture.loadPointer(SDL_CreateTextureFromSurface(render.getBase(), surface), true);
   SDL_SetTextureBlendMode(texture.getBase(), SDL_BLENDMODE_BLEND);

	//Clean up
	SDL_FreeSurface(surface);

	converted = true;

	return;
}

SDL_Rect SDLSprite::getFrameRect(int n)
{
	SDL_Rect frame;

	frame.x = 0;
	frame.y = n*sprite->ys;
	frame.w = texture.getWidth();
	frame.h = sprite->ys;

	return frame;
}

/*---------------------------------------------------------
SDLTexture class methods
---------------------------------------------------------*/

void SDLTexture::setup(bool reqFree)
{
	if(tex != NULL) SDL_QueryTexture(tex, NULL, NULL, &width, &height);
	else
	{
		width = 0;
		height = 0;
	}

	freeTexture = reqFree;

	return;
}

int SDLTexture::create(int width, int height, Uint32 pixelFormat)
{
    tex = SDL_CreateTexture(render.getBase(), pixelFormat, SDL_TEXTUREACCESS_TARGET, width, height);
    setup(true);

    return 0;
}

int SDLTexture::loadFile(char *filePath)
{
	if(tex) destroy();

	tex = IMG_LoadTexture(render.getBase(), filePath);

	if(tex == NULL)
	{
		error("Unable to load texture from file: ", (char *)IMG_GetError());
	}

	setup(true);

	return 0;
}

int SDLTexture::loadMemory(void *memory, int memSize)
{
	SDL_RWops *texMem = NULL;

	if(tex) destroy();

	texMem = SDL_RWFromMem(memory, memSize);

	tex = IMG_LoadTexture_RW(render.getBase(), texMem, SDL_TRUE);

	if(tex == NULL)
	{
		error("Unable to load texture from memory: ", (char *)IMG_GetError());
	}

	setup(true);

	return 0;
}


/*---------------------------------------------------------
SDLWindow class methods
---------------------------------------------------------*/
int SDLWindow::create(const char *title, int width, int height)
{
	window = SDL_CreateWindow(title,
									SDL_WINDOWPOS_UNDEFINED,
									SDL_WINDOWPOS_UNDEFINED,
									width, height,
									WINDOW_FLAGS);

	if(!window) error("Unable to create window: ", (char *)SDL_GetError());

	enableMouseFocus();
	enableInputFocus();

	return 0;
}

/*---------------------------------------------------------
SDLRenderer class methods
---------------------------------------------------------*/

void SDLRenderer::init(SDLWindow *window, int logicalWidth, int logicalHeight)
{
    #ifdef PANDORA
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles");
    #endif

	renderer = SDL_CreateRenderer(window->getWindow(), -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);

	if(!renderer) error("Unable to create renderer: ", (char *)SDL_GetError());

	SDL_RenderSetLogicalSize(renderer, logicalWidth, logicalHeight);

	SDL_SetRenderDrawBlendMode(render.getBase(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	resetRenderTexture();

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	//Setup target to another texture
	SDL_SetRenderTarget(renderer, renderTexture.getBase());

	//Load fade texture
	SDL_Surface *fadeSurface = SDL_CreateRGBSurface(0, HWXMAX, HWYMAX, 32, 0, 0, 0, 0);

	fadeTexture.loadPointer(SDL_CreateTextureFromSurface(render.getBase(), fadeSurface), true);
	SDL_SetTextureBlendMode(fadeTexture.getBase(), SDL_BLENDMODE_BLEND);

	SDL_FreeSurface(fadeSurface);

	return;
}

void SDLRenderer::resetRenderTexture(void)
{
	renderTexture.destroy();
	renderTexture.loadPointer(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, HWXMAX, HWYMAX), true);
	return;
}

//Stretch screen instead of keeping aspect ratio and borders
void SDLRenderer::stretch(int width, int height)
{
	viewport.x = viewport.y = 0;
	viewport.w = width;
	viewport.h = height;

	int xOffset = HWXMAX;
	int yOffset = HWYMAX;

	if(XFACTOR > (YFACTOR+0.01f))
	{
		xOffset = XSTRETCH;
		xRatio = HWXMAX/((float)width*YFACTOR);
		yRatio = HWYMAX/((float)height*YFACTOR);
	}
	else if(YFACTOR > (XFACTOR+0.01f))
	{
		yOffset = YSTRETCH;
		xRatio = HWXMAX/((float)width*XFACTOR);
		yRatio = HWYMAX/((float)height*XFACTOR);
	}
	else
	{
		xRatio = HWXMAX/((float)width*XFACTOR);
		yRatio = HWYMAX/((float)height*YFACTOR);
	}

	xOffset -= width;
	yOffset -= height;

	resetRenderTexture();

	if(xOffset > 0) viewport.x = xOffset/2;
	if(yOffset > 0) viewport.y = yOffset/2;

	SDL_RenderSetViewport(renderer, &viewport);

	SDL_RenderCopy(renderer, renderTexture.getBase(), NULL, NULL);
   SDL_RenderPresent(renderer);
   SDL_SetRenderTarget(renderer, renderTexture.getBase()); 

	return;
}

void SDLRenderer::setLinearScaling(bool enable)
{
	(enable == true) ? SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear") : SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	resetRenderTexture();

	return;
}

SDLTexture *SDLRenderer::createScreenTexture() {
	SDLTexture *screen = new SDLTexture;
	SDL_Texture *copy = SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, HWXMAX, HWYMAX);

	SDL_SetRenderTarget(renderer, copy);

	drawTexture(&renderTexture, 0, 0);
	SDL_RenderPresent(renderer);

	screen->loadPointer(copy, true);
	SDL_SetRenderTarget(renderer, renderTexture.getBase());

	return screen;
}

void SDLRenderer::update(void) {

	SDL_RenderPresent(renderer);
	SDL_SetRenderTarget(renderer, NULL);

	SDL_RenderCopyEx(renderer, renderTexture.getBase(), NULL, NULL, 0.0f, NULL, SDL_FLIP_NONE); //Needed so that the texture can be stretched.
	//drawTexture(&renderTexture, 0, 0, 0, 0, 320, 240);

	SDL_RenderPresent(renderer);

	SDL_SetRenderTarget(renderer, renderTexture.getBase());

	return;
}

void SDLRenderer::fade(SDLTexture *background, int a)
{
	fadeTexture.setAlpha(a);

	render.drawTexture(background, 0, 0);
	render.drawTexture(&fadeTexture, 0, 0);

	return;
}

void SDLRenderer::x_drawsprite(SDLSprite *sprite, int x, int y, int n)
{
	SDL_Rect clip;

    if(sprite->shouldDraw() == false) return;

	x += BORDER; //Ajust for screen border

	if(sprite->getNumFrames() == 0 || n == -1)
		drawTexture(sprite->getTexture(), x, y);
	else
	{
		clip = sprite->getFrameRect(n);
		drawTexture(sprite->getTexture(), x, y, &clip);
	}

	return;
}

void SDLRenderer::drawRect(SDL_Rect *rect, int r, int g, int b)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);

	SDL_RenderFillRect(renderer, rect);

	return;
}

void SDLRenderer::drawTile(SDLTile *tile, int x, int y)
{
	const SDL_Rect clip = {0, 0, TILEXS, TILEYS};

	drawTile(tile, x, y, &clip);

	return;
}

void SDLRenderer::drawTile(SDLTile *tile, int x, int y, const SDL_Rect *clip)
{
	SDLTexture *texture = tile->getBase();

	SDL_Rect dest = {BORDER + x, y, clip->w, clip->h};

	if(tile != NULL && tile->drawable())
	{
		SDL_RenderCopy(renderer, texture->getBase(), clip, &dest);
	}

	return;
}

void SDLRenderer::drawMap(SDLMapLayer *map, int yOffset)
{
	const int MIN_TILE_DRAW = 72; //7x9 tiles each 32x32 pixels
	const int MAX_TILE_DRAW = 81; //For when an extra row needs to be drawn because the first row is only a partially shown

	int bodySize = 0; //The number of tiles to draw between the first and last rows
	int fullSize = 0;	//Total amount of tiles to draw, based on yOffset mod 32

	yOffset +=16; //Fix the offset

	int i = 0;
	int y = 0;
	int rowStart = (int)floor((float)yOffset/(float)TILEYS);
	int yMod = yOffset % TILEYS;
	int offset = (map->getSize() - MIN_TILE_DRAW) - (rowStart * XTILES);

	//Clipping for first and last row
	SDL_Rect firstRow = {0, TILEYS - yMod, TILEXS, yMod};
	SDL_Rect lastRow = {0, 0, TILEXS, TILEYS};

	if(yMod < 16) //An extra row needs to be drawn
	{
		bodySize = MAX_TILE_DRAW - (2*XTILES);
		fullSize = MAX_TILE_DRAW;
	}
	else
	{
		bodySize = MIN_TILE_DRAW - (2*XTILES);
		fullSize = MIN_TILE_DRAW;
	}

	//printf("yOffset %d, rowStart %d, tiles %d, length %d, offset %d, yMod %d\n", yOffset, rowStart, map->getSize(), map->getLength(), offset, yMod);

	//printf("LastRow %d %d\n", lastRow.w, lastRow.h);

	//Due to the way the level data is saved all drawing has to be done from right to left

	//First draw the top row, allow for clipping
	for(i = 0; i < XTILES; i++)
	{
		//printf("[%d, %d] First row test [%d, %d]\n",i, offset - i, ((XTILES - 1) - (i % 9)) * TILEXS, y);
		render.drawTile(map->getTile(i + offset), ((XTILES - 1) - (i % 9)) * TILEXS, y, &firstRow);
	}

	y += firstRow.h;

	//Next draw all rows up to the last one, no clipping should be needed
	for(i = i; i < bodySize + XTILES; i++)
	{

		if(i != XTILES && i % 9 == 0)
			y += TILEYS;

		//printf("Body test [%d, %d]\n", (8 - (i % 9)) * TILEXS, y);

		render.drawTile(map->getTile(i + offset), ((XTILES - 1) - (i % 9)) * TILEXS, y);
	}

	y += TILEYS;
	lastRow.h = YMAX - y;

	//Draw the final row, allow for clipping
	for(i = i; i < fullSize; i++)
	{
		//printf("[%d, %d] Last row test [%d, %d]\n",i, offset - i, ((XTILES - 1) - (i % 9)) * TILEXS, y);
		render.drawTile(map->getTile(i + offset), ((XTILES - 1) - (i % 9)) * TILEXS, y, &lastRow);
	}

	//if(lastRow.h > 32) printf("Last row h %d\n", lastRow.h);

	return;
}

void SDLRenderer::drawTexture(SDLTexture *texture, int x, int y, int cx, int cy, int cw, int ch)
{
	SDL_Rect dest = {x, y, cw, ch};
	SDL_Rect src = {cx, cy, cw, ch};

	SDL_RenderCopy(renderer, texture->getBase(), &src, &dest);

	return;
}

void SDLRenderer::drawTexture(SDLTexture *texture, int x, int y, SDL_Rect *clip)
{
	SDL_Rect dest = {x, y, clip->w, clip->h};

	SDL_RenderCopy(renderer, texture->getBase(), clip, &dest);

	return;
}

void SDLRenderer::drawTexture(SDLTexture *texture, int x, int y)
{
	SDL_Rect dest = {x, y, texture->getWidth(), texture->getHeight()};

	SDL_RenderCopy(renderer, texture->getBase(), NULL, &dest);

	return;
}

/*---------------------------------------------------------
SDLTilePool class methods
---------------------------------------------------------*/
int SDLTilePool::add(SDLTile *tile, long *tileAddress)
{
    if(inPool(tileAddress) == false)
    {
       tiles[tileAddress] = tile;
		 numTiles ++;

		 return 1;
    }

    return 0;
}

void SDLTilePool::destroy()
{
   std::map<long *, SDLTile *>::iterator it;

   for (it = tiles.begin(); it != tiles.end(); ++it)
   {
      it->second->destroy();
		delete it->second;

      numTiles --;
	}

	tiles.clear();

   return;
}

/*---------------------------------------------------------
SDLMapLayer class methods
---------------------------------------------------------*/

SDLTile *SDLMapLayer::createTile(int arrayIndex, long *tileData)
{
	SDLTile *cTile = NULL;

	if(!tPool.inPool(tileData))
	{
		cTile = new SDLTile;

		if(tileData != NULL)
			convert(cTile, tileData);

		tPool.add(cTile, tileData);
	}
	else
	{
		cTile = tPool.get(tileData);
	}

	*(tiles + arrayIndex) = cTile;

	return cTile;
}

void SDLMapLayer::convert(SDLTile *tile, long *tileData)
{
	int x, y, mod;
	char *bytePix = NULL;
	Uint32 *surfacePix = NULL;
	unsigned char pix;
	SDL_Surface *surface = NULL;
	SDL_Surface *temp = NULL;

	//Create a blank surface
	temp = SDL_CreateRGBSurface(0, TILEXS, TILEYS, 32, 0, 0, 0, 0);

	if(shadows == true)
	{
		surface = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGBA8888, 0);
		SDL_FreeSurface(temp);
	}
	else
	{
		surface = temp;
	}

	SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));

	//Prepare for direct pixel access
	SDL_LockSurface(surface);

	bytePix = (char *)tileData;
	surfacePix = (Uint32 *)surface->pixels;

	for(y = 0; y < TILEYS; y++) //For each row of the sprite map
	{
		for(x = 0; x < TILEXS; x++) //For each column up to the width of the sprite
		{
			mod = x % 4; //Find the plane the requested pixel (x,y) is in

			pix = *(bytePix + (y*(TILEXS) + ((TILEXS*mod)/4) + (x/4))); //Get the pixel index colour from the correct address

			if(pix == 255) //Shadow
			{
				if(shadows == true)
					surfacePix[x + (y * TILEXS)] = SDL_MapRGBA(surface->format, 1, 1, 1, 64);
				else
					surfacePix[x + (y * TILEXS)] = SDL_MapRGBA(surface->format, palette.sdl_rgb[0].r, palette.sdl_rgb[0].g, palette.sdl_rgb[0].b, SDL_ALPHA_OPAQUE);
			}
			else
            {
                surfacePix[x + (y * TILEXS)] = SDL_MapRGBA(surface->format, palette.sdl_rgb[pix].r, palette.sdl_rgb[pix].g, palette.sdl_rgb[pix].b, SDL_ALPHA_OPAQUE);
            }
		}
	}

	SDL_UnlockSurface(surface);

	//Use first colour of palette as colour key.
	SDL_SetColorKey(surface, 1, SDL_MapRGBA(surface->format, palette.sdl_rgb[0].r, palette.sdl_rgb[0].g, palette.sdl_rgb[0].b, 255));

	//Convert the surface into a texture and load it into the class
	tile->load(SDL_CreateTextureFromSurface(render.getBase(), surface), true, tileData);
	SDL_SetTextureBlendMode(tile->getBase()->getBase(), SDL_BLENDMODE_BLEND);

	//Clean up
	SDL_FreeSurface(surface);

	return;
}
