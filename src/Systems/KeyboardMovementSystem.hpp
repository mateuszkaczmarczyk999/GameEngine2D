#ifndef KEYBOARDMOVEMENTSYSTEM_HPP
#define KEYBOARDMOVEMENTSYSTEM_HPP

#include "../ECS/ECS.hpp"
#include "../Events/EventBus.hpp"
#include "../Events/KeyPressedEvent.hpp"

class KeyboardMovementSystem: public System {
public:
    KeyboardMovementSystem(EventBus* eventBus) {
        this->eventBus = eventBus;
    }

    void SubscribeToEvents() {
        this->eventBus->SubscribeToEvent<KeyboardMovementSystem, KeyPressedEvent>(this, &KeyboardMovementSystem::OnKeyPress);
    }

    void OnKeyPress(KeyPressedEvent &event) {
        auto keyChar = SDL_GetKeyName(event.key);
        Logger::Log("Key pressed: " + std::string(keyChar));
    }

    void Update() {

    }

private:
    EventBus* eventBus;
};

#endif //KEYBOARDMOVEMENTSYSTEM_HPP
