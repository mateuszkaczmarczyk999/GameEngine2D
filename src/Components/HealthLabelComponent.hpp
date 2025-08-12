#ifndef HEALTHLABELCOMPONENT_HPP
#define HEALTHLABELCOMPONENT_HPP

#include <glm/glm.hpp>

struct HealthLabelComponent {
    glm::vec2 offset;

    HealthLabelComponent(const glm::vec2& offset = glm::vec2(0)) {
        this->offset = offset;
    }
};

#endif //HEALTHLABELCOMPONENT_HPP
