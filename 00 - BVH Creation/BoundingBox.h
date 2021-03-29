#pragma once

#include <glm.hpp>
#include <SDL.h>

struct BoundingBox
{
    glm::vec2 position;
    glm::vec2 size;

    bool Intersect(glm::vec2 a_mousePosition);
    void Combine(const BoundingBox& a_box1, const BoundingBox& a_box2);

    SDL_Rect GetRect();
};

