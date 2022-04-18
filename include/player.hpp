/*
player class
Hugh Smith
*/
#pragma once
#include "spaceships.hpp"


class Player : public Ship
{
public:
    int player_speed;
    float max_speed;
    float min_speed;
    void shoot();
    void accelerate(float add_value);

  // constructor
  Player(float x_input, float y_input, const char* texture_input, float rotation_input, float health_input, float max_speed_input, float min_speed_input):Ship(x_input, y_input, texture_input, 0, rotation_input, health_input){
      player_speed = 0;
      max_speed = max_speed_input;
      min_speed = min_speed_input;
  }
};

void Player::shoot()
{
    /*shooting*/
}

void Player::accelerate(float add_value)
{
    /*accelerate the ship*/
    if(player_speed + add_value <= max_speed && player_speed + add_value >= min_speed)
    {
        player_speed += add_value;
        magnitude = log(player_speed+1);
    }
}