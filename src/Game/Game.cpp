#include "Game.hpp"
#include "../Logger/Logger.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/AnimationComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/KeyboardMovementComponent.hpp"
#include "../Components/CameraFollowComponent.hpp"
#include "../Components/ProjectileEmittingComponent.hpp"
#include "../Components/HealthComponent.hpp"
#include "../Components/TextLabelComponent.hpp"
#include "../Components/HealthLabelComponent.hpp"
#include "../Components/FlipSpriteComponent.hpp"

#include "../Systems/MovementSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/AnimationSystem.hpp"
#include "../Systems/CollisionSystem.hpp"
#include "../Systems/CollisionRenderSystem.hpp"
#include "../Systems/DamageSystem.hpp"
#include "../Systems/KeyboardMovementSystem.hpp"
#include "../Systems/CameraFollowSystem.hpp"
#include "../Systems/ProjectileEmittingSystem.hpp"
#include "../Systems/ProjectileLifecycleSystem.hpp"
#include "../Systems/TextRenderSystem.hpp"
#include "../Systems/HealthRenderSystem.hpp"
#include "../Systems/FlipSpriteSystem.hpp"
#include "../Systems/GUIRenderSystem.hpp"

#include "../Settings/Settings.hpp"

#include "../Events/KeyPressedEvent.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
#include <glm/glm.hpp>

#include <fstream>
#include <string>

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

    if (TTF_Init() !=0) {
        Logger::Err("Error with initializing TTF.");
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = 800; //displayMode.w;
    windowHeight = 800; //displayMode.h;
    // windowWidth = displayMode.w - 50;
    // windowHeight = displayMode.h - 50;

    window = SDL_CreateWindow(
        "Game Engine 2D",
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    cameraFrame.x = 0;
    cameraFrame.y = 0;
    cameraFrame.w = windowWidth;
    cameraFrame.h = windowHeight;

    isRunning = true;
}

void Game::Run() {
    Setup();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
        // this->eventBus->Reset();
    }
}

void Game::LoadAssets() {
    assetStore->AddTexture(renderer, "jungle", "./assets/tilemaps/jungle.png");
    assetStore->AddTexture(renderer, "landing-base", "./assets/images/landing-base.png");
    assetStore->AddTexture(renderer, "tree", "./assets/images/tree.png");
    //Tiger tanks
    assetStore->AddTexture(renderer, "tank-tiger-right", "./assets/images/tank-tiger-right.png");
    assetStore->AddTexture(renderer, "tank-tiger-down", "./assets/images/tank-tiger-down.png");
    assetStore->AddTexture(renderer, "tank-tiger-left", "./assets/images/tank-tiger-left.png");
    assetStore->AddTexture(renderer, "tank-tiger-up", "./assets/images/tank-tiger-up.png");
    //Panther tanks
    assetStore->AddTexture(renderer, "tank-panther-right", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "tank-panther-down", "./assets/images/tank-panther-down.png");
    assetStore->AddTexture(renderer, "tank-panther-left", "./assets/images/tank-panther-left.png");
    assetStore->AddTexture(renderer, "tank-panther-up", "./assets/images/tank-panther-up.png");
    //Trucks
    assetStore->AddTexture(renderer, "truck-ford-right", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "truck-ford-down", "./assets/images/truck-ford-down.png");
    assetStore->AddTexture(renderer, "truck-ford-left", "./assets/images/truck-ford-left.png");
    assetStore->AddTexture(renderer, "truck-ford-up", "./assets/images/truck-ford-up.png");

    assetStore->AddTexture(renderer, "chopper", "./assets/images/chopper-spritesheet.png");
    assetStore->AddTexture(renderer, "radar", "./assets/images/radar.png");
    assetStore->AddTexture(renderer, "bullet", "./assets/images/bullet.png");
    assetStore->AddFont("charriot-title", "./assets/fonts/charriot.ttf", 24);
    assetStore->AddFont("charriot-s", "./assets/fonts/charriot.ttf", 16);
    assetStore->AddFont("arial-xs", "./assets/fonts/arial.ttf", 6);
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
            tile.AddComponent<SpriteComponent>("jungle", 32, 32, 0, false, tileXOffset, tileYOffset);
            tile.AddGroup("Tiles");

            start = end + 1;
            row++;
        }
        col++;
    }

    file.close();
}

void Game::LoadLevel() {
    Entity chopper = registry->CreateEntity();

    chopper.AddComponent<TransformComponent>(glm::vec2(150.0, 150.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.AddComponent<RigidBodyComponent>();
    chopper.AddComponent<SpriteComponent>("chopper", 32, 32, 2);
    chopper.AddComponent<BoxColliderComponent>(32, 32);
    chopper.AddComponent<AnimationComponent>(2, 15, true);
    chopper.AddComponent<KeyboardMovementComponent>(glm::vec2(0.0, -80.0), glm::vec2(80.0, 0.0), glm::vec2(0.0, 80.0), glm::vec2(-80.0, 0.0));
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<ProjectileEmittingComponent>(glm::vec2(500.0, 0.0), 100, 3000, 10, true, false);
    chopper.AddComponent<HealthComponent>(100);
    chopper.AddComponent<HealthLabelComponent>("charriot-s", "arial-xs", glm::vec2(5.0, -5.0));
    chopper.AddTag("Player");

    Entity tankA = registry->CreateEntity();

    tankA.AddComponent<TransformComponent>(glm::vec2(50.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    tankA.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 0.0));
    tankA.AddComponent<SpriteComponent>("tank-tiger-right", 32, 32, 1);
    tankA.AddComponent<BoxColliderComponent>(32, 32);
    tankA.AddComponent<ProjectileEmittingComponent>(glm::vec2(500.0, 0.0), 1000, 3000, 10);
    tankA.AddComponent<HealthComponent>(100);
    tankA.AddComponent<HealthLabelComponent>("charriot-s", "arial-xs", glm::vec2(5.0, -5.0));
    tankA.AddComponent<FlipSpriteComponent>("tank-tiger");
    tankA.AddGroup("Enemies");

    Entity tankB = registry->CreateEntity();

    tankB.AddComponent<TransformComponent>(glm::vec2(250.0, 150.0), glm::vec2(1.0, 1.0), 0.0);
    tankB.AddComponent<RigidBodyComponent>(glm::vec2(10.0, 0.0));
    tankB.AddComponent<SpriteComponent>("tank-panther-right", 32, 32, 1);
    tankB.AddComponent<BoxColliderComponent>(32, 32);
    tankB.AddComponent<ProjectileEmittingComponent>(glm::vec2(500.0, 0.0), 1000, 3000, 10);
    tankB.AddComponent<HealthComponent>(100);
    tankB.AddComponent<HealthLabelComponent>("charriot-s", "arial-xs", glm::vec2(5.0, -5.0));
    tankB.AddComponent<FlipSpriteComponent>("tank-panther");
    tankB.AddGroup("Enemies");

    Entity truck = registry->CreateEntity();

    truck.AddComponent<TransformComponent>(glm::vec2(500.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(-20.0, 0.0));
    truck.AddComponent<SpriteComponent>("truck-ford-left", 32, 32, 4);
    truck.AddComponent<BoxColliderComponent>(32, 32);
    truck.AddComponent<ProjectileEmittingComponent>(glm::vec2(250.0, -250), 1000, 3000, 10);
    truck.AddComponent<HealthComponent>(100);
    truck.AddComponent<HealthLabelComponent>("charriot-s", "arial-xs", glm::vec2(5.0, -5.0));
    truck.AddComponent<FlipSpriteComponent>("truck-ford");
    truck.AddGroup("Enemies");

    Entity radar = registry->CreateEntity();

    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    radar.AddComponent<SpriteComponent>("radar", 64, 64, 3, true);
    radar.AddComponent<AnimationComponent>(8, 5, true);
    radar.AddGroup("HUD");

    Entity label = registry->CreateEntity();
    label.AddComponent<TextLabelComponent>("CHOPPER 1.0", glm::vec2(windowWidth/2 - 80, 20), "charriot-title", RGBA::ColorName::Green, true);
}

void Game::Setup() {
    registry->AddSystem<RenderSystem>(renderer, cameraFrame, assetStore.get());
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>(eventBus.get());
    registry->AddSystem<CollisionRenderSystem>(renderer, cameraFrame, eventBus.get());
    registry->AddSystem<DamageSystem>(eventBus.get());
    registry->AddSystem<KeyboardMovementSystem>(eventBus.get());
    registry->AddSystem<CameraFollowSystem>(cameraFrame);
    registry->AddSystem<ProjectileEmittingSystem>(registry.get(), eventBus.get());
    registry->AddSystem<ProjectileLifecycleSystem>();
    registry->AddSystem<TextRenderSystem>(renderer, cameraFrame, assetStore.get());
    registry->AddSystem<HealthRenderSystem>(renderer, cameraFrame, assetStore.get());
    registry->AddSystem<FlipSpriteSystem>();
    registry->AddSystem<GUIRenderSystem>(renderer, registry.get());

    LoadAssets();
    LoadMap();
    LoadLevel();

    registry->GetSystem<CollisionRenderSystem>().SubscribeToEvents();
    registry->GetSystem<DamageSystem>().SubscribeToEvents();
    registry->GetSystem<KeyboardMovementSystem>().SubscribeToEvents();
    registry->GetSystem<ProjectileEmittingSystem>().SubscribeToEvents();
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        ImGuiIO& io = ImGui::GetIO();
        int mouseX, mouseY;
        const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
        io.MousePos = ImVec2(mouseX, mouseY);
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

        switch (sdlEvent.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) isRunning = false;
                if (sdlEvent.key.keysym.sym == SDLK_d) debugMode = !debugMode;
                this->eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
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
    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<CollisionSystem>().Update();
    registry->GetSystem<CameraFollowSystem>().Update();
    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->GetSystem<ProjectileEmittingSystem>().Update();
    registry->GetSystem<ProjectileLifecycleSystem>().Update();
    registry->GetSystem<FlipSpriteSystem>().Update();
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 36, 10, 10, 255);
    SDL_RenderClear(renderer);

    registry->GetSystem<RenderSystem>().Update();
    registry->GetSystem<TextRenderSystem>().Update();
    registry->GetSystem<HealthRenderSystem>().Update();
    if (debugMode) {
        registry->GetSystem<CollisionRenderSystem>().Update();
        registry->GetSystem<GUIRenderSystem>().Update();
    }

    SDL_RenderPresent(renderer);
}

void Game::Destroy() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
