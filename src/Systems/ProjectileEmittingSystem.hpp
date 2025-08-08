#ifndef PROJECTILEEMITTINGSYSTEM_HPP
#define PROJECTILEEMITTINGSYSTEM_HPP

#include "../Components/ProjectileEmittingComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/ProjectileComponent.hpp"
#include "../Events/KeyPressedEvent.hpp"
#include "../ECS/ECS.hpp"

class ProjectileEmittingSystem : public System {
public:
    ProjectileEmittingSystem(Registry* registry, EventBus* eventBus) : registry(registry), eventBus(eventBus) {
        RequireComponent<TransformComponent>();
        RequireComponent<ProjectileEmittingComponent>();
        RequireComponent<RigidBodyComponent>();
    }

    void SubscribeToEvents() {
        this->eventBus->SubscribeToEvent<ProjectileEmittingSystem, KeyPressedEvent>(this, &ProjectileEmittingSystem::OnSpaceKeyPress);
    }

    void OnSpaceKeyPress(KeyPressedEvent &event) {
        for (auto& entity: GetSystemEntities()) {
            auto& emitter = entity.GetComponent<ProjectileEmittingComponent>();
            if (event.key == SDLK_SPACE && emitter.playerControlled) ProjectileSpawn(entity, emitter);
        }
    }

    void Update() {
        for (auto& entity: GetSystemEntities()) {
            auto& emitter = entity.GetComponent<ProjectileEmittingComponent>();
            if (!emitter.playerControlled) ProjectileSpawn(entity, emitter);
        }
    }
private:
    Registry* registry;
    EventBus* eventBus;

    void ProjectileSpawn(const Entity& entity, ProjectileEmittingComponent& emitter) {
        if (SDL_GetTicks() - emitter.emissionTime > emitter.cooldown) {
            const auto& transform = entity.GetComponent<TransformComponent>();
            const auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
            const auto& direction = glm::normalize(rigidBody.velocity);

            glm::vec2 projectilePosition = transform.position;
            if (entity.HasComponent<SpriteComponent>()) {
                const auto& sprite = entity.GetComponent<SpriteComponent>();
                projectilePosition.x += (transform.scale.x * sprite.width / 2);
                projectilePosition.y += (transform.scale.y * sprite.height / 2);
            }

            Entity projectile = this->registry->CreateEntity();
            projectile.AddComponent<TransformComponent>(projectilePosition);
            projectile.AddComponent<RigidBodyComponent>(emitter.velocityScalar * direction);
            projectile.AddComponent<SpriteComponent>("bullet", 4, 4, 4);
            projectile.AddComponent<BoxColliderComponent>(4, 4);
            projectile.AddComponent<ProjectileComponent>(emitter.lifespan, emitter.damage, emitter.affectable);
            projectile.AddGroup("Projectiles");

            emitter.emissionTime = SDL_GetTicks();
        }
    }
};

#endif //PROJECTILEEMITTINGSYSTEM_HPP
