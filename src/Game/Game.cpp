#include "Game.hpp"
#include "../Logger/Logger.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/AnimationComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"

#include "../Systems/MovementSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/AnimationSystem.hpp"
#include "../Systems/CollisionSystem.hpp"
#include "../Systems/CollisionRenderSystem.hpp"
#include "../Systems/DamageSystem.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

#include <fstream>
#include <string>

#include "../Systems/DamageSystem.hpp"

Game::Game() {
    isRunning = false;
    debugMode = false;
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    eventBus = std::make_unique<EventBus>();
    Logger::Log("Game class created.");
}

Game::~Game() {
    Logger::Log("Game class destroyed.");
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Err("Error with initializing SDL.");
        return;
    }
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = 800; //displayMode.w;
    windowHeight = 800; //displayMode.h;
    // windowWidth = displayMode.w - 50;
    // windowHeight = displayMode.h - 50;

    window = SDL_CreateWindow(
        "Game Engne 2D",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        Logger::Err("Error with creating SDL window.");
        return;
    }
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        Logger::Err("Error with creating SDL renderer.");
        return;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    isRunning = true;
}

void Game::Run() {
    Setup();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::LoadAssets() {
    assetStore->AddTexture(renderer, "jungle", "./assets/tilemaps/jungle.png");
    assetStore->AddTexture(renderer, "tank-tiger-right", "./assets/images/tank-tiger-right.png");
    assetStore->AddTexture(renderer, "truck-ford-left", "./assets/images/truck-ford-left.png");
    assetStore->AddTexture(renderer, "chopper", "./assets/images/chopper.png");
    assetStore->AddTexture(renderer, "radar", "./assets/images/radar.png");
}

void Game::LoadMap() {
    std::ifstream file("./assets/tilemaps/jungle.map");
    if (!file) {
        Logger::Err("Can not open the file!");
    }
    std::string line;

    float tileSize = 32.0;
    float tileScale = 2.0;
    int col = 0;
    while (std::getline(file, line)) {
        size_t start = 0;
        size_t end = 0;
        int row = 0;

        while ((end = line.find(",", start)) != std::string::npos) {
            std::string tileRef = line.substr(start, end - start);
            auto tilePos = std::stoi(tileRef);
            auto tileYOffset = (tilePos / 10) * tileSize;
            auto tileXOffset = (tilePos - (10 * (tilePos / 10))) * tileSize;
            auto positionX = tileSize * tileScale * row;
            auto positionY = tileSize * tileScale * col;

            Entity tile = registry->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(positionX, positionY), glm::vec2(tileScale, tileScale), 0.0);
            tile.AddComponent<SpriteComponent>("jungle", 32, 32, 0, tileXOffset, tileYOffset);

            start = end + 1;
            row++;
        }
        col++;
    }

    file.close();
}

void Game::LoadLevel() {
    Entity tank = registry->CreateEntity();

    tank.AddComponent<TransformComponent>(glm::vec2(50.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 0.0));
    tank.AddComponent<SpriteComponent>("tank-tiger-right", 32, 32, 2);
    tank.AddComponent<BoxColliderComponent>(32, 32);

    Entity truck = registry->CreateEntity();

    truck.AddComponent<TransformComponent>(glm::vec2(500.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(-20.0, 0.0));
    truck.AddComponent<SpriteComponent>("truck-ford-left", 32, 32, 1);
    truck.AddComponent<BoxColliderComponent>(32, 32);

    Entity chopper = registry->CreateEntity();

    chopper.AddComponent<TransformComponent>(glm::vec2(150.0, 150.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(30.0, 10.0));
    chopper.AddComponent<SpriteComponent>("chopper", 32, 32, 1);
    chopper.AddComponent<AnimationComponent>(2, 15, true);

    Entity radar = registry->CreateEntity();

    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    radar.AddComponent<SpriteComponent>("radar", 64, 64, 1);
    radar.AddComponent<AnimationComponent>(8, 5, true);
}

void Game::Setup() {
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>(eventBus.get());
    registry->AddSystem<CollisionRenderSystem>(eventBus.get(), renderer);
    registry->AddSystem<DamageSystem>(eventBus.get());

    LoadAssets();
    LoadMap();
    LoadLevel();

    registry->GetSystem<CollisionRenderSystem>().SubscribeToEvents();
    registry->GetSystem<DamageSystem>().SubscribeToEvents();
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) isRunning = false;
                if (sdlEvent.key.keysym.sym == SDLK_d) debugMode = !debugMode;
                break;

            default:
                break;
        }
    }
}

void Game::Update() {
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - pravFrameTimestamp);
    if (CAPPED_FPS && timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) SDL_Delay(timeToWait);

    double deltaTime = (SDL_GetTicks() - pravFrameTimestamp) / 1000.0;

    pravFrameTimestamp = SDL_GetTicks();

    registry->Update();
    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<CollisionSystem>().Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 36, 10, 10, 255);
    SDL_RenderClear(renderer);

    registry->GetSystem<RenderSystem>().Update(renderer, assetStore);
    if (debugMode) registry->GetSystem<CollisionRenderSystem>().Update();

    SDL_RenderPresent(renderer);
}

void Game::Destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
