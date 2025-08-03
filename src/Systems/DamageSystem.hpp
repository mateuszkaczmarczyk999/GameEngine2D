#ifndef DAMAGESYSTEM_HPP
#define DAMAGESYSTEM_HPP

#include "../ECS/ECS.hpp"
#include "../Events/EventBus.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Events/CollisionEvent.hpp"

class DamageSystem: public System {
public:
    DamageSystem(EventBus* eventBus) {
        this->eventBus = eventBus;
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeToEvents() {
        this->eventBus->SubscribeToEvent<DamageSystem, CollisionEvent>(this, &DamageSystem::OnCollision);
    }

    void OnCollision(CollisionEvent &event) {
        event.entityA.Kill();
        event.entityB.Kill();
    }

    void Update() {

    }

private:
    EventBus* eventBus;
};

#endif //DAMAGESYSTEM_HPP
