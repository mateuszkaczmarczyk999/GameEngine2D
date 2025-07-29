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

    template<typename TOwner, typename TEvent>
    void SubscribeToEvent(TOwner *owner, void (TOwner::*callback)(TEvent &event)) {
        if (!subscribers[typeid(TEvent)].get()) {
            subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
        }
        auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(owner, callback);
        subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
    }

    template<typename TEvent, typename ...TArgs>
    void EmitEvent(TArgs&& ...args) {
        auto handlers = subscribers[typeid(TEvent)].get();
        if (handlers) {
            for (auto it = handlers->begin(); it != handlers->end(); it++) {
                auto handler = it->get();
                TEvent event(std::forward<TArgs>(args)...);
                handler->Execute(event);
            }
        }
    }
};

#endif //EVENTBUS_HPP
