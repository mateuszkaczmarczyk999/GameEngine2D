#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../AssetStore/AssetStore.hpp"
#include "../ECS/ECS.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/SpriteComponent.hpp"

struct Renderable {
    SpriteComponent sprite;
    TransformComponent transform;

    Renderable(SpriteComponent sprite, TransformComponent transform) {
        this->sprite = sprite;
        this->transform = transform;
    }
};

class RenderSystem : public System {
public:
    RenderSystem(SDL_Renderer* renderer, const SDL_Rect& cameraFrame, AssetStore* assetStore): renderer(renderer), cameraFrame(cameraFrame), assetStore(assetStore) {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    };

    void Update() {
        std::vector<Renderable> renderables;

        for (auto entity: GetSystemEntities()) {
            renderables.emplace_back(entity.GetComponent<SpriteComponent>(), entity.GetComponent<TransformComponent>());
        }

        auto sortByZIndex = [](Renderable &a, Renderable &b) {
            return a.sprite.zIndex < b.sprite.zIndex;
        };

        std::sort(renderables.begin(), renderables.end(), sortByZIndex);

        for (auto &renderable: renderables) {
            SDL_Rect srcRect = renderable.sprite.srcRect;
            const auto& sprite = renderable.sprite;
            const auto& transform = renderable.transform;

            SDL_Rect destRect = {
                static_cast<int>(transform.position.x) - (sprite.uiFixed ? 0 : this->cameraFrame.x),
                static_cast<int>(transform.position.y) - (sprite.uiFixed ? 0 : this->cameraFrame.y),
                static_cast<int>(sprite.width * transform.scale.x),
                static_cast<int>(sprite.height * transform.scale.y),
            };

            SDL_RenderCopyEx(
                this->renderer,
                this->assetStore->GetTexture(renderable.sprite.assetId),
                &srcRect,
                &destRect,
                renderable.transform.rotation,
                NULL,
                SDL_FLIP_NONE
            );
        }
    };
private:
    SDL_Renderer* renderer;
    const SDL_Rect& cameraFrame;
    AssetStore* assetStore;
};

#endif
