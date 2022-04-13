/*
entity classes
Hugh Smith
*/
#pragma once
#include <cmath>
#include <string>

class Entity
{
public:
    // the position
    float x, y;
    // The texture
    std::string texture;

    // constructor
    Entity(float x_input, float y_input, std::string texture_input)
    {
        x = x_input;
        y = y_input;
        texture = texture_input;
    }
};