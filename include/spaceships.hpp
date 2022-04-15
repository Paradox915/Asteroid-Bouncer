/*
space ship classes
Hugh Smith
*/
#pragma once
#include <cmath>

#include "entity.hpp"

class Ship : public Entity
{
public:
  // The ship class that every thing inherants from
  float magnitude; // The the speed
  // direction in radians
  float direction;
  int health; // The health
  
  float move(float magnitude, float direction, float x, float y)
  {
      // move the obj
      float horizontal = magnitude * cos(direction);
      float vertical = magnitude * sin(direction);

      float new_x = horizontal + x;
      float new_y = vertical + y;

      return (new_x, new_y);
  }

  // constructor
  Ship(float x_input, float y_input, std::string texture_input, float magnitude_input, float rotation_input, float health_input)
  {
    x = x_input;
    y = y_input;
    texture = texture_input;
    magnitude = magnitude_input;
    direction = rotation_input;
    health = health_input;
  }
};