#ifndef GUIRENDERSYSTEM_HPP
#define GUIRENDERSYSTEM_HPP

#include "../ECS/ECS.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/ProjectileEmittingComponent.hpp"
#include "../Components/HealthComponent.hpp"
#include "../Components/HealthLabelComponent.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
#include <SDL2/SDL.h>

class GUIRenderSystem: public System {

public:
    GUIRenderSystem(SDL_Renderer* renderer, Registry* registry) {
        this->renderer = renderer;
        this->registry = registry;
    };

    void Update() {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        static int positionX, positionY = 0;
        ImGui::SliderInt("Enemy position X", &positionX, 0, 1000);
        ImGui::SliderInt("Enemy position Y", &positionY, 0, 1000);
        static int velocityX, velocityY = 10;
        ImGui::SliderInt("Enemy velocity X", &velocityX, -100, 100);
        ImGui::SliderInt("Enemy velocity Y", &velocityY, -100, 100);

        static int picker_mode = 1;
        std::unordered_map<int, std::string> assetMap;
        assetMap.emplace(0, "tank-tiger-right");
        assetMap.emplace(1, "truck-ford-left");
        ImGui::Combo("Picker Mode", &picker_mode, "tank-tiger-right\0truck-ford-left\0");

        static float projectileAngle= 0.0f;
        static int projectileVelocity = 0;
        ImGui::SliderFloat("Enemy projectile angle", &projectileAngle, 0.0f, 60.0f, "%.3f");
        ImGui::SliderInt("Enemy projectile velocity", &projectileVelocity, 0, 200);
        const auto radians = glm::radians(projectileAngle);
        const auto velX = glm::sin(radians) * projectileVelocity;
        const auto velY = glm::cos(radians) * projectileVelocity;

        static int healthValue = 10;
        ImGui::SliderInt("Enemy start health", &healthValue, 0, 100);

        // ImGui::ShowDemoWindow();
        if (ImGui::Button("Create Enemy Entity")) {
            Entity entity = registry->CreateEntity();
            entity.AddComponent<TransformComponent>(glm::vec2(positionX, positionY), glm::vec2(1.0, 1.0), 0.0);
            entity.AddComponent<RigidBodyComponent>(glm::vec2(velocityX, velocityY));
            entity.AddComponent<SpriteComponent>(assetMap.at(picker_mode), 32, 32, 1);
            entity.AddComponent<BoxColliderComponent>(32, 32);
            entity.AddComponent<ProjectileEmittingComponent>(glm::vec2(velX, velY), 1000, 3000, 10);
            entity.AddComponent<HealthComponent>(healthValue);
            entity.AddComponent<HealthLabelComponent>("charriot-s", "arial-xs", glm::vec2(5.0, -5.0));
            entity.AddGroup("Enemies");
        }
        ImGui::Render();

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), this->renderer);
    }

private:
    SDL_Renderer* renderer;
    Registry* registry;
};

#endif //GUIRENDERSYSTEM_HPP
