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
#include <map>
#include<fstream>
#include<json/json.h>
#include <list>
// bad parctice but im doing it anyway
using namespace std;

// includes - header files

#include "../include/spaceships.hpp"
#include "../include/entity.hpp"
#include "../include/player.hpp"
#include "../include/bullet.hpp"

// constants
int WIDTH = 1200;
int HEIGHT = 1000;

string name = "Hugh2";

const char* map_file = "sprites/map/big_map.png";
SDL_Surface* game_map = IMG_Load(map_file);

noise::module::Perlin perlin;
int SEED = 27;
float THRESHHOLD = 217;//0.7

Uint64 frame_now = SDL_GetPerformanceCounter();
Uint64 frame_last = 0;
double delta_time = 0;

// "x y", add_value
std::map<string, int> perlin_add_values { {"0 0", 10}, {"1 0", 10}, {"0 1", 10}, };
//perlin_add_values["hfg"] = 4;
Player player(960,540,"sprites/player.png",0,100, 3, 0.2);

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
	
	//Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	Uint32 render_flags = SDL_RENDERER_PRESENTVSYNC;
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

void player_scores_json(int score)
{
	/*get the players score into the json file*/
	// read the file
	ifstream i("database/player_scores.json", std::ifstream::binary);
	Json::Value j;
	i >> j;
	if(j[name] < score)
	{
		j[name] = score;
	}
	// output the file
	std::ofstream o("database/player_scores.json");
	o << j;
}

/* 
this function came from:
https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface 
*/
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

switch (bpp)
{
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(Uint32 *)p;
            break;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
      }
}
/* 
this function came from:
https://stackoverflow.com/questions/6852055/how-can-i-modify-pixels-using-sdl
*/
void PutPixel24_nolock(SDL_Surface * surface, int x, int y, Uint32 color)
{
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint8) * 3);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    pixel[0] = (color >> 24) & 0xFF;
    pixel[1] = (color >> 16) & 0xFF;
    pixel[2] = (color >> 8) & 0xFF;
#else
    pixel[0] = color & 0xFF;
    pixel[1] = (color >> 8) & 0xFF;
    pixel[2] = (color >> 16) & 0xFF;
#endif
}
void PutPixel24(SDL_Surface * surface, int x, int y, Uint32 color)
{
    if( SDL_MUSTLOCK(surface) )
        SDL_LockSurface(surface);
    PutPixel24_nolock(surface, x, y, color);
    if( SDL_MUSTLOCK(surface) )
        SDL_LockSurface(surface);
}

void set_pixel_colour(SDL_Surface* game_map,int x, int y, int value)
{
	/*change the colour of a pixel, just changes all rgb values the same*/
	PutPixel24(game_map, x, y, (Uint32)0xFF000000 | value | (value << 16) | (value << 8));
}

int get_pixel_darkness(SDL_Surface* game_map, int x, int y)
{
	/*return how dark a pixel is*/
	SDL_Color rgb;
	Uint32 data = getpixel(game_map, x, y);
	SDL_GetRGB(data, game_map->format, &rgb.r, &rgb.g, &rgb.b);
	return (int)(rgb.b+rgb.r+rgb.g)/3;
}


void render_asteroids(int case_num, float x, float y, float scale)
{
	/*render asteroids*/
	string num_string = to_string(case_num);
	string input = "sprites/marching_squares/case_"+num_string+".png";
	surface = IMG_Load(input.c_str());
	// loads image
	tex = SDL_CreateTextureFromSurface(renderer, surface); 
	// clears main-memory 
	SDL_FreeSurface(surface); 

	// let us control our image position 
	// so that we can move it with our keyboard. 
	SDL_Rect dest; 

	// connects our texture with dest to control position 
	SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h); 

	// adjust height and width of our image box. 
	dest.w *= scale; 
	dest.h *= scale; 
	// sets initial x-position of object 
	dest.x = (0 - dest.w) / 2; 

	// sets initial y-position of object 
	dest.y = (0 - dest.h) / 2; 
	float new_x = WIDTH/2 + (player.x - x);
	float new_y = HEIGHT/2 + (player.y - y);
	dest.x += new_x;
	dest.y += new_y;
	SDL_RenderCopyEx(renderer, tex, NULL, &dest, 0, NULL, SDL_FLIP_NONE);

	SDL_DestroyTexture(tex);

		
}

int get_asteroid_case(float x, float y, float box_size)
{
	/*get the case for the box with the bottom left corner 
	at location x,y and size of box_size*/
	int count = 0;
	//cout << x << "  -  "<< y << " --: "<< get_pixel_darkness(game_map, x, y)<< "\n\n";
	//cout << get_pixel_darkness(game_map, 200, 200)<< "\n";
	if(get_pixel_darkness(game_map, x, y) > THRESHHOLD)
	{
		count += 8;
	}
	if(get_pixel_darkness(game_map, x+box_size, y) > THRESHHOLD)
	{
		count += 4;
	}
	if(get_pixel_darkness(game_map, x+box_size, y+box_size) > THRESHHOLD)
	{
		count += 2;
	}
	if(get_pixel_darkness(game_map, x, y+box_size) > THRESHHOLD)
	{
		count += 1;
	}
	return count;
}

void asteroids_in_range(float xmin, float xmax, float ymin, float ymax, float grid_size)
{
	// round to grid
	xmin = floor(xmin / grid_size) * grid_size;
	xmax = ceil(xmax / grid_size) * grid_size;

	ymin = floor(ymin / grid_size) * grid_size;
	ymax = ceil(ymax / grid_size) * grid_size;

	int num;
	for(float x = xmin; x <= xmax; x+=grid_size)
	{
		for(float y = ymin; y <= ymax; y+=grid_size)
		{
			num = get_asteroid_case(x, y, grid_size);
			if(num != 0 && num != 15)
				render_asteroids(num,x,y, 1);
		}
	}
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
	dest.w *= 3; 
	dest.h *= 3; 
	// sets initial x-position of object 
	dest.x = (0 - dest.w) / 2; 

	// sets initial y-position of object 
	dest.y = (0 - dest.h) / 2; 

	float new_x = WIDTH/2 + (player.x - ent.x);
	float new_y = HEIGHT/2 + (player.y - ent.y);
	dest.x += new_x;
	dest.y += new_y;
	SDL_RenderCopyEx(renderer, tex, NULL, &dest, ent.rotation, NULL, SDL_FLIP_NONE); 
	SDL_DestroyTexture(tex);
}

void render_objects(list<Entity> objects_to_render)
{
	/*render all objects in list*/
	// clears the screen 
	SDL_RenderClear(renderer); 
	for(Entity object : objects_to_render)
	{
		render(object);
	}

	// render asteroids
	asteroids_in_range(player.x-WIDTH/2,player.x+WIDTH/2,player.y-HEIGHT/2,player.y+HEIGHT/2,16);
	// for multiple rendering 
	SDL_RenderPresent(renderer);
}

void stop()
{
	/*exit the program*/
	// stop sdl
	stop_sdl();
	// add the player score
	player_scores_json(player.score);
	// save the map
	SDL_SaveBMP(game_map, map_file);

}

int main(int argc, char* args[])
{	
	perlin.SetSeed(SEED);
	perlin.SetPersistence(0.7);
	perlin.SetFrequency(0.001);
	perlin.SetOctaveCount(5);
	
	Entity fd(50,-100,80,"sprites/planet.png");
	Ship test_ship(100,50, "sprites/enemy.png",1, 90, 10);
	bool gameRunning = init_sdl();
	Uint64 start_frame = 0;
	Uint64 end_frame;
	list<Bullet> bullets;

	cout << get_pixel_darkness(game_map, 960, 540)<< "\n";
	cout << get_pixel_darkness(game_map, 2, 2)<< "\n";

	while(gameRunning == true)
	{
		// The main game loop
		list<Entity> objects = {};
		frame_last = frame_now;
   		frame_now = SDL_GetPerformanceCounter();
   		delta_time = (double)((frame_now-frame_last)*1000 / (double)SDL_GetPerformanceFrequency());

		
		// /*get any inputs from the user*/
		SDL_PumpEvents();
		kb = SDL_GetKeyboardState(NULL);
		if(kb[SDL_SCANCODE_SPACE]){
			Bullet current_bullet = player.shoot(0, SDL_GetTicks());
			bullets.push_front(current_bullet);
		}
		if(kb[SDL_SCANCODE_LALT]){
			Bullet current_bullet = player.shoot(1, SDL_GetTicks());
			bullets.push_front(current_bullet);
		}
		if(kb[SDL_SCANCODE_DOWN])
			player.accelerate(-0.03, delta_time);
		if(kb[SDL_SCANCODE_UP])
			player.accelerate(0.03, delta_time);
		if(kb[SDL_SCANCODE_LEFT])
			player.rotation -= 0.2*delta_time;
		if(kb[SDL_SCANCODE_RIGHT])
			player.rotation += 0.2*delta_time;
		if(kb[SDL_SCANCODE_ESCAPE])
			player.magnitude = 0;
		game_loop();

		typedef list<Bullet> Cont;
		for( Cont::iterator i = bullets.begin(); i != bullets.end(); ++i ) {
			Bullet & s(*i);
			objects.push_front(s.move());
		}
		objects.push_front(test_ship.move());
		objects.push_back(player.move());
		render_objects(objects);
		// check if exit
		gameRunning = check_exit();
	}
	stop();
	return 0;
}
