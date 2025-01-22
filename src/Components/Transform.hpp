#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "../ECS/ECS.hpp"
#include <glm/glm.hpp>

class Transform: public Component
{
    public:
        glm::vec2 position;
        glm::vec2 scale;
        double rotation;
};

#endif