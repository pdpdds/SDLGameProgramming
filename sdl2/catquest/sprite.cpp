#include "sprite.h"
#include "gamebase.h"
#include "draw.h"

Sprite::Sprite(std::string url, int x, int y) {
	texture = NULL;
	width = 0;
	height = 0;
	origin = new SDL_Point();
	origin->x = x;
	origin->y = y;
	if(url != "") {
		change(url, x, y);
	}
}
Sprite::~Sprite() {
	free();
	delete origin;
}
void Sprite::free() {
	if(texture != NULL) {
		SDL_DestroyTexture(texture);
		texture = NULL;
		width = 0;
		height = 0;
		origin->x = 0;
		origin->y = 0;
	}
}
void Sprite::change(std::string url, int x, int y) {
	free();
	SDL_Surface* temp = IMG_Load(url.c_str());
	ok = false;
	if(temp == NULL) {
		printf( "Unable to load image %s! SDL_image Error: %s\n", url.c_str(), IMG_GetError() );
	} else {
		SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0xFF, 0xFF, 0xFF ));
		texture = SDL_CreateTextureFromSurface(Gamebase::getRenderer(), temp);
		if(texture == NULL) {
			printf( "Unable to create texture from %s! SDL Error: %s\n", url.c_str(), SDL_GetError() );
		} else {
			setBlendMode(SDL_BLENDMODE_BLEND);
			width = temp->w;
			height = temp->h;
			origin->x = x;
			origin->y = y;
			ok = true;
		}
		SDL_FreeSurface(temp);
	}
	split(1, width, height);
}
Sprite* Sprite::split(int n, int w, int h, int x, int y, int sh, int sv, int per_row) {
	frames.clear();
	if(per_row != -1) {
		width = w * per_row + x;
	}
	for(int i = 0; i < n; i++) {
		SDL_Rect r;
		r.x = (w + sh) * i + x;
		r.y = y;
		while(r.x + w > width) {
			r.x -= (width - x);
			r.y += (h + sv);
		}
		r.w = w;
		r.h = h;
		frames.push_back(r);
	}
	width = w;
	height = h;
	return this;
}

void Sprite::draw(int n, int x, int y, int w, int h) {
	Draw::sprite(this, n, x, y, w, h);
}
void Sprite::draw(int n, SDL_Rect r) {
	Draw::sprite(this, n, r.x, r.y, r.w, r.h);
}
void Sprite::draw_ext(int n, SDL_Rect r, double image_angle) {
	Draw::sprite(this, n, r.x, r.y, r.w, r.h, image_angle);
}
void Sprite::draw_raw(int x, int y, int w, int h) {
	Draw::sprite_sheet(this, x, y, w, h);
}

void Sprite::colorMod(Uint8 r, Uint8 g, Uint8 b) {
	SDL_SetTextureColorMod(texture, r, g, b);
}
void Sprite::colorMod(Uint8 c[3]) {
	SDL_SetTextureColorMod(texture, c[0], c[1], c[2]);
}
void Sprite::alphaMod(Uint8 a) {
	SDL_SetTextureAlphaMod(texture, a);
}
void Sprite::setBlendMode(SDL_BlendMode bm) {
	SDL_SetTextureBlendMode(texture, bm);
}

Sprite* Sprite::addAnim(int b, int e, int s) {
	anim.push_back(Animation(b, e, s));
	return this;
}
Sprite* Sprite::addAnim(std::string seq, int s) {
	std::vector<int> sequence;
	while(seq != "") {
		sequence.push_back(std::stoi(seq.substr(0, seq.find(','))));
		if(seq.find(',') != seq.npos) {
			seq = seq.substr(seq.find(',') + 1, seq.length());
		} else {
			seq = "";
		}
	}
	anim.push_back(Animation(0, 0, s).setSequence(sequence));
	return this;
}
void Sprite::removeAnim(int n) {
	anim.erase(anim.begin(), anim.begin() + n);
}

SDL_Texture* Sprite::getTexture() {
	return texture;
}
SDL_Rect* Sprite::getFrame(int n) {
	return &frames[n];
}
Animation* Sprite::getAnim(int n) {
	return &anim[n];
}
int Sprite::getWidth() {
	return width;
}
int Sprite::getHeight() {
	return height;
}
SDL_Point* Sprite::getOrigin() {
	return origin;
}
