#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <unordered_map>
#include <SDL2/SDL.h>

namespace RGBA {
    enum ColorName : uint8_t {
        White,
        Red,
        Green,
        Blue,
    };

    inline const std::unordered_map<ColorName, SDL_Color> colorsMap = {
        { ColorName::White, {255, 255, 255, 255} },
        { ColorName::Red, {255, 0, 0, 255} },
        { ColorName::Green, {0, 255, 0, 255} },
        { ColorName::Blue, {0, 0, 255, 255} },
    };

    inline const SDL_Color& getColor(const ColorName colorName) {
        return RGBA::colorsMap.at(colorName);
    }

    inline SDL_Color lerpColors(const ColorName  colorNameA, const ColorName  colorNameB, const float t) {
        const auto& a = RGBA::colorsMap.at(colorNameA);
        const auto& b = RGBA::colorsMap.at(colorNameB);
        return SDL_Color {
            static_cast<uint8_t>(static_cast<float>(a.r) + (static_cast<float>(b.r) - static_cast<float>(a.r)) * t),
            static_cast<uint8_t>(static_cast<float>(a.g) + (static_cast<float>(b.g) - static_cast<float>(a.g)) * t),
            static_cast<uint8_t>(static_cast<float>(a.b) + (static_cast<float>(b.b) - static_cast<float>(a.b)) * t),
            static_cast<uint8_t>(static_cast<float>(a.a) + (static_cast<float>(b.a) - static_cast<float>(a.a)) * t)
        };
    }
};

#endif //SETTINGS_HPP
