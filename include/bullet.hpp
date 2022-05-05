/*
bullet classes
Hugh Smith
*/
#pragma once
#include <cmath>
#include "entity.hpp"

#define PI 3.14159265

class Bullet : public Entity
{
public:
  Entity move();
  Entity get_entity();
  // The Bullet class that every thing inherants from
  float magnitude; // The the speed
  float damage;
  int range;
  
  // constructor
  Bullet(float x_input, float y_input, const char* texture_input, float magnitude_input, float rotation_input, float damage_input, int range_input):Entity(x_input, y_input, rotation_input, texture_input){
    magnitude = magnitude_input;
    damage = damage_input;
    range = range_input;
  }
};

Entity Bullet::get_entity()
    {
      /*get an entity of the Bullet*/
      return Entity(x, y, rotation, texture);
    }

Entity Bullet::move()
{
    // move the obj
    float vertical = magnitude * cos(rotation*PI/180);
    float horizontal = -magnitude * sin(rotation*PI/180);
    //cout << horizontal << "\n";
    x =  x + horizontal;
    y = y + vertical;
    return get_entity();
}
