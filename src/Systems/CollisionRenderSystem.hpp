#ifndef COLLISIONRENDERSYSTEM_HPP
#define COLLISIONRENDERSYSTEM_HPP

#include "../Components/TransformComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Events/CollisionEvent.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <SDL2/SDL.h>

class CollisionRenderSystem : public System {
public:
    CollisionRenderSystem(SDL_Renderer* renderer, const SDL_Rect& cameraFrame, EventBus* eventBus): renderer(renderer), cameraFrame(cameraFrame), eventBus(eventBus) {
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    };

    void SubscribeToEvents() {
        this->eventBus->SubscribeToEvent<CollisionRenderSystem, CollisionEvent>(this, &CollisionRenderSystem::OnCollision);
    };

    void OnCollision(CollisionEvent &event) {
        this->filled = true;
        this->color = {255, 0, 0, 100};
    };

    void Update() {
        for (auto entity : GetSystemEntities()) {
            auto &transform = entity.GetComponent<TransformComponent>();
            auto &collider = entity.GetComponent<BoxColliderComponent>();

            DrawRectangle(
                static_cast<int>(transform.position.x + collider.offset.x) - this->cameraFrame.x,
                static_cast<int>(transform.position.y + collider.offset.y) - this->cameraFrame.y,
                collider.width,
                collider.height
            );
        }
        this->filled = false;
        this->color = {0, 255, 0, 255};
    };

private:
    SDL_Renderer* renderer;
    const SDL_Rect& cameraFrame;
    EventBus* eventBus;
    std::array<int, 4> color = {0, 255, 0, 255};
    bool filled = false;

    void DrawRectangle(const int x, const int y, const int w, const int h) const {
        SDL_SetRenderDrawColor(this->renderer, this->color[0], this->color[1], this->color[2], this->color[3]);
        const SDL_Rect rect = {x, y, w,h };
        SDL_RenderDrawRect(this->renderer, &rect);
        if (this->filled) SDL_RenderFillRect(this->renderer, &rect);
    };
};
#endif
