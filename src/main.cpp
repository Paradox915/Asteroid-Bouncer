/*
The main file for the game.
Hugh Smith
*/

// includes - libarys
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
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

// the max ammount of enemys
int max_enemys = 80;
int spawn_distance = 80;
int max_distance_from_player = 1000;
// "x y", add_value
std::map<string, int> perlin_add_values { {"0 0", 10}, {"1 0", 10}, {"0 1", 10}, };
int player_health = 100;
int player_x = 900;
int player_y = 550;
Player player(player_x,player_y,"sprites/player/funky_ship_sprtesheet (1).png",0,player_health, 1.8, 0.2);

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Surface* surface = NULL; 
SDL_Texture* tex = NULL;


TTF_Font* Sans;

const Uint8* kb;

// noise
int music_channel_volume = 20;
int explosion_channel = 2;
int explosion_channel_volume = 10;
int shoot_channel = 3;
int shoot_channel_volume = 5;

//The music that will be played
Mix_Music *Music_sound_game = NULL;
Mix_Music *Music_sound_menue = NULL;

//The sound effects that will be used
// enemys
Mix_Chunk *explosion_sound = NULL;
Mix_Chunk *explosion_wall_sound = NULL;

// player
Mix_Chunk *hit_sound = NULL;
Mix_Chunk *shoot_sound = NULL;

// menue
Mix_Chunk *click_sound = NULL;

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

	Sans = TTF_OpenFont("font.ttf", 15);
	if ( !Sans ) {
		cout << "Failed to load font: " << TTF_GetError() << endl;
		result = false;
	}

	//Initialize SDL_mixer
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
	{
		printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
		result = false;
	}

	return result;
}

bool load_soundeffects()
{
    bool result = true;

    //Load music
    Music_sound_game = Mix_LoadMUS("audio/music/i_ruied_nit_finished_.mp3");
    if( Music_sound_game == NULL )
    {
        printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
        result = false;
    }

    Music_sound_menue = Mix_LoadMUS("audio/music/high_brow_fin.mp3");
    if( Music_sound_menue == NULL )
    {
        printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
        result = false;
    }
    //Load sound effects
    explosion_sound = Mix_LoadWAV("audio/explosion.wav");
    if( explosion_sound == NULL )
    {
        printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        result = false;
    }

	explosion_wall_sound = Mix_LoadWAV("audio/explosion_collision.wav");
    if( explosion_wall_sound == NULL )
    {
        printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        result = false;
    }

	hit_sound = Mix_LoadWAV("audio/player/hitHurt.wav");
    if( hit_sound == NULL )
    {
        printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        result = false;
    }

	shoot_sound = Mix_LoadWAV("audio/player/laserShoot.wav");
    if( shoot_sound == NULL )
    {
        printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        result = false;
    }

	click_sound = Mix_LoadWAV("audio/menue/blipSelect.wav");
    if( click_sound == NULL )
    {
        printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        result = false;
    }


	// change volumes
	Mix_VolumeMusic(music_channel_volume);
	Mix_Volume(explosion_channel, explosion_channel_volume);
	Mix_Volume(shoot_channel, shoot_channel_volume);
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
			if(num != 15 && num != 0){
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
void render_background(Entity ent)
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
	dest.w *= 0.6; 
	dest.h *= 0.6; 
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

void render_json_score(string score)
{

	SDL_Color White = {156, 53, 73};

	string s_score = score;
	SDL_Surface* surfaceMessage =
	TTF_RenderText_Blended_Wrapped(Sans, s_score.c_str() , White, 0);

	// now you can convert it into a texture
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_Rect Message_rect; //create a rect
	Message_rect.x = 250;  //controls the rect's x coordinate 
	Message_rect.y = 400; // controls the rect's y coordinte
	Message_rect.w = 150; // controls the width of the rect
	Message_rect.h = 250; // controls the height of the rect


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
	// save the map
	//SDL_SaveBMP(game_map, map_file);

}

list<Enemy> spawn_enemys(list<Enemy> enemys, int max_enemys, Player player)
{
	/*
	Function for spawing enemys

	@takes list of enemys, max size, the player   
	@returns list of enemys
	*/

	int enemys_to_spawn = max_enemys - enemys.size();

	// loop though and spawn all of the desired enemys'
	for(int i = 0; i < enemys_to_spawn; i++)
	{
		// pick position that is not in an asteroid
		int x_pos;
		int y_pos;
		bool pos_found = false;
		while(pos_found == false)
		{
			// pick a random coord that is some distance from the player
			// pick random distance away for x and y
			int x_random = rand() % spawn_distance + WIDTH/2;
			int y_random = rand() % spawn_distance + HEIGHT/2;
			// pick sign eg + or - for the x and y
			int x_sign = rand() %2 -1;
			if(x_sign == 0)
				x_sign = 1;
			
			int y_sign = rand() %2 -1;
			if(y_sign == 0)
				y_sign = 1;

			// get the relitive pos
			x_pos = player.x + (x_sign * x_random);
			y_pos = player.y + (y_sign * y_random);

			// check if the pos is valid
			pos_found = (get_pixel_darkness(game_map, x_pos, y_pos) > THRESHHOLD);
		}
		enemys.push_front(Enemy(x_pos,y_pos,"sprites/enemy_sprite_sheet.png",0,1, 2));
	}
	return enemys;
}

bool rungame()
{
	list<Enemy> enemys;
	bool gameRunning = true;
	bool continue_game = true;
	Uint64 start_frame = 0;
	Uint64 end_frame;
	list<Bullet> bullets;
	Uint32 time = 0;
	//Play the music
	Mix_PlayMusic(Music_sound_game, -1);

	while(gameRunning == true)
	{
		// The main game loop
		list<Entity> objects = {};
		frame_last = frame_now;
   		frame_now = SDL_GetPerformanceCounter();
   		delta_time = (double)((frame_now-frame_last)*1000 / (double)SDL_GetPerformanceFrequency());
		/*----------------------music----------------------------*/
		if( Mix_PlayingMusic() == 0 )
		{
			//Play the music
			Mix_PlayMusic(Music_sound_game, -1);
		}
		/*----------------------get any inputs from the user----------------------*/
		SDL_PumpEvents();
		kb = SDL_GetKeyboardState(NULL);
		if(kb[SDL_SCANCODE_SPACE] && time < SDL_GetTicks()){
			time = SDL_GetTicks() + 100;
			Bullet current_bullet = player.shoot();
			bullets.push_front(current_bullet);
			// play sound effect
			Mix_PlayChannel(shoot_channel, shoot_sound, 0);
		}
		if(kb[SDL_SCANCODE_DOWN] || kb[SDL_SCANCODE_S])
			player.accelerate(-0.03, delta_time);
		if(kb[SDL_SCANCODE_UP] || kb[SDL_SCANCODE_W])
			player.accelerate(0.03, delta_time);
		if(kb[SDL_SCANCODE_LEFT] || kb[SDL_SCANCODE_A])
			player.rotation -= 0.2*delta_time;
		if(kb[SDL_SCANCODE_RIGHT] || kb[SDL_SCANCODE_D])
			player.rotation += 0.2*delta_time;
		if(kb[SDL_SCANCODE_ESCAPE])
			player.magnitude = 0;
		if(kb[SDL_SCANCODE_EQUALS]&& GRID_SIZE < 80)
			GRID_SIZE += 1;
		if(kb[SDL_SCANCODE_MINUS] && GRID_SIZE > 16)
			GRID_SIZE -= 1;



		/*----------------------checks to do with enemys and bullets----------------------*/
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

			// broken
			if(max_distance_from_player * max_distance_from_player < (s.x - player.x) * (s.x - player.x)  + (s.y - player.y) * (s.y - player.y))
			{
				i = enemys.erase(i);
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
				Mix_PlayChannel(explosion_channel, explosion_wall_sound, 0);
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
				Mix_PlayChannel(explosion_channel, hit_sound, 0);
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
					Mix_PlayChannel(explosion_channel, explosion_sound, 0);
					}
				}
			}
			
		}

		/*----------------------spawing of enemys----------------------*/
		enemys = spawn_enemys(enemys, max_enemys, player);

		/*----------------------render everything to the screen----------------------*/
		render_objects(objects);
		// check if exit
		continue_game = check_exit();
		gameRunning = continue_game;
		//cout << continue_game;
		if(player.health <= 0)
			gameRunning = false;
		if(player.check_collision_asteroids(game_map, THRESHHOLD))
		{
			player.health -= 1;
			hit_asteroid(game_map, player.x, player.y, 30, 100);
		}
	}
	player.x = player_x;
	player.y = player_y;
	player.health = player_health;
	player.magnitude = 1;
	return continue_game;
}

int main(int argc, char* args[])
{
	bool running = (init_sdl() && load_soundeffects());
	// menue screen
	Entity back_ground(player_x,player_y,0,"sprites/menues/mainmenue.png");
	string output = "";
	while(running == true)
	{
		if( Mix_PlayingMusic() == 0 )
		{
			//Play the music
			Mix_PlayMusic(Music_sound_menue, -1);
		}
		SDL_RenderClear(renderer); 
		render_background(back_ground);
		render_json_score(output);
		SDL_RenderPresent(renderer);
		SDL_PumpEvents();
		kb = SDL_GetKeyboardState(NULL);
		if(kb[SDL_SCANCODE_RETURN]){
			// play sound effect
			Mix_PlayChannel(-1, click_sound, 0);
			running = rungame();
			// add the player score
			player_scores_json(player.score);
		}else if(kb[SDL_SCANCODE_1]){
			// play sound effect
			Mix_PlayChannel(-1, click_sound, 0);
			back_ground.texture = "sprites/menues/credits.png";
			output = "";
		}else if(kb[SDL_SCANCODE_2]){
			// play sound effect
			Mix_PlayChannel(-1, click_sound, 0);
			back_ground.texture = "sprites/menues/scores.png";
			ifstream i("database/player_scores.json", std::ifstream::binary);
			Json::Value j;
			i >> j;	
			output = "";
			for (auto it = j.begin(); it != j.end(); ++it)
			{
				output += it.key().asString() + "\n";
				output += "    -" + (*it).asString() + "\n\n";
			}
			
		}else if(kb[SDL_SCANCODE_3]){
			// play sound effect
			Mix_PlayChannel(-1, click_sound, 0);
			back_ground.texture = "sprites/menues/mainmenue.png";
			output = "";
		}	
		
	}
	stop();
	return 0;
}
