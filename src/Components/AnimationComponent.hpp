#ifndef ANIMATIONCOMPONENT_HPP
#define ANIMATIONCOMPONENT_HPP

#include <SDL2/SDL.h>
#include <string>

struct AnimationComponent {
    int frameCount;
    int frameSpeed;
    bool isLooped;
    int currentFrame;
    int startTime;

    AnimationComponent(int frameCount = 1, int frameSpeed = 1, bool isLooped = true) {
        this->frameCount = frameCount;
        this->frameSpeed = frameSpeed;
        this->isLooped = isLooped;
        this->currentFrame = 1;
        this->startTime = SDL_GetTicks();
    };
};

#endif
