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
    /*
    Enemy class for the basic enimys
    inherits from spaceship class
    */
private:
    std::list<Enemy> get_close_enemys(std::list<Enemy> others);
    void cohesion(std::list<Enemy> close_enemys);
    void alignment(std::list<Enemy> close_enemys);
    void separation(std::list<Enemy> close_enemys);
    void player_chase(Player player);
    float distance_between_enemys(Enemy one, Enemy two);

    float cohesion_strength = 0.1;
    float cohesion_distance = 150;
    float alingment_strength = 0.07;
    float alingment_distance = 40;
    float seperation_strength = 20;
    float seperation_distance = 70;

    float player_chasestrength = 0.3;


public:
    void boids(std::list<Enemy> others, Player player);

    int time_of_death = -2;
    int time_of_explosion = -2;
  // constructor
  Enemy(float x_input, float y_input, const char* texture_input, float rotation_input, float health_input, float speed_input, bool animated_input = true):Ship(x_input, y_input, texture_input, speed_input, rotation_input, health_input, animated_input){
  }
};

void Enemy::boids(std::list<Enemy> others, Player player)
{
    /*
    The main boids function for the flocking movment of the enemys

    @param list of all enemys, the player
    @returns void
    */
   /*
    get all close ememys
    and return the lists 
    */
   std::list<Enemy> cohesion_list;
   std::list<Enemy> seperation_list;
   std::list<Enemy> alingment_list;

    for(Enemy enemy : others)
    {
        float distance = distance_between_enemys(enemy, *this);
        if(distance > 0)
        {
            // force apart
            if(distance < seperation_distance)
            {
                seperation_list.push_back(enemy);
            }else if(distance < cohesion_distance) // together
            {
                cohesion_list.push_back(enemy);
            }
            if(distance < alingment_distance) // same direction
            {
                alingment_list.push_back(enemy);
            }
        }
    }

    float start_x = x;
    float start_y = y;

    if(seperation_list.size() != 0)
        separation(seperation_list);

    if(alingment_list.size() != 0)
        alignment(alingment_list);
    
    if(cohesion_list.size() != 0)
        cohesion(cohesion_list);
    
    move();
    player_chase(player);

    // caculate new rotation based off the movement

    float angle = atan2(start_y-y,start_x-x);
    rotation = angle * (180/PI) + 90;
    //int rand_angle = rand() % 10 + -10;
    //rotation +=rand_angle;
   
}

float Enemy::distance_between_enemys(Enemy one, Enemy two)
{
    /*
    get the distance between two enemys

    @param enemys * 2
    @returns distance between the 2 as a float
    */
    return sqrt(pow(one.x - two.x, 2) + pow(one.y - two.y, 2));
}

void Enemy::cohesion(std::list<Enemy> close_enemys)
{
    /*
    caculate the force to apply based on cohesion 
    and return a pointer to the direction and magnitude.
    steer to move toward the average position of local flockmates

    @param list of enemys
    @returns void
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
    //cout << average_x << ",- " << average_y << "\n";
    float x_delta = x - average_x;
    float y_delta = y - average_y;
    
    if(x_delta < 0){
        x_delta = -1;
    }else
    {
        x_delta = 1;
    }

    if(y_delta < 0){
        y_delta = -1;
    }else
    {
        y_delta = 1;
    }
    
    x -= x_delta*cohesion_strength;
    y -= y_delta*cohesion_strength;
}
void Enemy::alignment(std::list<Enemy> close_enemys)
{
    /*
    caculate the force to apply based on cohesion 
    and return a pointer to the direction and magnitude.
    steer towards the average heading of local flockmates 

    @params list of enemys
    @returns void
    */

    float avarge_rot;

    for(Enemy fello : close_enemys)
    {
        avarge_rot += fello.rotation;
    }

    avarge_rot /= close_enemys.size();


    if(avarge_rot > rotation)
    {
        rotation += alingment_strength;
    }else
    {
        rotation -= alingment_strength;
    }

}
void Enemy::separation(std::list<Enemy> close_enemys)
{
    /*
    caculate the force to apply based on cohesion 
    and return a pointer to the direction and magnitude.
    steer to avoid crowding local flockmates 

    @params list of enemys
    @returns void
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
    //cout << average_x << ",- " << average_y << "\n";
    float x_delta = x - average_x;
    float y_delta = y - average_y;
    
    if(x_delta < 0){
        x_delta = -1;
    }else
    {
        x_delta = 1;
    }

    if(y_delta < 0){
        y_delta = -1;
    }else
    {
        y_delta = 1;
    }
    
    x += x_delta*seperation_strength;
    y += y_delta*seperation_strength;
}

void Enemy::player_chase(Player player)
{
    /*
    caculate the force to apply based on cohesion 
    and return a pointer to the direction and magnitude.
    steer to avoid crowding local flockmates 

    @prams player
    @returns void
    */

    // get avarge position of the close fellas
    float average_x = player.x;
    float average_y = player.y;

    //cout << average_x << ",- " << average_y << "\n";
    float x_delta = x - average_x;
    float y_delta = y - average_y;
    
    if(x_delta < 0){
        x_delta = -1;
    }else
    {
        x_delta = 1;
    }

    if(y_delta < 0){
        y_delta = -1;
    }else
    {
        y_delta = 1;
    }
    
    x -= x_delta*player_chasestrength;
    y -= y_delta*player_chasestrength;
}