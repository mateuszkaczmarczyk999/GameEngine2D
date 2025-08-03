#ifndef KEYPRESSEDEVENT_HPP
#define KEYPRESSEDEVENT_HPP

#include "Event.hpp"
#include "../ECS/ECS.hpp"

class KeyPressedEvent : public Event {
public:
    SDL_Keycode key;
    KeyPressedEvent(const SDL_Keycode key) : key(key) {};
};

#endif //KEYPRESSEDEVENT_HPP
