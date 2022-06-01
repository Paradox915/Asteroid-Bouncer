/*
entity classes
Hugh Smith
*/
#pragma once
#include "../include/pixel_functions.hpp"
class Entity
{
public:
    // the position
    float x, y;
    float rotation;
    // The texture
    const char* texture;
    bool animated;

    bool check_collision_asteroids(SDL_Surface* game_map, int threshold);
    // constructor
    Entity(float x_input, float y_input, float rot_input, const char* texture_input, bool animated_input = false)
    {
        animated = animated_input;
        x = x_input;
        y = y_input;
        rotation = rot_input;
        texture = texture_input;
    }
};

bool Entity::check_collision_asteroids(SDL_Surface* game_map, int threshold)
{
    /*check for a collision with the asteroids*/
    bool collision = false;

    if (get_pixel_darkness(game_map, x, y) < threshold)
    {
        collision = true;
    }

    return collision;
}