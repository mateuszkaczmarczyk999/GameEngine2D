#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include "../ECS/ECS.hpp"
#include "../AssetStore/AssetStore.hpp"
#include "../Events/EventBus.hpp"

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;
const bool CAPPED_FPS = true;

class Game {
public:
    Game();

    ~Game();

    void Initialize();
    void Run();
    void Setup();
    void LoadAssets();
    void LoadMap();
    void LoadLevel();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();

    static int windowWidth;
    static int windowHeight;
    static int mapWidth;
    static int mapHeight;

private:
    int pravFrameTimestamp = 0;
    bool isRunning;
    bool debugMode;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect cameraFrame;

    std::unique_ptr<Registry> registry;
    std::unique_ptr<AssetStore> assetStore;
    std::unique_ptr<EventBus> eventBus;
};

#endif
