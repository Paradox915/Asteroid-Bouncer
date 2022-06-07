/*
The main file for the game.
Hugh Smith
*/

// includes - libarys
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <SDL2/SDL_timer.h> 
#include <SDL2/SDL_ttf.h>
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
#include "../include/enemy.hpp"
#include "../include/pixel_functions.hpp"

// constants
int WIDTH = 1200;
int HEIGHT = 1000;
int GRID_SIZE = 16;
string name = "Hugh2";

const char* map_file = "sprites/map/big_map.png";
SDL_Surface* game_map = IMG_Load(map_file);

float THRESHHOLD = 217;

list<float> x_asteroids;
list<float> y_asteroids;

Uint64 frame_now = SDL_GetPerformanceCounter();
Uint64 frame_last = 0;
double delta_time = 0;

// "x y", add_value
std::map<string, int> perlin_add_values { {"0 0", 10}, {"1 0", 10}, {"0 1", 10}, };
//perlin_add_values["hfg"] = 4;
Player player(960,540,"sprites/player/funky_ship_sprtesheet (1).png",0,100, 1.8, 0.2);

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

	if(TTF_Init() < 0) // returns - on fail
	{
		cout << "failed it intalise SDL TTF";
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

void hit_asteroid(SDL_Surface* game_map, int x, int y, int range, float stregnth)
{
	/*change the file in turms of a hit*/
	for(int i = x - range; i < x + range; i ++)
	{
	for(int j = y - range; j < y + range; j ++)
	{
		set_pixel_colour(game_map, i, j, 255);
	}
	}
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

	x_asteroids.clear();
	y_asteroids.clear();
	int num;
	for(float x = xmin; x <= xmax; x+=grid_size)
	{
		for(float y = ymin; y <= ymax; y+=grid_size)
		{
			num = get_asteroid_case(x, y, grid_size);
			if(num != 0 && num != 15){
				render_asteroids(num,x,y, GRID_SIZE/16);
				x_asteroids.push_front(x);
				y_asteroids.push_front(y);
				}
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
void render_animation_16(Entity ent, int size = 16, int frames=3, int speed = 100)
{
	/*render stuf to the screen but animated*/
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
	dest.w = 48; 
	dest.h = 48;

	// sets initial x-position of object 
	dest.x = (0 - dest.w) / 2; 

	// sets initial y-position of object 
	dest.y = (0 - dest.h) / 2; 

	float new_x = WIDTH/2 + (player.x - ent.x);
	float new_y = HEIGHT/2 + (player.y - ent.y);
	dest.x += new_x;
	dest.y += new_y;

	int ticks = SDL_GetTicks();
	int seconds = ticks / speed;		
	int sprite = seconds % frames;
	SDL_Rect srcrect = {sprite*size, 0, size, size};
	SDL_RenderCopyEx(renderer, tex, &srcrect, &dest, ent.rotation, NULL, SDL_FLIP_NONE); 
	SDL_DestroyTexture(tex);
}
void render_score(int score)
{
	
	TTF_Font* Sans = TTF_OpenFont("font.ttf", 15);
	if ( !Sans ) {
		cout << "Failed to load font: " << TTF_GetError() << endl;
	}
	SDL_Color White = {107, 139, 164};

	string s_score = "score :"+to_string(score)+" heath :"+to_string(player.health);

	SDL_Surface* surfaceMessage =
	TTF_RenderText_Solid(Sans, s_score.c_str() , White);

	// now you can convert it into a texture
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_Rect Message_rect; //create a rect
	Message_rect.x = 20;  //controls the rect's x coordinate 
	Message_rect.y = 0; // controls the rect's y coordinte
	Message_rect.w = 400; // controls the width of the rect
	Message_rect.h = 40; // controls the height of the rect


	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
	//SDL_RenderCopyEx(renderer, Message, NULL, &Message_rect, 0, NULL, SDL_FLIP_NONE); 
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

void render_player(Entity ent)
{
	/*render the player to the screen*/

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
	dest.w *= 1; 
	dest.h *= 3.66; 

	// sets initial x-position of object 
	dest.x = (0 - dest.w) / 2; 

	// sets initial y-position of object 
	dest.y = (0 - dest.h) / 2; 

	float new_x = WIDTH/2 + (player.x - ent.x);
	float new_y = HEIGHT/2 + (player.y - ent.y);
	dest.x += new_x;
	dest.y += new_y;
	//SDL_RenderCopy(renderer, tex, &srcrect, &dest);
	int ticks = SDL_GetTicks();
	int seconds = ticks / 100;		
	int sprite = seconds % 4;
	SDL_Rect srcrect = {sprite*32, 0, 32, 48};
	SDL_RenderCopyEx(renderer, tex, &srcrect, &dest, ent.rotation, NULL, SDL_FLIP_NONE); 
	SDL_DestroyTexture(tex);
}

void render_objects(list<Entity> objects_to_render)
{
	/*render all objects in list*/
	// clears the screen 
	SDL_RenderClear(renderer); 
	for(Entity object : objects_to_render)
	{
		if(object.animated == true){
			render_animation_16(object, object.size, object.frames);
		}else{
			render(object);
		}
	}

	// render asteroids
	asteroids_in_range(player.x-WIDTH/2,player.x+WIDTH/2,player.y-HEIGHT/2,player.y+HEIGHT/2,GRID_SIZE);
	// for multiple rendering 
	render_player(player.move());

	render_score(player.score);

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
	//SDL_SaveBMP(game_map, map_file);

}

int main(int argc, char* args[])
{		
	list<Enemy> enemys;
	for(int i = 0; i < 80; i++)
	{
		enemys.push_front(Enemy(900+10*i,900 + 0*i,"sprites/enemy_sprite_sheet.png",i,1, 2));
	}
	bool gameRunning = init_sdl();
	Uint64 start_frame = 0;
	Uint64 end_frame;
	list<Bullet> bullets;
	Uint32 time = 0;

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
		if(kb[SDL_SCANCODE_SPACE] && time < SDL_GetTicks()){
			time = SDL_GetTicks() + 100;
			Bullet current_bullet = player.shoot();
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
		if(kb[SDL_SCANCODE_EQUALS]&& GRID_SIZE < 80)
			GRID_SIZE += 1;
		if(kb[SDL_SCANCODE_MINUS] && GRID_SIZE > 16)
			GRID_SIZE -= 1;

		typedef list<Bullet> Cont;
		for( Cont::iterator i = bullets.begin(); i != bullets.end(); ++i ) {
			Bullet & s(*i);
			objects.push_front(s.move());
			if(s.check_collision_asteroids(game_map, THRESHHOLD))
			{
				hit_asteroid(game_map, s.x, s.y, 50, 100);
				i = bullets.erase(i);
			}
		}
		typedef list<Enemy> Cont2;
		for( Cont2::iterator i = enemys.begin(); i != enemys.end(); ++i ) {
			Enemy & s(*i);

			if(SDL_GetTicks()/100 > s.time_of_explosion + s.frames && s.time_of_explosion != -2)
			{
				i = enemys.erase(i);
				player.score += 1;
			}

			if(s.time_of_explosion == -2){
			s.boids(enemys, player, x_asteroids, y_asteroids);
			objects.push_front(s.move());
			}else
			{
				objects.push_front(s.get_entity());
			}

			// check for collision with asteroids
			if(s.check_collision_asteroids(game_map, THRESHHOLD) == true && s.time_of_explosion == -2)
			{
				s.texture = "sprites/explosion.png";
				s.frames = 20;
				s.time_of_death = -1;//SDL_GetTicks()/100;
				s.time_of_explosion = SDL_GetTicks()/100;
				hit_asteroid(game_map, s.x, s.y, 50, 100);
			}

			// check for collision with player
			if(s.check_collision(player.x, player.y) == true && s.time_of_explosion == -2)
			{
				s.texture = "sprites/explosion.png";
				s.frames = 20;
				s.time_of_death = -1;//SDL_GetTicks()/100;
				s.time_of_explosion = SDL_GetTicks()/100;
				hit_asteroid(game_map, s.x, s.y, 50, 100);
				player.health -= 10;
			}


			for(Bullet bullet : bullets)
			{
				if(s.check_collision(bullet.x, bullet.y))
				{
					s.health -= bullet.damage;
					if(s.health <= 0 && s.time_of_death == -2){

					s.texture = "sprites/explosion.png";
					s.frames = 20;
					s.time_of_death = -1;//SDL_GetTicks()/100;
					s.time_of_explosion = SDL_GetTicks()/100;
					}
				}
			}
			// cout << s.time_of_explosion <<"\n";
			
		}
		render_objects(objects);
		// check if exit
		gameRunning = check_exit();
		if(player.check_collision_asteroids(game_map, THRESHHOLD))
		{
			player.health -= 1;
			hit_asteroid(game_map, player.x, player.y, 30, 100);
		}
	}
	stop();
	return 0;
}
