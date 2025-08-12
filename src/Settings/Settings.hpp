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
};

#endif //SETTINGS_HPP
