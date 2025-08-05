#ifndef PROJECTILEEMITTINGCOMPONENT_HPP
#define PROJECTILEEMITTINGCOMPONENT_HPP

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

struct ProjectileEmittingComponent {
    glm::vec2 velocity;
    int frequency;
    int lifespan;
    int damage;
    bool affectable;
    int emissionTime;

    ProjectileEmittingComponent(glm::vec2 velocity = glm::vec2(0), int frequency = 0, int lifespan = 0, int damage = 0, bool affectable = false) {
        this->velocity = velocity;
        this->frequency = frequency;
        this->lifespan = lifespan;
        this->damage = damage;
        this->affectable = affectable;
        this->emissionTime = SDL_GetTicks();
    }
};

#endif //PROJECTILEEMITTINGCOMPONENT_HPP
