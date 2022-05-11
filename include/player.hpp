/*
player class
Hugh Smith
*/
#pragma once
#include <cmath>
#include "spaceships.hpp"
#include "../include/bullet.hpp"

class Player : public Ship
{
private:
    int left_right = 1;
    float lazer_x_offset = 18;
    float lazer_y_offset = 0;

public:
    float player_speed;
    float max_speed;
    float min_speed;
    int score;
    Bullet shoot();
    void accelerate(float add_value, double delta_time);

  // constructor
  Player(float x_input, float y_input, const char* texture_input, float rotation_input, float health_input, float max_speed_input, float min_speed_input):Ship(x_input, y_input, texture_input, 0, rotation_input, health_input){
      player_speed = min_speed_input;
      max_speed = max_speed_input;
      min_speed = min_speed_input;
      score = 0;
  }
};

Bullet Player::shoot()
{
    /*shooting*/
    Bullet bullet(x, y,"",10, rotation, 0, 0);
    float rad_rot = fmod(rotation, 360);
    rad_rot *= PI/180;
    bullet.damage = 0.5;
    bullet.range = 5;
    bullet.magnitude = 15+magnitude;
    bullet.texture = "sprites/lazer.png";
    bullet.x += sin(rad_rot-90) * lazer_x_offset * left_right;
    bullet.y += -cos(rad_rot-90) * lazer_x_offset * left_right;
    left_right *= -1;
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