/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, standard IO, and strings
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image
SDL_Surface* loadSurface( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//Current displayed image
SDL_Surface* mainHamster = NULL;
SDL_Surface* alternativeHamster = NULL;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface( gWindow );
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load stretching surface
	mainHamster = loadSurface( "hamster.bmp" );
	if( mainHamster == NULL )
	{
		printf( "Failed to load main hamster image!\n" );
		success = false;
	}

	alternativeHamster = loadSurface( "hamster-2-final.bmp" );
	if( alternativeHamster == NULL )
	{
		printf( "Failed to load alternative hamster image!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded image
	SDL_FreeSurface( mainHamster );
	mainHamster = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

SDL_Surface* loadSurface( std::string path )
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
		if( optimizedSurface == NULL )
		{
			printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return optimizedSurface;
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			int posX = 0;
			int posY = 0;
			int directionX = 0;
			int directionY = 0;

			SDL_Surface* activeHamster = mainHamster;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					else if( e.type == SDL_KEYDOWN )
					{
						//Select surfaces based on key press
						switch( e.key.keysym.sym )
						{
							case SDLK_UP:
							directionY = -1;
							break;

							case SDLK_DOWN:
							directionY = 1;
							break;

							case SDLK_LEFT:
							directionX = -1;
							break;

							case SDLK_RIGHT:
							directionX = 1;
							break;

							default:
							directionX = 0;
							directionY = 0;
							break;
						}
					}
					else if( e.type == SDL_KEYUP )
					{
						//Select surfaces based on key press
						switch( e.key.keysym.sym )
						{
							case SDLK_UP:
							case SDLK_DOWN:
							directionY = 0;
							break;

							case SDLK_LEFT:
							case SDLK_RIGHT:
							directionX = 0;
							break;
						}
					}
					else if( e.type == SDL_MOUSEBUTTONDOWN)
					{
						if(e.motion.x >= posX && e.motion.x <= posX + activeHamster->w/2 &&
							 e.motion.y >= posY && e.motion.y <= posY + activeHamster->h/2) {
							activeHamster = alternativeHamster;
						}
					}
					else if(e.type == SDL_MOUSEBUTTONUP)
					{
						activeHamster = mainHamster;
					}
				}

				if(posX > SCREEN_WIDTH - activeHamster->w/2) {
					directionX = 0;
					posX = SCREEN_WIDTH - activeHamster->w/2;
				} else if(posX < 0) {
					directionX = 0;
					posX = 0;
				}

				if(posY > SCREEN_HEIGHT - activeHamster->h/2) {
					directionY = 0;
					posY = SCREEN_HEIGHT - activeHamster->h/2;
				} else if(posY < 0) {
					directionY = 0;
					posY = 0;
				}

				posX += directionX;
				posY += directionY;

				//Apply the image stretched
				SDL_Rect stretchRect;
				stretchRect.x = posX;
				stretchRect.y = posY;
				stretchRect.w = activeHamster->w/2;
				stretchRect.h = activeHamster->h/2;

				SDL_FillRect( gScreenSurface, NULL, SDL_MapRGB( gScreenSurface->format, 0x00, 0xFF, 0xFF ) );
				SDL_BlitScaled( activeHamster, NULL, gScreenSurface, &stretchRect );
				// SDL_BlitSurface( mainHamster, NULL, gScreenSurface, NULL );

				//Update the surface
				SDL_UpdateWindowSurface( gWindow );
				// SDL_Delay( 5 );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
