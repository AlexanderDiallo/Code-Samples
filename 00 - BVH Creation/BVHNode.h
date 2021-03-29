#pragma once

#include <SDL.h>
#include <vector>
#include "BoundingBox.h"
#include "Object.h"
#include <glm.hpp>
#include <memory>

class IBVH
{
public:
    ~IBVH() {}
    virtual bool Intersect(SDL_Renderer* renderer, glm::vec2 MousePosition) = 0;
};

class PointerBVHNode : public IBVH
{
public:

    PointerBVHNode(std::vector<std::shared_ptr<object_2d>>& a_objects);

    virtual bool Intersect(SDL_Renderer* renderer, glm::vec2 MousePosition) override;

private:

    BoundingBox m_boundingBox;
    std::shared_ptr<PointerBVHNode> m_pLeftNode = nullptr;
    std::shared_ptr<PointerBVHNode> m_pRightNode = nullptr;

    std::shared_ptr<object_2d> m_pObject = nullptr;

};

struct DataBVHNode
{
    BoundingBox boundingBox;
    std::shared_ptr<object_2d> pObject = nullptr;
};

class DataBVH : public IBVH
{
public:
    DataBVH(std::vector<std::shared_ptr<object_2d>>& a_objects);

    virtual bool Intersect(SDL_Renderer* renderer, glm::vec2 MousePosition) override;

private:
    void CreateNode(std::vector<std::shared_ptr<object_2d>>& a_objects, size_t a_nodeIndex);
    bool IntersectNode(SDL_Renderer* a_renderer, glm::vec2 a_mousePosition, size_t a_nodeIndex);

    std::vector<DataBVHNode> m_dataBVHNodes;
};


size_t GetLongestAxis(std::vector<std::shared_ptr<object_2d>>& pSurfaces);

void SplitObjectsAtMidpoint(const std::vector<std::shared_ptr<object_2d>>& a_pObjects,
    std::vector<std::shared_ptr<object_2d>>& a_pLeftNodeObjects,
    std::vector<std::shared_ptr<object_2d>>& a_pRightNodeObjects,
    size_t axis);

void SplitObjectsInHalf(std::vector<std::shared_ptr<object_2d>>& a_pObjects,
    std::vector<std::shared_ptr<object_2d>>& a_pLeftNodeObjects,
    std::vector<std::shared_ptr<object_2d>>& a_pRightNodeObjects,
    size_t axis);

bool CompareByPositionOnXAxis(std::shared_ptr<object_2d> a_pObject1, std::shared_ptr<object_2d> a_pObject2);
bool CompareByPositionOnYAxis(std::shared_ptr<object_2d> a_pObject1, std::shared_ptr<object_2d> a_pObject2);

size_t UpperPowerOfTwo(size_t a_value);