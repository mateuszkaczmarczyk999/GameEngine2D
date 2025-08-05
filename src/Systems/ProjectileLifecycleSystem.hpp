#ifndef PROJECTILELIFECYCLESYSTEM_HPP
#define PROJECTILELIFECYCLESYSTEM_HPP

#include "../Components/ProjectileComponent.hpp"
#include "../ECS/ECS.hpp"
#include <SDL2/SDL.h>

class ProjectileLifecycleSystem : public System {
public:
    ProjectileLifecycleSystem() {
        RequireComponent<ProjectileComponent>();
    }

    void Update() {
        for (auto& entity: GetSystemEntities()) {
            const auto& projectileComponent = entity.GetComponent<ProjectileComponent>();
            if (SDL_GetTicks() - projectileComponent.emissionTime > projectileComponent.lifespan) {
                entity.Kill();
            }
        }
    }
};

#endif //PROJECTILELIFECYCLESYSTEM_HPP
