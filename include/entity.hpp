/*
entity classes
Hugh Smith
*/
#pragma once
#include <string>

class Entity
{
public:
    // the position
    float x, y;
    float rotation;
    // The texture
    const char* texture;

    // constructor
    Entity(float x_input, float y_input, float rot_input, const char* texture_input)
    {
        x = x_input;
        y = y_input;
        rotation = rot_input;
        texture = texture_input;
    }
};