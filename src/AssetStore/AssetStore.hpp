#ifndef ASSETSTORE_HPP
#define ASSETSTORE_HPP

#include <map>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class AssetStore {
public:
    AssetStore();

    ~AssetStore();

    void AddTexture(SDL_Renderer *renderer, const std::string &assetId, const std::string &filePath);

    SDL_Texture *GetTexture(const std::string &assetId);

    void ClearAssets();

private:
    std::map<std::string, SDL_Texture *> textures;
};

#endif
