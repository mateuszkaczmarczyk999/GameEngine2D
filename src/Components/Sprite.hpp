#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <SDL2/SDL_image.h>
#include <string>

struct SpriteComponent
{
    int width;
    int height;
    std::string assetId;
    SDL_Rect srcRect;

    SpriteComponent(const std::string assetId = "", int width = 0, int height = 0, int originX = 0, int originY = 0) {
        this->width = width;
        this->height = height;
        this->assetId = assetId;
        this->srcRect = { originX, originY, width, height };
    };
};

#endif