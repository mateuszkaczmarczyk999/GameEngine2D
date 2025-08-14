#ifndef FLIPSPRITECOMPONENT_HPP
#define FLIPSPRITECOMPONENT_HPP
#include <string>
#include <array>

struct FlipSpriteComponent {
    std::string prefixAssetId;
    std::array<std::string, 4> flipSuffix;

    FlipSpriteComponent(const std::string prefixAssetId = "", const std::array<std::string, 4> flipSuffix = {"-up", "-right", "-down", "-left"}) {
        this->prefixAssetId = prefixAssetId;
        this->flipSuffix = flipSuffix;
    }
};

#endif //FLIPSPRITECOMPONENT_HPP
