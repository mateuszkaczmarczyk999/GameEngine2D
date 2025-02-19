#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include <glm/glm.hpp>

struct RigidBodyComponent
{
    glm::vec2 velocity;

    RigidBodyComponent(glm::vec2 velocity = glm::vec2(0.0, 0.0)) {
        this->velocity = velocity;
    };

};

#endif