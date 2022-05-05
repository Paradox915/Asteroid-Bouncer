/*
player class
Hugh Smith
*/
#pragma once
#include "spaceships.hpp"
#include "../include/bullet.hpp"

class Player : public Ship
{
public:
    float player_speed;
    float max_speed;
    float min_speed;
    int score;
    Bullet shoot(int type);
    void accelerate(float add_value, double delta_time);

  // constructor
  Player(float x_input, float y_input, const char* texture_input, float rotation_input, float health_input, float max_speed_input, float min_speed_input):Ship(x_input, y_input, texture_input, 0, rotation_input, health_input){
      player_speed = min_speed_input;
      max_speed = max_speed_input;
      min_speed = min_speed_input;
      score = 0;
  }
};

Bullet Player::shoot(int type)
{
    /*shooting
    0 = lazers
    1 = canon*/
    Bullet bullet(x, y,"",10, rotation, 0, 0);
    if(type == 0)// lazers
    {
        bullet.damage = 0.5;
        bullet.range = 5;
        bullet.magnitude = 3.5;
        bullet.texture = "sprites/lazer.png";
    }else if(type == 1)// canon
    {
        bullet.damage = 3;
        bullet.range = 10;
        bullet.magnitude = 1;
        bullet.texture = "sprites/canon_ball.png";
    }
    cout << bullet.x<<" "<< bullet.y<<bullet.texture<<"\n";
    return bullet;
}

void Player::accelerate(float add_value, double delta_time)
{
    /*accelerate the ship*/
    if(player_speed + add_value <= max_speed && player_speed + add_value >= min_speed)
    {
        player_speed += add_value;
        magnitude = 0.5 * delta_time * log(player_speed+1);
        //cout << magnitude <<"-" <<player_speed <<"\n";
    }
}