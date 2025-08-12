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
            const auto& label = entity.GetComponent<HealthLabelComponent>();
            const auto& health = entity.GetComponent<HealthComponent>();
            const auto& transform = entity.GetComponent<TransformComponent>();
            const auto& text = std::to_string(health.healthPercentage) + " %";
            const auto& color = RGBA::lerpColors(RGBA::Red, RGBA::Green, health.healthPercentage / 100.0);
            RenderText(label.textAssetId, text.c_str(), color, transform.position + label.offset);

            std::string healthBar;
            for (int i = 0; i < 100; i+=10) {
                if (i < health.healthPercentage) healthBar += u8"█";
                else healthBar += u8"░";
            }

            RenderText(label.barAssetId, healthBar.c_str(), color, transform.position + label.offset - glm::vec2(0, 15.0));
        }
    };
private:
    SDL_Renderer* renderer;
    const SDL_Rect& cameraFrame;
    AssetStore* assetStore;

    void RenderText(const std::string& fontAsset, const char* utf8, const SDL_Color& color, const glm::vec2& position) {
        SDL_Surface* surface = TTF_RenderUTF8_Blended(this->assetStore->GetFont(fontAsset), utf8, color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        int width, height;
        SDL_QueryTexture(texture, NULL, NULL, &width, &height);
        SDL_Rect destRect = {
            static_cast<int>(position.x - this->cameraFrame.x),
            static_cast<int>(position.y - this->cameraFrame.y),
            width,
            height,
        };
        SDL_RenderCopy(this->renderer, texture, NULL, &destRect);
        SDL_DestroyTexture(texture);
    }
};

#endif //HEALTHRENDERSYSTEM_HPP
