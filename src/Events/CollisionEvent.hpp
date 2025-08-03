#ifndef COLLISIONEVENT_HPP
#define COLLISIONEVENT_HPP

#include "Event.hpp"
#include "../ECS/ECS.hpp"

class CollisionEvent : public Event {
public:
    Entity entityA;
    Entity entityB;

    CollisionEvent(const Entity &entityA, const Entity &entityB): entityA(entityA), entityB(entityB) {};
};

#endif //COLLISIONEVENT_HPP
