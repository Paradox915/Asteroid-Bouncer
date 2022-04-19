/*
The main file for the game.
Hugh Smith
*/

// includes - libarys
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <SDL2/SDL_timer.h> 
#include <noise/noise.h>
// bad parctice but im doing it anyway
using namespace std;

// includes - header files
#include "../include/spaceships.hpp"
#include "../include/entity.hpp"
#include "../include/player.hpp"

// constants
int WIDTH = 900;
int HEIGHT = 900;

Uint64 frame_now = SDL_GetPerformanceCounter();
Uint64 frame_last = 0;
double delta_time = 0;

Player player(0,0,"sprites/ship.png",0,100, 5, 1);

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Surface* surface = NULL; 
SDL_Texture* tex = NULL;

SDL_Surface* back_ground = IMG_Load("sprites/background.png");

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

	int new_x = WIDTH/2 + (player.x - ent.x);
	int new_y = HEIGHT/2 + (player.y - ent.y);
	dest.x += new_x;
	dest.y += new_y;
	//cout <<ent.x<<"-"<<ent.y<<" -- "<<dest.x<<"-"<<dest.y <<"\n";
	SDL_RenderCopyEx(renderer, tex, NULL, &dest, ent.rotation, NULL, SDL_FLIP_NONE); 
	 
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
	// for multiple rendering 
	SDL_RenderPresent(renderer);
}

int main(int argc, char* args[])
{

	noise::module::Perlin perlin;
	perlin.SetSeed(1);
	double value = perlin.GetValue(190,2,3);

	Entity fd(50,-100,80,"sprites/ship.png");
	Ship test_ship(100,50, "sprites/enemy.png",0.05, 90, 10);
	bool gameRunning = init_sdl();
	while(gameRunning == true)
	{
		// The main game loop
		frame_last = frame_now;
   		frame_now = SDL_GetPerformanceCounter();

   		delta_time = (double)((frame_now-frame_last)*1000 / (double)SDL_GetPerformanceFrequency());

		
		test_ship.move();
		player.move();
		Entity players[3] = {player.get_entity(), test_ship.get_entity(), fd};
		int legnth = sizeof(players)/sizeof(players[0]);
		// /*get any inputs from the user*/
		SDL_PumpEvents();
		kb = SDL_GetKeyboardState(NULL);
		if(kb[SDL_SCANCODE_SPACE])
			cout << "space";
		if(kb[SDL_SCANCODE_DOWN])
			player.accelerate(-0.03, delta_time);
		if(kb[SDL_SCANCODE_UP])
			player.accelerate(0.03, delta_time);
		if(kb[SDL_SCANCODE_LEFT])
			player.rotation -= 0.2*delta_time;
		if(kb[SDL_SCANCODE_RIGHT])
			player.rotation += 0.2*delta_time;
		game_loop();
		render_objects(players, legnth);
		// check if exit
		gameRunning = check_exit();
	}
	stop_sdl();
	return 0;
}
