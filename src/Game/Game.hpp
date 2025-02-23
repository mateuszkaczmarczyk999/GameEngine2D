#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include "../ECS/ECS.hpp"
#include "../AssetStore/AssetStore.hpp"

const int FPS = 60; 
const int MILLISECS_PER_FRAME = 1000 / FPS;
const bool CAPPED_FPS = true;

class Game
{
    public:
        Game();
        ~Game();

        void Initialize();
        void Run();
        void Setup();
        void ProcessInput();
        void Update();
        void Render();
        void Destroy();

        int windowWidth;
        int windowHeight;

    private:
        int pravFrameTimestamp = 0;
        bool isRunning;
        SDL_Window* window;
        SDL_Renderer* renderer;

        std::unique_ptr<Registry> registry;
        std::unique_ptr<AssetStore> assetStore;
};

#endif