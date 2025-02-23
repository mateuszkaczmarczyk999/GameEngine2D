#ifndef ECS_HPP
#define ECS_HPP

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>

#include "../Logger/Logger.hpp"


const unsigned int COMPONENT_CAPACITY = 32;
typedef std::bitset<COMPONENT_CAPACITY> Signature;

class Entity
{
    public:
        Entity(int id): id(id) {};
        Entity(const Entity& other) = default;
        int GetId() const;

        Entity& operator = (const Entity& other) = default;
        bool operator == (const Entity& other) const { return id == other.id; }
        bool operator != (const Entity& other) const { return id != other.id; }
        bool operator < (const Entity& other) const { return id < other.id; }
        bool operator <= (const Entity& other) const { return id <= other.id; }
        bool operator > (const Entity& other) const { return id > other.id; }
        bool operator >= (const Entity& other) const { return id >= other.id; }

        template <typename T, typename ...TArgs> void AddComponent(TArgs&& ...args);
        template <typename T> void RemoveComponent();
        template <typename T> bool HasComponent() const;
        template <typename T> T& GetComponent() const;

        class Registry* registry;

    private:
        int id;
};

struct IComponent
{
    protected:
        static int nextId;
};

template <typename TComponent>
class Component: public IComponent
{
    public:
        static int GetId()
        {
            static auto id = nextId++;
            return id;
        }
};

class System
{
    public:
        System() = default;
        virtual ~System() = default;
        void AddEntityToSystem(Entity entity);
        void RemoveEntityToSystem(Entity entity);
        const Signature& GetComponentSignature() const;
        std::vector<Entity> GetSystemEntities() const;

        // Defines the component tyoe that entitiies must have to be considered by the system
        template <typename TComponent> void RequireComponent();

    private:
        Signature componentSignature;
        std::vector<Entity> systemEntities;
};

template <typename TComponent>
void System::RequireComponent()
{
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
};

class IPool
{
    public:
        virtual ~IPool() {}
};

template <typename T>
class Pool: public IPool
{
    public:
        Pool(int size) { data.resize(size); }
        virtual ~Pool() = default;
        bool IsEmpty() const { return data.empty(); }
        int GetSize() const { return data.size(); }
        void Resize(int size) { data.resize(size); }
        void Clear() { data.clear(); }
        void Add(T object) { data.push_back(object); }
        void Set(int idx, T object) { data[idx] = object; }
        T& Get(int idx) { return static_cast<T&>(data[idx]); }
        T& operator [](unsigned int idx) { return static_cast<T>(data[idx]); }

    private:
        std::vector<T> data;
};

class Registry
{
    public:
        Registry() {
            Logger::Log("Registry class created.");
        };
        ~Registry() {
            Logger::Log("Registry class destroyed.");
        };
        void Update();
        Entity CreateEntity();

        template <typename T, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
        template <typename T> void RemoveComponent(Entity entity);
        template <typename T> bool HasComponent(Entity entity) const;
        template <typename T> T& GetComponent(Entity entity) const;

        template <typename T, typename ...TArgs> void AddSystem(TArgs&& ...args);
        template <typename T> void RemoveSystem();
        template <typename T> bool HasSystem() const;
        template <typename T> T& GetSystem() const;

        void AddEntityToSystems(Entity entity);
        
    private:
        int countEntitis = 0;
        std::set<Entity> entitiesToBeAdded;
        std::set<Entity> entitiesToBeKilled;

        std::vector<std::shared_ptr<IPool>> componentPools;
        std::vector<Signature> entityComponentSignatures;
        std::unordered_map<std::type_index, std::shared_ptr<System>> systems; 
};

template <typename T, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    if (componentId >= componentPools.size())
    {
        componentPools.resize(componentId + 1, nullptr);
    }

    if (!componentPools[componentId])
    {
        std::shared_ptr<Pool<T>> newComponentPool = std::make_shared<Pool<T>>(1);
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);

    if (entityId >= componentPool->GetSize())
    {
        componentPool->Resize(countEntitis);
    }

    T newComponent(std::forward<TArgs>(args)...);
    componentPool->Set(entityId, newComponent);
    entityComponentSignatures[entityId].set(componentId);

    Logger::Log("Component Id: " + std::to_string(componentId) + " was added to Entity with id: " + std::to_string(entityId));
};

template <typename T>
void Registry::RemoveComponent(Entity entity)
{
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    entityComponentSignatures[entityId].set(componentId, false);

    Logger::Log("Component Id: " + std::to_string(componentId) + " was removed from Entity with id: " + std::to_string(entityId));
}

template <typename T>
bool Registry::HasComponent(Entity entity) const
{
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    return entityComponentSignatures[entityId].test(componentId);
}

template <typename T>
T& Registry::GetComponent(Entity entity) const
{
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);
    return componentPool->Get(entityId);
}

template <typename T, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args)
{
    std::shared_ptr<T> newSystem = std::make_shared<T>(std::forward<TArgs>(args)...);
    std::type_index systemIdx = std::type_index(typeid(T));
    systems.insert(std::make_pair(systemIdx, newSystem));
};

template <typename T>
void Registry::RemoveSystem()
{
    std::type_index systemIdx = std::type_index(typeid(T));
    auto system = systems.find(systemIdx);
    systems.erase(system);
}

template <typename T>
bool Registry::HasSystem() const
{
    std::type_index systemIdx = std::type_index(typeid(T));
    return systems.find(systemIdx) != systems.end();
}

template <typename T>
T& Registry::GetSystem() const
{
    std::type_index systemIdx = std::type_index(typeid(T));
    auto system = systems.find(systemIdx);
    return *(std::static_pointer_cast<T>(system->second));
}

template <typename T, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args)
{
    registry->AddComponent<T>(*this, std::forward<TArgs>(args)...);
};

template <typename T>
void Entity::RemoveComponent()
{
    registry->RemoveComponent<T>(*this);
};

template <typename T>
bool Entity::HasComponent() const
{
    return registry->HasComponent<T>(*this);
};

template <typename T>
T& Entity::GetComponent() const
{
    return registry->GetComponent<T>(*this);
};

#endif