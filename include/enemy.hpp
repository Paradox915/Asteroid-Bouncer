/*
enemy class
Hugh Smith
*/
#pragma once
#include <iostream>
#include <list>
#include <cmath>
#include "spaceships.hpp"

class Enemy : public Ship
{
private:
    std::list<Enemy> get_close_enemys();
    float* cohesion(std::list<Enemy> close_enemys);
    float* alignment(std::list<Enemy> close_enemys);
    float* separation(std::list<Enemy> close_enemys);
public:
    float speed;
    void boids();
  // constructor
  Enemy(float x_input, float y_input, const char* texture_input, float rotation_input, float health_input, float speed_input):Ship(x_input, y_input, texture_input, 0, rotation_input, health_input){
      speed = speed_input;
  }
};

void Enemy::boids()
{
    /*
    do shit
    */
}



std::list<Enemy> Enemy::get_close_enemys()
{
    /*
    get all close ememys
    and return  to the list 
    */
}
float* Enemy::cohesion(std::list<Enemy> close_enemys)
{
    /*
    caculate the force to apply based on cohesion 
    and return a pointer to the direction and magnitude.
    steer to move toward the average position of local flockmates
    */
}
float* Enemy::alignment(std::list<Enemy> close_enemys)
{
    /*
    caculate the force to apply based on cohesion 
    and return a pointer to the direction and magnitude.
    steer towards the average heading of local flockmates 
    */
}
float* Enemy::separation(std::list<Enemy> close_enemys)
{
    /*
    caculate the force to apply based on cohesion 
    and return a pointer to the direction and magnitude.
    steer to avoid crowding local flockmates 
    */
}