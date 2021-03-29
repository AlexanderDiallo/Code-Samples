#include "BVHNode.h"

#include <algorithm>
#include <cassert>
#include <cmath>

PointerBVHNode::PointerBVHNode(std::vector<std::shared_ptr<object_2d>>& a_pObjects)
    : m_pLeftNode(nullptr)
    , m_pRightNode(nullptr)
    , m_pObject(nullptr)
{
    assert(a_pObjects.size() > 0);

    std::vector<std::shared_ptr<object_2d>> pLeftNodeObjects;
    std::vector<std::shared_ptr<object_2d>> pRightNodeObjects;

    if (a_pObjects.size() == 1)
    {
        m_pObject = a_pObjects[0];
        m_boundingBox = m_pObject->bbox;
    }
    else if (a_pObjects.size() == 2)
    {
        pLeftNodeObjects.push_back(a_pObjects[0]);
        m_pLeftNode = std::make_shared<PointerBVHNode>(pLeftNodeObjects);

        pRightNodeObjects.push_back(a_pObjects[1]);
        m_pRightNode = std::make_shared<PointerBVHNode>(pRightNodeObjects);

        m_boundingBox.Combine(a_pObjects[0]->bbox, a_pObjects[1]->bbox);
    }
    else
    {
        size_t longestAxis = GetLongestAxis(a_pObjects);
        /*SplitObjectsAtMidpoint(a_pObjects,
            pLeftNodeObjects,
            pRightNodeObjects,
            longestAxis);*/
        SplitObjectsInHalf(
            a_pObjects,
            pLeftNodeObjects,
            pRightNodeObjects,
            longestAxis
        );

        if (pLeftNodeObjects.size() != 0)
        {
            m_pLeftNode = std::make_shared<PointerBVHNode>(pLeftNodeObjects);
        }
        if (pRightNodeObjects.size() != 0)
        {
            m_pRightNode = std::make_shared<PointerBVHNode>(pRightNodeObjects);
        }

        if (m_pLeftNode == nullptr)
        {
            m_boundingBox = m_pRightNode->m_boundingBox;
        }
        else if (m_pRightNode == nullptr)
        {
            m_boundingBox = m_pLeftNode->m_boundingBox;
        }
        else
        {
            m_boundingBox.Combine(m_pLeftNode->m_boundingBox, m_pRightNode->m_boundingBox);
        }

    }
}

bool PointerBVHNode::Intersect(SDL_Renderer* a_renderer, glm::vec2 a_mousePosition)
{
    if (m_boundingBox.Intersect(a_mousePosition))
    {
        if (a_renderer != nullptr) { SDL_RenderDrawRect(a_renderer, &m_boundingBox.GetRect()); }
        if (m_pLeftNode == nullptr && m_pRightNode == nullptr)
        {
            assert(m_pObject != nullptr);
            if (m_pObject->bbox.Intersect(a_mousePosition))
            {
                if (a_renderer != nullptr) { SDL_RenderDrawRect(a_renderer, &m_pObject->bbox.GetRect()); }
                return true;
            }
            else
            {
                return false;
            }
        }

        if (m_pLeftNode->Intersect(a_renderer, a_mousePosition) == true)
        {
            return true;
        }

        if (m_pRightNode != nullptr && m_pRightNode->Intersect(a_renderer, a_mousePosition) == true)
        {
            return true;
        }

    }

    return false;
}

size_t GetLongestAxis(std::vector<std::shared_ptr<object_2d>>& a_pObjects)
{
    assert(a_pObjects.size() != 0);
        
    glm::vec2 minExtents(
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max()
    );

    glm::vec2 maxExtents(
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::lowest()
    );

    for (const auto& pObject : a_pObjects)
    {
        for (int axis = 0; axis < 2; axis++)
        {
            minExtents[axis] = std::min(minExtents[axis], pObject->bbox.position[axis] - (pObject->bbox.size[axis] / 2.0f));
            maxExtents[axis] = std::max(maxExtents[axis], pObject->bbox.position[axis] + (pObject->bbox.size[axis] / 2.0f));
        }
    }

    glm::vec2 lengthByAxis(
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::lowest()
    );

    for (int axis = 0; axis < 2; axis++)
    {
        lengthByAxis[axis] = maxExtents[axis] - minExtents[axis];
    }

    size_t longestAxis = 2;
    float longestLength = std::numeric_limits<float>::min();

    for (int axis = 0; axis < 2; axis++)
    {
        if (longestLength < lengthByAxis[axis])
        {
            longestAxis = axis;
            longestLength = lengthByAxis[axis];
        }
    }

    assert(longestAxis != 2);

    return longestAxis;
}

void SplitObjectsAtMidpoint(const std::vector<std::shared_ptr<object_2d>>& a_pObjects,
    std::vector<std::shared_ptr<object_2d>>& a_pLeftNodeObjects,
    std::vector<std::shared_ptr<object_2d>>& a_pRightNodeObjects,
    size_t axis)
{
    assert(a_pObjects.size() > 0);
    assert(a_pLeftNodeObjects.size() == 0 && a_pRightNodeObjects.size() == 0);
    assert(axis >= 0 && axis < 2);

    glm::vec2 extents(
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::lowest()
    );

    for (const auto& pObject : a_pObjects)
    {
        extents[0] = std::min(extents[0], pObject->position[axis]);
        extents[1] = std::max(extents[1], pObject->position[axis]);
    }

    const float midpoint = (extents[0] + extents[1]) / 2.0f;

    for (const auto& pObject : a_pObjects)
    {
        if (pObject->position[axis] <= midpoint)
        {
            a_pLeftNodeObjects.push_back(pObject);
        }
        else
        {
            a_pRightNodeObjects.push_back(pObject);
        }
    }
}

void SplitObjectsInHalf(std::vector<std::shared_ptr<object_2d>>& a_pObjects, std::vector<std::shared_ptr<object_2d>>& a_pLeftNodeObjects, std::vector<std::shared_ptr<object_2d>>& a_pRightNodeObjects, size_t axis)
{
    assert(a_pObjects.size() > 0);
    assert(a_pLeftNodeObjects.size() == 0 && a_pRightNodeObjects.size() == 0);
    assert(axis >= 0 && axis < 2);

    if (axis == 0)
    {
        sort(a_pObjects.begin(), a_pObjects.end(), CompareByPositionOnXAxis);
    }
    else
    {
        sort(a_pObjects.begin(), a_pObjects.end(), CompareByPositionOnYAxis);
    }

    size_t middle = a_pObjects.size() / 2;

    for (int i = 0; i < a_pObjects.size(); i++)
    {
        if (i < middle)
        {
            a_pLeftNodeObjects.push_back(a_pObjects[i]);
        }
        else
        {
            a_pRightNodeObjects.push_back(a_pObjects[i]);
        }
    }
}

bool CompareByPositionOnXAxis(std::shared_ptr<object_2d> pObject1, std::shared_ptr<object_2d> pObject2)
{
    return pObject1->bbox.position[0] < pObject2->bbox.position[0];
}

bool CompareByPositionOnYAxis(std::shared_ptr<object_2d> pObject1, std::shared_ptr<object_2d> pObject2)
{
    return pObject1->bbox.position[1] < pObject2->bbox.position[1];
}

size_t UpperPowerOfTwo(size_t a_value)
{
    unsigned long v = static_cast<unsigned long>(a_value);
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return static_cast<size_t>(v);

}

DataBVH::DataBVH(std::vector<std::shared_ptr<object_2d>>& a_objects)
{
    size_t numberOfNodes = UpperPowerOfTwo(a_objects.size()) * 2 - 1;
    m_dataBVHNodes.resize(numberOfNodes);

    CreateNode(a_objects, 0);
}

bool DataBVH::Intersect(SDL_Renderer* a_renderer, glm::vec2 a_mousePosition)
{
    return IntersectNode(a_renderer, a_mousePosition, 0);
}

void DataBVH::CreateNode(std::vector<std::shared_ptr<object_2d>>& a_pObjects, size_t a_nodeIndex)
{
    assert(m_dataBVHNodes.size() > 0);
    assert(a_pObjects.size() > 0);

    size_t leftChildNodeIndex = 2 * a_nodeIndex + 1;
    size_t rightChildNodeIndex = 2 * a_nodeIndex + 2;

    std::vector<std::shared_ptr<object_2d>> pLeftNodeObjects;
    std::vector<std::shared_ptr<object_2d>> pRightNodeObjects;

    if (a_pObjects.size() == 1)
    {
        assert(a_nodeIndex <= m_dataBVHNodes.size());

        m_dataBVHNodes[a_nodeIndex].pObject = a_pObjects[0];
        m_dataBVHNodes[a_nodeIndex].boundingBox = a_pObjects[0]->bbox;
    }
    else if (a_pObjects.size() == 2)
    {
        assert(leftChildNodeIndex <= m_dataBVHNodes.size());
        assert(rightChildNodeIndex <= m_dataBVHNodes.size());

        pLeftNodeObjects.push_back(a_pObjects[0]);
        CreateNode(pLeftNodeObjects, leftChildNodeIndex);

        pRightNodeObjects.push_back(a_pObjects[1]);
        CreateNode(pRightNodeObjects, rightChildNodeIndex);

        m_dataBVHNodes[a_nodeIndex].boundingBox.Combine(
            m_dataBVHNodes[leftChildNodeIndex].boundingBox,
            m_dataBVHNodes[rightChildNodeIndex].boundingBox
        );
    }
    else
    {
        size_t longestAxis = GetLongestAxis(a_pObjects);
        /*SplitObjectsAtMidpoint(a_pObjects,
            pLeftNodeObjects,
            pRightNodeObjects,
            longestAxis);*/
        SplitObjectsInHalf(
            a_pObjects,
            pLeftNodeObjects,
            pRightNodeObjects,
            longestAxis
        );

        if (pLeftNodeObjects.size() != 0)
        {
            CreateNode(pLeftNodeObjects, leftChildNodeIndex);
        }
        if (pRightNodeObjects.size() != 0)
        {
            CreateNode(pRightNodeObjects, rightChildNodeIndex);
        }
     
        m_dataBVHNodes[a_nodeIndex].boundingBox.Combine(
            m_dataBVHNodes[rightChildNodeIndex].boundingBox, 
            m_dataBVHNodes[leftChildNodeIndex].boundingBox
        );        
    }
}

bool DataBVH::IntersectNode(SDL_Renderer* a_renderer, glm::vec2 a_mousePosition, size_t a_nodeIndex)
{
    size_t leftChildNodeIndex = 2 * a_nodeIndex + 1;
    size_t rightChildNodeIndex = 2 * a_nodeIndex + 2;

    if (m_dataBVHNodes[a_nodeIndex].boundingBox.Intersect(a_mousePosition))
    {
        if (a_renderer != nullptr) {
            SDL_RenderDrawRect(a_renderer, &m_dataBVHNodes[a_nodeIndex].boundingBox.GetRect());
        }

        if (m_dataBVHNodes[a_nodeIndex].pObject != nullptr &&
            leftChildNodeIndex < m_dataBVHNodes.size())
        {
            if (m_dataBVHNodes[a_nodeIndex].pObject->bbox.Intersect(a_mousePosition))
            {
                if (a_renderer != nullptr) {
                    SDL_RenderDrawRect(a_renderer, &m_dataBVHNodes[a_nodeIndex].pObject->bbox.GetRect());
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        if (rightChildNodeIndex < m_dataBVHNodes.size() &&
            IntersectNode(a_renderer, a_mousePosition, leftChildNodeIndex) == true)
        {
            return true;
        }

        if (rightChildNodeIndex < m_dataBVHNodes.size() &&
            IntersectNode(a_renderer, a_mousePosition, rightChildNodeIndex) == true)
        {
            return true;
        }

    }

    return false;
}


