#ifndef RENDER_HPP
#define RENDER_HPP

#include "../Logger/Logger.hpp"
#include "../ECS/ECS.hpp"
#include "../Components/Transform.hpp"
#include "../Components/Sprite.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class RenderSystem: public System
{
    public:
        RenderSystem()
        {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();
        };

        void Update(SDL_Renderer* renderer)
        {
            for (auto entity: GetSystemEntities())
            {
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();

                SDL_Rect spriteRect = {
                    static_cast<int>(transform.position.x),
                    static_cast<int>(transform.position.y),
                    sprite.width,
                    sprite.height
                };

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &spriteRect);
            }
        };
};

#endif