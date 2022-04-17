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
  // The ship class that every thing inherants from
  float magnitude; // The the speed
  // direction in degrees
  float direction;
  int health; // The health
  
  void move()
  {
      // move the obj
      float vertical = magnitude * cos(direction*PI/180);
      float horizontal = magnitude * sin(direction*PI/180);
      x = horizontal + x;
      y = -vertical + y;
  }

  Entity get_entity()
    {
      /*get an entity of the ship*/
      return Entity(x, y, direction, texture);
    }

  // constructor
  Ship(float x_input, float y_input, const char* texture_input, float magnitude_input, float rotation_input, float health_input):Entity(x_input, y_input, rotation_input, texture_input){
    magnitude = magnitude_input;
    direction = rotation_input;
    health = health_input;
  }

  
};