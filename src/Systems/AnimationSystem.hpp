#ifndef ANIMATIONSYSTEM_HPP
#define ANIMATIONSYSTEM_HPP

#include "../ECS/ECS.hpp"
#include "../Components/AnimationComponent.hpp"
#include "../Components/SpriteComponent.hpp"

class AnimationSystem: public System
{
    public:
        AnimationSystem()
        {
            RequireComponent<AnimationComponent>();
            RequireComponent<SpriteComponent>();
        };

        void Update()
        {
            for (auto entity: GetSystemEntities())
            {
                auto& animation = entity.GetComponent<AnimationComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();

                int elapsedTime = SDL_GetTicks() - animation.startTime;
                int elapsedFrame = (elapsedTime * animation.frameSpeed) / 1000;
                animation.currentFrame = elapsedFrame % animation.frameCount;
                sprite.srcRect.x = animation.currentFrame * sprite.width;
            }
        };
};

#endif