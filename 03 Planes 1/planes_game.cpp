#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 700;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* playerPlane = NULL;
SDL_Surface* bulletSprite = NULL;

void init() {
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		throw("SDL could not initialize!");
	}

	gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if( gWindow == NULL ) {
    throw("Window could not be created!");
	}

  gScreenSurface = SDL_GetWindowSurface( gWindow );
}

void clean() {
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;

  SDL_Quit();
}

SDL_Surface* loadSurface( string path ) {
	SDL_Surface* optimizedSurface = NULL;

	SDL_Surface* loadedSurface = SDL_LoadBMP( path.c_str() );
	if( loadedSurface == NULL ) {
		throw("Unable to load image");
	}

	optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
	if( optimizedSurface == NULL ) {
		throw("Unable to optimize image");
	}

	SDL_FreeSurface( loadedSurface );

	return optimizedSurface;
}

class Point {
public:
  int x;
  int y;

  Point() : x(0), y(0) {}
  Point(int x, int y) : x(x), y(y) {}
};

class Entity {
public:
  Point pos;
  int speed;
  Point direction;
  SDL_Surface* sprite = NULL;
public:
  Entity(Point pos, int speed, Point direction, SDL_Surface* sprite)
    : pos(pos), speed(speed), direction(direction), sprite(sprite) { }

  virtual void handleEvent(SDL_Event e) {}
  virtual void update(vector<Entity*>* entities) = 0;
  virtual void render() = 0;
};

class Bullet : public Entity {
public:
  Bullet(SDL_Surface* sprite) : Entity(Point(50, 50), 1, Point(0, 0), sprite) { }

  void update(vector<Entity*>* entities) {
    pos.y += direction.y * speed;
    pos.x += direction.x * speed;

    if(pos.y <= -25) {
      auto it = entities->begin();
      for(; *it != this; it++) {}
      entities->erase(it);
      delete(this);
    }
  }

  void render() {
    SDL_Rect stretchRect;
    stretchRect.x = pos.x;
    stretchRect.y = pos.y;
    stretchRect.w = 20;
    stretchRect.h = 50;

    SDL_BlitScaled( sprite, NULL, gScreenSurface, &stretchRect );
  }
};

class Player : public Entity  {
  bool isFiring = false;
  bool isFired = false;
public:
  Player(SDL_Surface* sprite) : Entity(Point(SCREEN_WIDTH/2, SCREEN_HEIGHT - sprite -> h), 1, Point(0, 0), sprite) { }

  void handleEvent(SDL_Event e) {
    if( e.type == SDL_KEYDOWN ) {
      switch( e.key.keysym.sym ) {
        case SDLK_LEFT:
        direction.x = -1;
        break;

        case SDLK_RIGHT:
        direction.x = 1;
        break;

        case SDLK_SPACE:
        isFiring = true;
        isFired = false;
        break;

        default:
        direction.x = 0;
        isFiring = false;
        break;
      }
    } else if( e.type == SDL_KEYUP ) {
      direction.x = 0;
      isFiring = false;
      isFired = false;
    }
  }

  void update(vector<Entity*>* entities) {
    pos.x += speed * direction.x;

    if(pos.x > SCREEN_WIDTH - 100) {
      direction.x = 0;
      pos.x = SCREEN_WIDTH - 100;
    } else if(pos.x < 0) {
      direction.x = 0;
      pos.x = 0;
    }

    if(isFiring && !isFired) {
      isFired = true;
      Bullet* newBullet = new Bullet(bulletSprite);
      newBullet->pos = Point(pos.x + 50 - 10, pos.y - 50);
      newBullet->direction.y = -1;

      entities->push_back(newBullet);
    }
  }

  void render() {
    SDL_Rect stretchRect;
    stretchRect.x = pos.x;
    stretchRect.y = pos.y;
    stretchRect.w = 100;
    stretchRect.h = 100;

    SDL_BlitScaled( sprite, NULL, gScreenSurface, &stretchRect );
  }
};

void load() {
	playerPlane = loadSurface( "plane1.bmp" );
	if( playerPlane == NULL ) {
    throw("Failed to load player plane image");
	}

	bulletSprite = loadSurface( "rocket.bmp" );
	if( bulletSprite == NULL ) {
    throw("Failed to load player plane image");
	}
}

void runGame() {
  bool quit = false;
  SDL_Event e;

  Player player = Player(playerPlane);
  vector<Entity*> entities;
  entities.push_back(&player);

  while(!quit) {
    cout << "Entities: " << entities.size() << endl;
    while( SDL_PollEvent( &e ) != 0 ) {
      if( e.type == SDL_QUIT ) {
        quit = true;
      }

      for(int i=0; i< entities.size(); i++) {
        entities[i]->handleEvent(e);
      }
    }

    for(int i=0; i< entities.size(); i++) {
      entities[i]->update(&entities);
    }

    SDL_FillRect( gScreenSurface, NULL, SDL_MapRGB( gScreenSurface->format, 0x00, 0xFF, 0xFF ) );
    for(int i=0; i< entities.size(); i++) {
      entities[i]->render();
    }

    SDL_UpdateWindowSurface( gWindow );
  }
}

int main() {
  try {
    init();
    load();
    runGame();
  } catch(exception e) {
    cout << e.what() << endl;
  }
  clean();
  return 0;
}
