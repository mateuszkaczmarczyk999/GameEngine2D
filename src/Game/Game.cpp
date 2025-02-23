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
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<MovementSystem>();

    Entity tank = registry->CreateEntity();

    tank.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 10.0));
    tank.AddComponent<SpriteComponent>(10, 10);

    Entity tank2 = registry->CreateEntity();

    tank2.AddComponent<TransformComponent>(glm::vec2(50.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    tank2.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 50.0));
    tank2.AddComponent<SpriteComponent>(10, 50);
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

    // SDL_Surface* surface = IMG_Load("./assets/images/tank-tiger-right.png");
    // SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    // SDL_FreeSurface(surface);

    // SDL_Rect dstRect = { 
    //     static_cast<int>(playerPosition.x),
    //     static_cast<int>(playerPosition.y),
    //     32,
    //     32
    // };
    // SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    // SDL_DestroyTexture(texture);

    registry->GetSystem<RenderSystem>().Update(renderer);

    SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}