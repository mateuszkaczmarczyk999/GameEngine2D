#ifndef HEALTHLABELCOMPONENT_HPP
#define HEALTHLABELCOMPONENT_HPP

#include <glm/glm.hpp>

struct HealthLabelComponent {
    std::string textAssetId;
    std::string barAssetId;
    glm::vec2 offset;

    HealthLabelComponent(const std::string& textAssetId = "", const std::string& barAssetId = "", const glm::vec2& offset = glm::vec2(0)) {
        this->textAssetId = textAssetId;
        this->barAssetId = barAssetId;
        this->offset = offset;
    }
};

#endif //HEALTHLABELCOMPONENT_HPP
