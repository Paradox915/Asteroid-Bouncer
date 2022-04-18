/*
The main file for the game.
Hugh Smith
*/

// includes - libarys
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <SDL2/SDL_timer.h> 

// bad parctice but im doing it anyway
using namespace std;

// includes - header files
#include "../include/spaceships.hpp"
#include "../include/entity.hpp"
#include "../include/player.hpp"

// constants
int WIDTH = 1920;
int HEIGHT = 1080;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Surface* surface = NULL; 
SDL_Texture* tex = NULL;

const Uint8* kb;
// functions
bool init_sdl()
{
	bool result = true;
	/*Function to start and setup SDL*/
	// initalise SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0) // returns - on fail
	{
		cout << "failed it intalise SDL";
		result = false;
	}


	window = SDL_CreateWindow("Asteroid Bouncer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

	if (window == NULL)
	{
		cout << "failed it create window";
		result = false;
	}
	
	Uint32 render_flags = SDL_RENDERER_ACCELERATED; 
	renderer = SDL_CreateRenderer(window, -1, render_flags);
	if (renderer == NULL)
	{
		cout << "failed it create renderer";
		result = false;
	}

	return result;
}

void stop_sdl()
{
	/*Function to stop SDL*/

	// destroy renderer 
	SDL_DestroyRenderer(renderer); 

	// destroy window 
	SDL_DestroyWindow(window); 

	SDL_Quit();
}

bool check_exit()
{
	/*check if the use wants to exit the program*/
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{ 
		if(event.type == SDL_QUIT||
                  (event.type == SDL_WINDOWEVENT &&
                   event.window.event == SDL_WINDOWEVENT_CLOSE)){ 
			return false;
		}
	}
	return true;
}

void game_loop()
{
	/*the core game loop*/
}

void render(Entity ent)
{
	/*render stuf to the screen*/

	surface = IMG_Load(ent.texture); 

	// loads image to our graphics hardware memory. 
	tex = SDL_CreateTextureFromSurface(renderer, surface); 

	// clears main-memory 
	SDL_FreeSurface(surface); 

	// let us control our image position 
	// so that we can move it with our keyboard. 
	SDL_Rect dest; 

	// connects our texture with dest to control position 
	SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h); 

	// adjust height and width of our image box. 
	dest.w *= 6; 
	dest.h *= 6; 
	// sets initial x-position of object 
	dest.x = (0 - dest.w) / 2; 

	// sets initial y-position of object 
	dest.y = (0 - dest.h) / 2; 

	dest.x += ent.x;
	dest.y += ent.y;
	SDL_RenderCopyEx(renderer, tex, NULL, &dest, ent.rotation, NULL, SDL_FLIP_NONE); 

	// triggers the double buffers 
	// for multiple rendering 
	SDL_RenderPresent(renderer); 
}

void render_objects(Entity objects_to_render[], int lengnth)
{
	/*render all objects in list*/
	// clears the screen 
	SDL_RenderClear(renderer); 
	for(int i = 0;  i < lengnth; i ++)
	{
		render(objects_to_render[i]);
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderDrawLine(renderer,1,1,900,900);
	
}

int main(int argc, char* args[])
{
	Entity fd(900/3,900/3,80,"sprites/ship.png");
	Ship test_ship(900/3,900/3, "sprites/enemy.png", 1, 90, 10);
	Player player_1(0,0,"sprites/ship.png",0,100, 15, 1);
	bool gameRunning = init_sdl();
	while(gameRunning == true)
	{
		// The main game loop
		Uint64 frame_start = SDL_GetPerformanceCounter();
		test_ship.move();
		player_1.move();
		Entity players[3] = {player_1.get_entity(), test_ship.get_entity(), fd};
		int legnth = sizeof(players)/sizeof(players[0]);
		// /*get any inputs from the user and return them as a list*/
		SDL_PumpEvents();
		kb = SDL_GetKeyboardState(NULL);
		if(kb[SDL_SCANCODE_SPACE])
			cout << "space";
		if(kb[SDL_SCANCODE_DOWN])
			player_1.accelerate(-1);
		if(kb[SDL_SCANCODE_UP])
			player_1.accelerate(1);
		if(kb[SDL_SCANCODE_LEFT])
			player_1.rotation -= 1;
		if(kb[SDL_SCANCODE_RIGHT])
			player_1.rotation += 1;
		game_loop();
		render_objects(players, legnth);
		// check if exit
		gameRunning = check_exit();

		Uint64 frame_end = SDL_GetPerformanceCounter();

		float elapsedMS = (frame_end - frame_start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
		

		// Cap to 60 FPS
		SDL_Delay(floor(16.666f - elapsedMS));
	}
	stop_sdl();
	return 0;
}
