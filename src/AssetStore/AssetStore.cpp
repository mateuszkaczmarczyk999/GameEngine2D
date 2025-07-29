#include "AssetStore.hpp"
#include "../Logger/Logger.hpp"

AssetStore::AssetStore() {
    Logger::Log("AssetStore class created.");
};

AssetStore::~AssetStore() {
    ClearAssets();
    Logger::Log("AssetStore class destroyed.");
};

void AssetStore::AddTexture(SDL_Renderer *renderer, const std::string &assetId, const std::string &filePath) {
    SDL_Surface *surface = IMG_Load(filePath.c_str());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    textures.emplace(assetId, texture);

    Logger::Log("New texture added with id: " + assetId);
};

SDL_Texture *AssetStore::GetTexture(const std::string &assetId) {
    return textures[assetId];
};

void AssetStore::ClearAssets() {
    for (auto textureRecord: textures) {
        SDL_DestroyTexture(textureRecord.second);
    }
    textures.clear();
};
