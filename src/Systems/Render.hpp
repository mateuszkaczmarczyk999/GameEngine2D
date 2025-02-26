#ifndef RENDER_HPP
#define RENDER_HPP

#include "../Logger/Logger.hpp"
#include "../AssetStore/AssetStore.hpp"
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

        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore)
        {
            auto sortByZIndex = [](Entity a, Entity b) {
                const auto spriteA = a.GetComponent<SpriteComponent>();
                const auto spriteB = b.GetComponent<SpriteComponent>();
                return spriteA.zIndex < spriteB.zIndex;
            };

            auto entities = GetSystemEntities();
            std::sort(entities.begin(), entities.end(), sortByZIndex);

            for (auto entity: entities)
            {
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();

                SDL_Rect srcRect = sprite.srcRect;

                SDL_Rect destRect = {
                    static_cast<int>(transform.position.x),
                    static_cast<int>(transform.position.y),
                    static_cast<int>(sprite.width * transform.scale.x),
                    static_cast<int>(sprite.height * transform.scale.y),
                };

                SDL_RenderCopyEx(
                    renderer,
                    assetStore->GetTexture(sprite.assetId),
                    &srcRect,
                    &destRect,
                    transform.rotation,
                    NULL,
                    SDL_FLIP_NONE
                );
            }
        };
};

#endif