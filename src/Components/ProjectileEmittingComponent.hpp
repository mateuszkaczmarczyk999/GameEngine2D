#ifndef PROJECTILEEMITTINGCOMPONENT_HPP
#define PROJECTILEEMITTINGCOMPONENT_HPP

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

struct ProjectileEmittingComponent {
    float velocityScalar;
    int frequency;
    int lifespan;
    int damage;
    bool affectable;
    bool playerControlled;
    int emissionTime;

    ProjectileEmittingComponent(float velocityScalar = 100, int frequency = 0, int lifespan = 0, int damage = 0, bool playerControlled = false, bool affectable = true) {
        this->velocityScalar = velocityScalar;
        this->frequency = frequency;
        this->lifespan = lifespan;
        this->damage = damage;
        this->playerControlled = playerControlled;
        this->affectable = affectable;
        this->emissionTime = SDL_GetTicks();
    }
};

#endif //PROJECTILEEMITTINGCOMPONENT_HPP
