/*
space ship classes
Hugh Smith
*/
#pragma once
#include <cmath>


class ship
{
public:
  // The ship class that every thing inherants from
  float pos[2]; // The position x, y
  float velosity[2]; // The the speed and rotation in radians
  int health; // The health
  
  float * move(float velosity[2], float pos[2])
  {
      // move the obj
      float horizontal = velosity[0] * cos(velosity[1]);
      float vertical = velosity[0] * sin(velosity[1]);

      float new_horizontal = horizontal + pos[0];
      float new_vertical = vertical + pos[1];

      float new_pos[2] = {new_horizontal, new_vertical};

      return new_pos;
  }
};