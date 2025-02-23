#include "Game.hpp"
#include "../Logger/Logger.hpp"
#include "../Components/Transform.hpp"
#include "../Components/RigidBody.hpp"
#include "../Components/Sprite.hpp"
#include "../Systems/Movement.hpp"
#include "../Systems/Render.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

Game::Game()
{
    isRunning = false;
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    Logger::Log("Game class created.");
}

Game::~Game()
{
    Logger::Log("Game class destroyed.");
}

void Game::Initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Logger::Err("Error with initializing SDL.");
        return;
    }
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    // windowWidth = 800; //displayMode.w;
    // windowHeight = 600; //displayMode.h;
    windowWidth = displayMode.w - 50;
    windowHeight = displayMode.h - 50;

    window = SDL_CreateWindow(
        "Game Engne 2D",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS
    );
    if (!window)
    {
        Logger::Err("Error with creating SDL window.");
        return;
    }
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer)
    {
        Logger::Err("Error with creating SDL renderer.");
        return;
    }
    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    isRunning = true;
}

void Game::Run()
{
    Setup();
    while(isRunning)
    {
        ProcessInput();
        Update();
        Render();
    }
}

glm::vec2 playerPosition;
glm::vec2 playerVelocity;

void Game::Setup()
{
    assetStore->AddTexture(renderer, "tank-tiger-right", "./assets/images/tank-tiger-right.png");
    assetStore->AddTexture(renderer, "truck-ford-left", "./assets/images/truck-ford-left.png");

    registry->AddSystem<RenderSystem>();
    registry->AddSystem<MovementSystem>();

    Entity tank = registry->CreateEntity();

    tank.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 10.0));
    tank.AddComponent<SpriteComponent>("tank-tiger-right", 32, 32);

    Entity tank2 = registry->CreateEntity();

    tank2.AddComponent<TransformComponent>(glm::vec2(50.0, 50.0), glm::vec2(1.0, 1.0), 45.0);
    tank2.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 50.0));
    tank2.AddComponent<SpriteComponent>("truck-ford-left", 32, 32);
}

void Game::ProcessInput()
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) isRunning = false;
                break;
            
            default:
                break;
        }
    }
}

void Game::Update()
{
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - pravFrameTimestamp);
    if (CAPPED_FPS && timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) SDL_Delay(timeToWait);

    double deltaTime = (SDL_GetTicks() - pravFrameTimestamp) / 1000.0;

    pravFrameTimestamp = SDL_GetTicks();

    registry->Update();
    registry->GetSystem<MovementSystem>().Update(deltaTime);
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 36, 10, 10, 255);
    SDL_RenderClear(renderer);

    registry->GetSystem<RenderSystem>().Update(renderer, assetStore);

    SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}