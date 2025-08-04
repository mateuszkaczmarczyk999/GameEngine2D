#ifndef KEYBOARDMOVEMENTSYSTEM_HPP
#define KEYBOARDMOVEMENTSYSTEM_HPP

#include "../ECS/ECS.hpp"
#include "../Events/EventBus.hpp"
#include "../Events/KeyPressedEvent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/KeyboardMovementComponent.hpp"

class KeyboardMovementSystem: public System {
public:
    KeyboardMovementSystem(EventBus* eventBus) {
        this->eventBus = eventBus;
        RequireComponent<SpriteComponent>();
        RequireComponent<RigidBodyComponent>();
        RequireComponent<KeyboardMovementComponent>();
    }

    void SubscribeToEvents() {
        this->eventBus->SubscribeToEvent<KeyboardMovementSystem, KeyPressedEvent>(this, &KeyboardMovementSystem::OnKeyPress);
    }

    void OnKeyPress(KeyPressedEvent &event) {
        for (auto entity: GetSystemEntities()) {
            const auto &keyboardMovement = entity.GetComponent<KeyboardMovementComponent>();
            auto &rigidBody = entity.GetComponent<RigidBodyComponent>();
            auto &sprite = entity.GetComponent<SpriteComponent>();

            switch (event.key) {
                case SDLK_UP:
                    rigidBody.velocity = keyboardMovement.upVelocity;
                    sprite.srcRect.y = sprite.height * 0;
                    break;
                case SDLK_RIGHT:
                    rigidBody.velocity = keyboardMovement.rightVelocity;
                    sprite.srcRect.y = sprite.height * 1;
                    break;
                case SDLK_DOWN:
                    rigidBody.velocity = keyboardMovement.downVelocity;
                    sprite.srcRect.y = sprite.height * 2;
                    break;
                case SDLK_LEFT:
                    rigidBody.velocity = keyboardMovement.leftVelocity;
                    sprite.srcRect.y = sprite.height * 3;
                    break;
            }
        }
    }

    void Update() {

    }

private:
    EventBus* eventBus;
};

#endif //KEYBOARDMOVEMENTSYSTEM_HPP
