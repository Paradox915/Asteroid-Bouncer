/*
enemy class
Hugh Smith
*/
#pragma once
#include <iostream>
#include <list>
#include <cmath>
#include "spaceships.hpp"
#include <Eigen/Dense>

using Eigen::Vector2f;
class Enemy : public Ship
{
private:
    std::list<Enemy> get_close_enemys(std::list<Enemy> others);
    float cohesion(std::list<Enemy> close_enemys);
    float alignment(std::list<Enemy> close_enemys);
    float separation(std::list<Enemy> close_enemys);
    float distance_between_enemys(Enemy one, Enemy two);

    float cohesion_strength = 0.01;
    float alingment_strength = 0.01;
    float seperation_strength = 0.01;

public:
    float vision_distance = 10000;
    void boids(std::list<Enemy> others);
  // constructor
  Enemy(float x_input, float y_input, const char* texture_input, float rotation_input, float health_input, float speed_input):Ship(x_input, y_input, texture_input, speed_input, rotation_input, health_input){
  }
};

void Enemy::boids(std::list<Enemy> others)
{
    /*
    do shit
    */
   std::list<Enemy> close = get_close_enemys(others);
}



std::list<Enemy> Enemy::get_close_enemys(std::list<Enemy> others)
{
    /*
    get all close ememys
    and return  to the list 
    */
   std::list<Enemy> close;

    for(Enemy enemy : others)
    {
        if(distance_between_enemys(enemy, *this) < vision_distance &&distance_between_enemys(enemy, *this) > 0)
        {
            close.push_front(enemy);
        }
    }
    return close;
}

float Enemy::distance_between_enemys(Enemy one, Enemy two)
{
    /*get the distance between 2 enemys*/
    return sqrt(pow(one.x - two.x, 2) + pow(one.y - two.y, 2));
}

float Enemy::cohesion(std::list<Enemy> close_enemys)
{
    /*
    caculate the force to apply based on cohesion 
    and return a pointer to the direction and magnitude.
    steer to move toward the average position of local flockmates
    */
   // get avarge position of the close fellas
   float average_x = 0;
   float average_y = 0;

   for(Enemy fello : close_enemys)
   {
       average_x += fello.x;
       average_y += fello.y;
   }

   average_x /= close_enemys.size();
   average_y /= close_enemys.size();

   Vector2f vector_to_average(x-average_x, y-average_y);
   return 0;
}
float Enemy::alignment(std::list<Enemy> close_enemys)
{
    /*
    caculate the force to apply based on cohesion 
    and return a pointer to the direction and magnitude.
    steer towards the average heading of local flockmates 
    */
   return 0;
}
float Enemy::separation(std::list<Enemy> close_enemys)
{
    /*
    caculate the force to apply based on cohesion 
    and return a pointer to the direction and magnitude.
    steer to avoid crowding local flockmates 
    */
   return 0;
}
