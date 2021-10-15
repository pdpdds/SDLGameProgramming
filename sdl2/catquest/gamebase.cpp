#include "gamebase.h"
#include "pause_menu.h"

SDL_Window* Gamebase::window = NULL;
SDL_Surface* Gamebase::screenSurface = NULL;
SDL_Renderer* Gamebase::renderer = NULL;
const Uint8* Gamebase::keys = NULL;
Uint8* Gamebase::keys_p = NULL;
bool Gamebase::ok = true;

int Gamebase::screen_width = 0;
int Gamebase::screen_height = 0;

int Gamebase::c_view = -1;

std::vector<View*> Gamebase::views = std::vector<View*>();
std::vector<Sprite*> Gamebase::sprites = std::vector<Sprite*>();
std::vector<Object*> Gamebase::objects = std::vector<Object*>();
std::vector<Font*> Gamebase::fonts = std::vector<Font*>();
std::vector<Sound*> Gamebase::sounds = std::vector<Sound*>();
std::vector<Music*> Gamebase::musics = std::vector<Music*>();

Object* Gamebase::paused = NULL;

Gamebase::Gamebase(int w, int h) {
	screen_width = w;
	screen_height = h;

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0) {
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	} else if(window == NULL) {
		window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height	, SDL_WINDOW_SHOWN);
		if(window == NULL) {
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		} else {
			SDL_SetWindowBordered(window, SDL_FALSE);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if(renderer == NULL) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
			} else {
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				} else {
					if(TTF_Init() == -1)	{
						printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					} else {
						 //Initialize SDL_mixer
						if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
							printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
						} else {
							screenSurface = SDL_GetWindowSurface(window);
							addView(0, 0, screen_width, screen_height);
							useView(0);
							keys = SDL_GetKeyboardState(NULL);

							keys_p = new Uint8[SDL_NUM_SCANCODES];
							for(int i = 0; i < SDL_NUM_SCANCODES; i++) {
								keys_p[i] = 0;
							}
							ok = true;

							return;
						}
					}
				}
			}
		}
	}
	ok = false;
}
Gamebase::~Gamebase() {
	clearObjects();
	while(sprites.size()) {
		removeSprite(0);
	}
	while(views.size()) {
		removeView(0);
	}
	while(fonts.size()) {
		removeFont(0);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

SDL_Surface* Gamebase::getSurface()  {
	return screenSurface;
}
SDL_Renderer* Gamebase::getRenderer()  {
	return renderer;
}
bool Gamebase::checkKey(int k) {
	return keys[k] != 0;
}
bool Gamebase::checkKeyPressed(int k) {
	return keys_p[k] == 1;
}
bool Gamebase::checkKeyReleased(int k) {
	return keys_p[k] == 3;
}
void Gamebase::clearInput() {
	for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
		keys_p[i] = 2 * checkKey(i);
	}
}

Sprite* Gamebase::addSprite(std::string url, int x, int y) {
	Sprite* s = new Sprite(url, x, y);
	if(s->isOK()) {
		sprites.push_back(s);
		return s;
	} else {
		delete s;
	}
	return NULL;
}
void Gamebase::changeSprite(int i, std::string url) {
	sprites[i]->change(url);
}
void Gamebase::removeSprite(int i) {
	delete sprites[i];
	sprites.erase(sprites.begin() + i);
}
Sprite* Gamebase::getSprite(int i) {
	return sprites[i];
}

Font* Gamebase::addFont(std::string url, int size) {
	Font* f = new Font(url, size);
	if(f->isOK()) {
		fonts.push_back(f);
		return f;
	} else {
		delete f;
	}
	return NULL;
}
void Gamebase::changeFont(int i, std::string url, int size) {
	fonts[i]->change(url, size);
}
void Gamebase::removeFont(int i) {
	delete fonts[i];
	fonts.erase(fonts.begin() + i);
}
Font* Gamebase::getFont(int i) {
	return fonts[i];
}

void Gamebase::removeObject(Object* o) {
	for(unsigned int i = 0; i < objects.size(); i++) {
		if(objects[i] == o) {
			if (o == paused) {
				paused = NULL;
			}
			delete o;
			objects.erase(objects.begin() + i);
			return;
		}
	}
}
void Gamebase::removeObject(int i) {
	if (objects[i] == paused) {
		paused = NULL;
	}
	delete objects[i];
	objects.erase(objects.begin() + i);
}
void Gamebase::clearObjects() {
	while(objects.size()) {
		removeObject(0);
	}
}
Object* Gamebase::getObject(int i) {
	return objects[i];
}

View* Gamebase::addView(int px, int py, int pw, int ph) {
	View* v = new View(px, py, pw, ph);
	if(v->isOK()) {
		views.push_back(v);
		return v;
	} else {
		delete v;
	}
	return NULL;
}
void Gamebase::removeView(int i) {
	delete views[i];
	views.erase(views.begin() + i);
}
View* Gamebase::getView(int i) {
	return views[i];
}
void Gamebase::useView(int i) {
	SDL_RenderSetViewport(renderer, views[i]->getPort());
	c_view = i;
}
int Gamebase::currentView() {
	return c_view;
}
void Gamebase::moveView(int v, int x, int y) {
	views[v]->set_viewx(x);
	views[v]->set_viewy(y);
}

Sound* Gamebase::addSound(std::string url) {
	Sound* s = new Sound(url);
	if(s->isOK()) {
		sounds.push_back(s);
		return s;
	} else {
		delete s;
	}
	return NULL;
}
void Gamebase::changeSound(int i, std::string url) {
	sounds[i]->change(url);
}
void Gamebase::removeSound(int i) {
	delete sounds[i];
	sounds.erase(sounds.begin() + i);
}
Sound* Gamebase::getSound(int i) {
	return sounds[i];
}

Music* Gamebase::addMusic(std::string url) {
	Music* m = new Music(url);
	if(m->isOK()) {
		musics.push_back(m);
		return m;
	} else {
		delete m;
	}
	return NULL;
}
void Gamebase::changeMusic(int i, std::string url) {
	musics[i]->change(url);
}
void Gamebase::removeMusic(int i) {
	delete musics[i];
	musics.erase(musics.begin() + i);
}
Music* Gamebase::getMusic(int i) {
	return musics[i];
}

void Gamebase::roomStart() {
	for(unsigned int i = 0; i < objects.size(); i++) {
		objects[i]->roomStart();
	}
}

void Gamebase::events() {
	while(SDL_PollEvent(&evt) != 0) {
		if(evt.type == SDL_QUIT) {
			ok = false;
		} else if(evt.type == SDL_MOUSEMOTION || evt.type == SDL_MOUSEBUTTONDOWN || evt.type == SDL_MOUSEBUTTONUP) {
			mouse_button = SDL_GetMouseState(&mouse_x, &mouse_y);
		} else if(evt.type == SDL_MOUSEWHEEL) {
			/*
			evt.wheel.x;
			evt.wheel.y;
			evt.wheel.direction;
			*/
		}
	}
	for(int i = 0; i < SDL_NUM_SCANCODES; i++) {
		if(keys_p[i] == 0 && keys[i] == 1) {
			keys_p[i] = 1;
		} else if(keys_p[i] == 2 && keys[i] == 0) {
			keys_p[i] = 3;
		} else if(keys_p[i] == 3 || (keys_p[i] == 1 && keys[i] == 0)) {
			keys_p[i] = keys[i];
		} else if(keys[i] == 1) {
			keys_p[i] = 2;
		}
	}
	if ((Gamebase::checkKeyPressed(SDL_SCANCODE_P) || Gamebase::checkKeyPressed(SDL_SCANCODE_ESCAPE)) && Room::c_room != -1) {
		Gamebase::pause();
		Gamebase::clearInput();
	}
}

void Gamebase::runObjects() {
	if (paused == NULL) {
		for (unsigned int i = 0; i < objects.size(); i++) {
			if (on_screen(objects[i]) || objects[i]->isImportant()) {
				objects[i]->step();
			}
		}
	} else {
		paused->step();
	}
}

void Gamebase::draw() {
	for (int d = Object::getMaxDepth(); d >= Object::getMinDepth(); d--) {
		for (unsigned int i = 0; i < objects.size(); i++) {
			if (on_screen(objects[i]) && objects[i]->getDepth() == d) {
				objects[i]->draw();
			}
		}
	}
	if (paused != NULL) {
		paused->draw();
	}
}

void Gamebase::refresh() {
	SDL_RenderPresent(renderer);
}

bool Gamebase::on_screen(Object* o, int extra) {
	SDL_Point pos = o->get_pos();
	SDL_Rect view = {0, 0, screen_width, screen_height};
	if(c_view != -1) {
		view = *views[c_view]->getView();
	}
	return (pos.x > view.x - 3 * TILE_SIZE - extra && pos.x < view.x + view.w + 3 * TILE_SIZE + extra &&
			pos.y > view.y - 3 * TILE_SIZE - extra && pos.y < view.y + view.h + 3 * TILE_SIZE + extra);
}

void Gamebase::end() {
	ok = false;
}

bool Gamebase::isOK() {
	return Gamebase::ok;
}

void Gamebase::pause() {
	if (paused == NULL) {
		paused = addObject<PauseMenu>(views[0]->getView()->x, views[0]->getView()->y);
	} else {
		removeObject(paused);
	}
}
