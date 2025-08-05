#ifndef PROJECTILECOMPONENT_HPP
#define PROJECTILECOMPONENT_HPP

#include <SDL2/SDL.h>

struct ProjectileComponent {
    int lifespan;
    int damage;
    bool affectable;
    int emissionTime;

    ProjectileComponent(int lifespan = 0, int damage = 0, bool affectable = true) {
        this->lifespan = lifespan;
        this->damage = damage;
        this->affectable = affectable;
        this->emissionTime = SDL_GetTicks();
    }
};

#endif //PROJECTILECOMPONENT_HPP
