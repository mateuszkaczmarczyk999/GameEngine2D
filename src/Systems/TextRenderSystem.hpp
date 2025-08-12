#ifndef TEXTRENDERSYSTEM_HPP
#define TEXTRENDERSYSTEM_HPP

#include <SDL2/SDL.h>

#include "../ECS/ECS.hpp"
#include "../Settings/Settings.hpp"
#include "../AssetStore/AssetStore.hpp"
#include "../Components/TextLabelComponent.hpp"

class TextRenderSystem : public System {
public:
    TextRenderSystem(SDL_Renderer* renderer, const SDL_Rect& cameraFrame, AssetStore* assetStore): renderer(renderer), cameraFrame(cameraFrame), assetStore(assetStore) {
        RequireComponent<TextLabelComponent>();
    };

    void Update() {
        for (auto &entity: GetSystemEntities()) {
            const auto& textLabel = entity.GetComponent<TextLabelComponent>();

            SDL_Surface* surface = TTF_RenderText_Blended(
                this->assetStore->GetFont(textLabel.assetId),
                textLabel.text.c_str(),
                RGBA::getColor(textLabel.colorName)
                );

            SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);

            int width, height;

            SDL_QueryTexture(texture, NULL, NULL, &width, &height);

            SDL_Rect destRect = {
                static_cast<int>(textLabel.position.x) - (textLabel.uiFixed ? 0 : this->cameraFrame.x),
                static_cast<int>(textLabel.position.y) - (textLabel.uiFixed ? 0 : this->cameraFrame.y),
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


#endif //TEXTRENDERSYSTEM_HPP
