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
    GUIRenderSystem(SDL_Renderer* renderer, Registry* registry, SDL_Rect& cameraFrame): renderer(renderer), registry(registry), cameraFrame(cameraFrame) {}

    void Update() {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("Spawn enemies")) {
            const char* sprites[] = {"tank-tiger-right", "truck-ford-left"};
            static int selectedSpriteIndex = 0;
            static int positionX, positionY = 0;
            static int velocityX, velocityY = 10;
            static float projectileAngle = 0.0f;
            static float projectileVelocity = 10.0f;
            static int projectileRepeat = 10;
            static int projectileDuration = 10;
            static int health = 10;

            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Combo("texture id", &selectedSpriteIndex, sprites, IM_ARRAYSIZE(sprites));
            }
            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("position x", &positionX, 0, 1200);
                ImGui::SliderInt("position y", &positionY, 0, 1200);
            }
            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Velocity", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("velocity x", &velocityX, -300, 300);
                ImGui::SliderInt("velocity y", &velocityY, -300, 300);
            }
            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Projectiles", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderAngle("angle (deg)", &projectileAngle, 0, 360);
                ImGui::SliderFloat("speed (px/sec)", &projectileVelocity, 10, 500);
                ImGui::InputInt("repeat (sec)", &projectileRepeat);
                ImGui::InputInt("duration (sec)", &projectileDuration);
            }
            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("%", &health, 0, 100);
            }

            if (ImGui::Button("Create Enemy Entity")) {
                const auto radians = glm::radians(projectileAngle);
                const auto velX = glm::sin(radians) * projectileVelocity;
                const auto velY = glm::cos(radians) * projectileVelocity;

                Entity entity = registry->CreateEntity();
                entity.AddComponent<TransformComponent>(glm::vec2(positionX, positionY), glm::vec2(1.0, 1.0), 0.0);
                entity.AddComponent<RigidBodyComponent>(glm::vec2(velocityX, velocityY));
                entity.AddComponent<SpriteComponent>(sprites[selectedSpriteIndex], 32, 32, 1);
                entity.AddComponent<BoxColliderComponent>(32, 32);
                entity.AddComponent<ProjectileEmittingComponent>(glm::vec2(velX, velY), projectileRepeat * 1000, projectileDuration * 1000, 10);
                entity.AddComponent<HealthComponent>(health);
                entity.AddComponent<HealthLabelComponent>("charriot-s", "arial-xs", glm::vec2(5.0, -5.0));
                entity.AddGroup("Enemies");
            }

            positionX = positionY = 0;
            projectileAngle = 0.0f;
            projectileRepeat = projectileDuration = 10;
            projectileVelocity = 100;
            health = 100;
        }
        ImGui::End();

        // Display a small overlay window to display the map position using the mouse
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
        ImGui::SetNextWindowBgAlpha(0.9f);
        if (ImGui::Begin("Map coordinates", NULL, windowFlags)) {
            ImGui::Text(
                "Map coordinates (x=%.1f, y=%.1f)",
                ImGui::GetIO().MousePos.x + this->cameraFrame.x,
                ImGui::GetIO().MousePos.y + this->cameraFrame.y
            );
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), this->renderer);
    }

private:
    SDL_Renderer* renderer;
    Registry* registry;
    const SDL_Rect& cameraFrame;
};

#endif //GUIRENDERSYSTEM_HPP
