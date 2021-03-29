#include "BoundingBox.h"

#include <algorithm>

bool BoundingBox::Intersect(glm::vec2 a_mousePosition)
{
    if (a_mousePosition.x < position.x + (size.x / 2.0f) &&
        a_mousePosition.x > position.x - (size.x / 2.0f) &&
        a_mousePosition.y < position.y + (size.y / 2.0f) &&
        a_mousePosition.y > position.y - (size.y / 2.0f))
    {
        return true;
    }

    return false;
}

void BoundingBox::Combine(const BoundingBox& a_box1, const BoundingBox& a_box2)
{
    glm::vec2 min(std::numeric_limits<float>::max());
    glm::vec2 max(std::numeric_limits<float>::lowest());

    for (int axis = 0; axis < 2; axis++)
    {
        min[axis] = std::min(
            a_box1.position[axis] - (a_box1.size[axis] / 2.0f),
            a_box2.position[axis] - (a_box2.size[axis] / 2.0f));

        max[axis] = std::max(
            a_box1.position[axis] + (a_box1.size[axis] / 2.0f),
            a_box2.position[axis] + (a_box2.size[axis] / 2.0f));
    }

    for (int axis = 0; axis < 2; axis++)
    {
        size[axis] = max[axis] - min[axis];
        position[axis] = min[axis] + (size[axis] / 2.0f);
    }
}

SDL_Rect BoundingBox::GetRect()
{
    SDL_Rect rect
    { position.x - (size.x / 2.0f), position.y - (size.y / 2.0f), size.x, size.y }; 
    return rect;
}
