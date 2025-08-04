#ifndef KEYBOARDMOVEMENTCOMPONENT_HPP
#define KEYBOARDMOVEMENTCOMPONENT_HPP

#include <glm/glm.hpp>

struct KeyboardMovementComponent {
    glm::vec2 upVelocity;
    glm::vec2 rightVelocity;
    glm::vec2 downVelocity;
    glm::vec2 leftVelocity;

    KeyboardMovementComponent(glm::vec2 up = glm::vec2(0.0, -1.0), glm::vec2 right = glm::vec2(1.0, 0.0), glm::vec2 down = glm::vec2(0.0, 1.0), glm::vec2 left = glm::vec2(-1.0, 0.0)) {
        this->upVelocity = up;
        this->rightVelocity = right;
        this->downVelocity = down;
        this->leftVelocity = left;
    };
};

#endif //KEYBOARDMOVEMENTCOMPONENT_HPP
