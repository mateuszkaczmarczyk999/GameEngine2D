#ifndef RENDER_HPP
#define RENDER_HPP

#include "../Logger/Logger.hpp"
#include "../AssetStore/AssetStore.hpp"
#include "../ECS/ECS.hpp"
#include "../Components/Transform.hpp"
#include "../Components/Sprite.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct Renderable
{
    SpriteComponent sprite;
    TransformComponent transform;

    Renderable(SpriteComponent sprite, TransformComponent transform)
    {
        this->sprite = sprite;
        this->transform = transform;
    }
};

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
            std::vector<Renderable> renderables;

            for (auto entity: GetSystemEntities())
            {
                renderables.emplace_back(entity.GetComponent<SpriteComponent>(), entity.GetComponent<TransformComponent>());
            }

            auto sortByZIndex = [](Renderable& a, Renderable& b) {
                return a.sprite.zIndex < b.sprite.zIndex;
            };

            std::sort(renderables.begin(), renderables.end(), sortByZIndex);

            for (auto& renderable: renderables)
            {
                SDL_Rect srcRect = renderable.sprite.srcRect;

                SDL_Rect destRect = {
                    static_cast<int>(renderable.transform.position.x),
                    static_cast<int>(renderable.transform.position.y),
                    static_cast<int>(renderable.sprite.width * renderable.transform.scale.x),
                    static_cast<int>(renderable.sprite.height * renderable.transform.scale.y),
                };

                SDL_RenderCopyEx(
                    renderer,
                    assetStore->GetTexture(renderable.sprite.assetId),
                    &srcRect,
                    &destRect,
                    renderable.transform.rotation,
                    NULL,
                    SDL_FLIP_NONE
                );
            }
        };
};

#endif