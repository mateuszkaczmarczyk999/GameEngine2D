#ifndef TEXTLABELCOMPONENT_HPP
#define TEXTLABELCOMPONENT_HPP

#include <glm/glm.hpp>
#include <string>
#include "../Settings/Settings.hpp"

struct TextLabelComponent {
    std::string text;
    glm::vec2 position;
    std::string assetId;
    RGBA::ColorName colorName;
    bool uiFixed;

    TextLabelComponent(const std::string& text = "", const glm::vec2& position = glm::vec2(0), const std::string& assetId = "", RGBA::ColorName colorName = RGBA::ColorName::White, const bool uiFixed = false) {
        this->text = text;
        this->position = position;
        this->assetId = assetId;
        this->colorName = colorName;
        this->uiFixed = uiFixed;
    }
};

#endif //TEXTLABELCOMPONENT_HPP
