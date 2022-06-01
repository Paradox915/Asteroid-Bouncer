/*
space ship classes
Hugh Smith
*/
#pragma once
#include <cmath>
#include "entity.hpp"

#define PI 3.14159265

class Ship : public Entity
{
public:
  Entity move();
  Entity get_entity();
  // The ship class that every thing inherants from
  float magnitude; // The the speed
  int health; // The health
  
  // constructor
  Ship(float x_input, float y_input, const char* texture_input, float magnitude_input, float rotation_input, float health_input, bool animated_input = false):Entity(x_input, y_input, rotation_input, texture_input, animated_input){
    magnitude = magnitude_input;
    health = health_input;
  }
};

Entity Ship::get_entity()
    {
      /*get an entity of the ship*/
      return Entity(x, y, rotation, texture, animated);
    }

Entity Ship::move()
{
    // move the obj
    float vertical = magnitude * cos(rotation*PI/180);
    float horizontal = -magnitude * sin(rotation*PI/180);
    //cout << horizontal << "\n";
    x =  x + horizontal;
    y = y + vertical;
    return get_entity();
}