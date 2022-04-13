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
// #include "../include/spaceships.hpp"
// #include "../include/entity.hpp"

// constants
int WIDTH = 900;
int HEIGHT = 900;

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

	SDL_Window* window;
	SDL_Renderer* renderer;

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
	SDL_Window* window;
	SDL_Renderer* renderer;

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

void input()
{
	/*get any inputs from the user*/
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{ 
		switch (event.type){ 
		case SDL_KEYDOWN: 
			// keyboard API for key pressed 
			cout << event.key.keysym.scancode;
		}
	}
}

void game_loop()
{
	/*the core game loop*/
}

void render()
{
	/*render stuf to the screen*/
	SDL_Window* window;
	SDL_Renderer* renderer;
	// creates a surface to load an image into the main memory 
	SDL_Surface* surface; 

	// please provide a path for your image 
	surface = IMG_Load("../sprites/ship.png"); 

	// loads image to our graphics hardware memory. 
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface); 

	// clears main-memory 
	SDL_FreeSurface(surface); 

	// let us control our image position 
	// so that we can move it with our keyboard. 
	SDL_Rect dest; 

	// connects our texture with dest to control position 
	SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h); 

	// adjust height and width of our image box. 
	dest.w /= 6; 
	dest.h /= 6; 

	// sets initial x-position of object 
	dest.x = (1000 - dest.w) / 2; 

	// sets initial y-position of object 
	dest.y = (1000 - dest.h) / 2; 

	// clears the screen 
	SDL_RenderClear(renderer); 
	SDL_RenderCopy(renderer, tex, NULL, &dest); 

	// triggers the double buffers 
	// for multiple rendering 
	SDL_RenderPresent(renderer); 
}

int main(int argc, char* args[])
{
	bool gameRunning = init_sdl();
	while(gameRunning)
	{
		// The main game loop
		input();
		game_loop();
		render();
		//check if exit
		gameRunning = check_exit();
		SDL_Delay(1000 / 60);
	}
	stop_sdl();
	return 0;
}