/*
entity classes
Hugh Smith
*/
#pragma once
#include "../include/pixel_functions.hpp"
class Entity
{
    /*
    basic entity class that most objects will inherit from
    */
private:
    // the hitbox
    int hit_radius = 20;
public:
    // the position
    float x, y;
    float rotation;
    // The texture
    const char* texture;
    bool animated;
    int size;
    int frames;

    bool check_collision_asteroids(SDL_Surface* game_map, int threshold);
    bool check_collision(float other_x, float other_y);
    // constructor
    Entity(float x_input, float y_input, float rot_input, const char* texture_input, bool animated_input = false, int size_input = 16, int frames_input = 3)
    {
        animated = animated_input;
        x = x_input;
        y = y_input;
        rotation = rot_input;
        texture = texture_input;
        size = size_input;
        frames = frames_input;
    }
};

bool Entity::check_collision_asteroids(SDL_Surface* game_map, int threshold)
{
    /*
    check for a collision with the asteroids

    @param game map pointer as sdl surface, threshold as int
    @returns bool wether a collision should occure 
    */
    bool collision = false;

    if (get_pixel_darkness(game_map, x, y) < threshold)
    {
        collision = true;
    }

    return collision;
}

bool Entity::check_collision(float other_x, float other_y)
{
    /*
    check for a collision with a point
    @param x, y floats for the coord of the point to check
    @returns bool wether a collision should occure 
    */
    bool collision = false;
    if(hit_radius * hit_radius > (other_x - x) * (other_x - x)  + (other_y - y) * (other_y - y))
        collision = true;

    return collision;
}