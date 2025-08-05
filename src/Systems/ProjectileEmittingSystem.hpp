#ifndef PROJECTILEEMITTINGSYSTEM_HPP
#define PROJECTILEEMITTINGSYSTEM_HPP

#include "../Components/ProjectileEmittingComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/ProjectileComponent.hpp"
#include "../ECS/ECS.hpp"

class ProjectileEmittingSystem : public System {
public:
    ProjectileEmittingSystem(Registry* registry) : registry(registry) {
        RequireComponent<TransformComponent>();
        RequireComponent<ProjectileEmittingComponent>();
    }

    void Update() {
        for (auto& entity: GetSystemEntities()) {
            auto& emitter = entity.GetComponent<ProjectileEmittingComponent>();

            if (SDL_GetTicks() - emitter.emissionTime > emitter.frequency) {
                const auto& transform = entity.GetComponent<TransformComponent>();

                glm::vec2 projectilePosition = transform.position;
                if (entity.HasComponent<SpriteComponent>()) {
                    const auto& sprite = entity.GetComponent<SpriteComponent>();
                    projectilePosition.x += (transform.scale.x * sprite.width / 2);
                    projectilePosition.y += (transform.scale.y * sprite.height / 2);
                }

                Entity projectile = this->registry->CreateEntity();
                projectile.AddComponent<TransformComponent>(projectilePosition);
                projectile.AddComponent<RigidBodyComponent>(emitter.velocity);
                projectile.AddComponent<SpriteComponent>("bullet", 4, 4, 4);
                projectile.AddComponent<BoxColliderComponent>(4, 4);
                projectile.AddComponent<ProjectileComponent>(emitter.lifespan, emitter.damage, emitter.affectable);

                emitter.emissionTime = SDL_GetTicks();
            }
        }
    }
private:
    Registry* registry;
};

#endif //PROJECTILEEMITTINGSYSTEM_HPP
