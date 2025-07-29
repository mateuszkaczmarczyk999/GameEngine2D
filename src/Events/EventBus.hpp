#ifndef EVENTBUS_HPP
#define EVENTBUS_HPP

#include <list>
#include <map>
#include "../Logger/Logger.hpp"
#include "Event.hpp"

class IEventCallback {
private:
    virtual void CallEvent(Event &event) = 0;

public:
    virtual ~IEventCallback() = default;

    void Execute(Event &event) {
        CallEvent(event);
    }
};

template<typename TOwner, typename TEvent>
class EventCallback : public IEventCallback {
private:
    typedef void (TOwner::*CallbackFunction)(TEvent &);

    CallbackFunction callback;
    TOwner *owner;

    virtual void CallEvent(Event &event) override {
        std::invoke(callback, owner, static_cast<TEvent &>(event));
    }

public:
    EventCallback(TOwner *owner, CallbackFunction callback) {
        this->owner = owner;
        this->callback = callback;
    }

    virtual ~EventCallback() override = default;
};

typedef std::list<std::unique_ptr<IEventCallback> > HandlerList;

class EventBus {
private:
    std::map<std::type_index, std::unique_ptr<HandlerList> > subscribers;

public:
    EventBus() {
        Logger::Log("EventBus was created");
    }

    ~EventBus() {
        Logger::Log("EventBus was destroyed");
    }
};

#endif //EVENTBUS_HPP
