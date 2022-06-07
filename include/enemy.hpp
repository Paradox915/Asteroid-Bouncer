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
    void asteroid_avoidence(float average_x, float average_y);
    void player_chase(Player player);
    float distance_between_enemys(Enemy one, Enemy two);

    float cohesion_strength = 0.001;
    float cohesion_distance = 150;
    float alingment_strength = 0.7;
    float alingment_distance = 60;
    float seperation_strength = 7;
    float seperation_distance = 40;

    float asteroid_strength = 3;
    float asteroid_distance = 10;

    float player_chasestrength = 0.03;//0;


public:
    void boids(std::list<Enemy> others, Player player, std::list<float> x_asteroids, std::list<float> y_asteroids);

    int time_of_death = -2;
    int time_of_explosion = -2;
  // constructor
  Enemy(float x_input, float y_input, const char* texture_input, float rotation_input, float health_input, float speed_input, bool animated_input = true):Ship(x_input, y_input, texture_input, speed_input, rotation_input, health_input, animated_input){
  }
};

void Enemy::boids(std::list<Enemy> others, Player player, std::list<float> x_asteroids, std::list<float> y_asteroids)
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

    // asteroid avidance
    float average_x;
    float average_y;
    int close_size = 0;
    auto index_x = x_asteroids.begin();
    auto index_y = y_asteroids.begin();
    for(int i = 0; i < x_asteroids.size(); i++)
    {
        float distance_between = sqrt(pow(x - *index_x, 2) + pow(y - *index_y, 2)); 
        if(distance_between < asteroid_distance)
        {
            close_size ++;
            average_x += *index_x;
            average_y += *index_y;
        }

        advance(index_x, 1);
        advance(index_y, 1);
    }
    average_x /= close_size;
    average_y /= close_size;
    asteroid_avoidence(average_x, average_y);

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

    float angle = atan2(y_delta, x_delta);
    angle = rotation - (-angle * (180/PI) + 90);

    if(angle > 180){
        rotation += seperation_strength;
    }else{
        rotation -= seperation_strength;
    }
    
    // if(x_delta < 0){
    //     x_delta = -1;
    // }else
    // {
    //     x_delta = 1;
    // }

    // if(y_delta < 0){
    //     y_delta = -1;
    // }else
    // {
    //     y_delta = 1;
    // }
    
    // x += x_delta*seperation_strength;
    // y += y_delta*seperation_strength;
}

void Enemy::asteroid_avoidence(float average_x, float average_y)
{
    /*
    caculate the force to apply based on cohesion 
    and return a pointer to the direction and magnitude.
    steer to avoid crowding local flockmates 

    @params list of enemys
    @returns void
    */

    //cout << average_x << ",- " << average_y << "\n";
    float x_delta = x - average_x;
    float y_delta = y - average_y;

    float angle = atan2(y_delta, x_delta);
    angle = rotation - (-angle * (180/PI) + 90);

    if(angle > 180){
        rotation += asteroid_strength;
    }else{
        rotation -= asteroid_strength;
    }
    
    // if(x_delta < 0){
    //     x_delta = -1;
    // }else
    // {
    //     x_delta = 1;
    // }

    // if(y_delta < 0){
    //     y_delta = -1;
    // }else
    // {
    //     y_delta = 1;
    // }
    
    // x += x_delta*seperation_strength;
    // y += y_delta*seperation_strength;
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