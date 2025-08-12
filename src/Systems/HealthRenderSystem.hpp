#ifndef HEALTHRENDERSYSTEM_HPP
#define HEALTHRENDERSYSTEM_HPP

#include <SDL2/SDL.h>

#include "../ECS/ECS.hpp"
#include "../Settings/Settings.hpp"
#include "../AssetStore/AssetStore.hpp"
#include "../Components/HealthLabelComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/HealthComponent.hpp"

class HealthRenderSystem : public System {
public:
    HealthRenderSystem(SDL_Renderer* renderer, const SDL_Rect& cameraFrame, AssetStore* assetStore): renderer(renderer), cameraFrame(cameraFrame), assetStore(assetStore) {
        RequireComponent<HealthLabelComponent>();
        RequireComponent<TransformComponent>();
        RequireComponent<HealthComponent>();
    };

    void Update() {
        for (auto &entity: GetSystemEntities()) {
            const auto& health = entity.GetComponent<HealthComponent>();
            const auto& transform = entity.GetComponent<TransformComponent>();
            const auto& text = std::to_string(health.healthPercentage) + " %";
            const auto& color = RGBA::lerpColors(RGBA::Red, RGBA::Green, health.healthPercentage / 100.0);

            SDL_Surface* surface = TTF_RenderText_Blended(
                this->assetStore->GetFont("charriot"),
                text.c_str(),
                color
                );

            SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);

            int width, height;

            SDL_QueryTexture(texture, NULL, NULL, &width, &height);

            SDL_Rect destRect = {
                static_cast<int>(transform.position.x - this->cameraFrame.x),
                static_cast<int>(transform.position.y - this->cameraFrame.y),
                width,
                height,
            };

            SDL_RenderCopy(this->renderer, texture, NULL, &destRect);
        }
    };
private:
    SDL_Renderer* renderer;
    const SDL_Rect& cameraFrame;
    AssetStore* assetStore;
};

#endif //HEALTHRENDERSYSTEM_HPP
