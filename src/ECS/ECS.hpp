#ifndef ECS_HPP
#define ECS_HPP

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <deque>
#include <iostream>

#include "../Logger/Logger.hpp"


const unsigned int COMPONENT_CAPACITY = 32;
typedef std::bitset<COMPONENT_CAPACITY> Signature;

class Entity {
public:
    Entity(int id): id(id) {
    };

    Entity(const Entity &other) = default;

    int GetId() const;

    void Kill();

    Entity &operator =(const Entity &other) = default;

    bool operator ==(const Entity &other) const { return id == other.id; }
    bool operator !=(const Entity &other) const { return id != other.id; }
    bool operator <(const Entity &other) const { return id < other.id; }
    bool operator <=(const Entity &other) const { return id <= other.id; }
    bool operator >(const Entity &other) const { return id > other.id; }
    bool operator >=(const Entity &other) const { return id >= other.id; }

    template<typename T, typename... TArgs>
    void AddComponent(TArgs &&... args);

    template<typename T>
    void RemoveComponent();

    template<typename T>
    bool HasComponent() const;

    template<typename T>
    T &GetComponent() const;

    void AddTag(const std::string& tag) const;
    bool HasTag(const std::string& tag) const;
    void RemoveTag() const;

    void AddGroup(const std::string& group);
    bool HasGroup(const std::string& group) const;
    void RemoveGroups() const;

    class Registry *registry;

private:
    int id;
};

struct IComponent {
protected:
    static int nextId;
};

template<typename TComponent>
class Component : public IComponent {
public:
    static int GetId() {
        static auto id = nextId++;
        return id;
    }
};

class System {
public:
    System() = default;

    virtual ~System() = default;

    void AddEntityToSystem(Entity entity);

    void RemoveEntityFromSystem(Entity entity);

    const Signature &GetComponentSignature() const;

    std::vector<Entity> GetSystemEntities() const;

    // Defines the component type that entities must have to be considered by the system
    template<typename TComponent>
    void RequireComponent();

private:
    Signature componentSignature;
    std::vector<Entity> systemEntities;
};

template<typename TComponent>
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
};

class IPool {
public:
    virtual ~IPool() = default;
    virtual void RemoveEntityFromPool(int entityId) = 0;
};

template<typename T>
class Pool : public IPool {
private:
    int size;
    std::vector<T> data;
    std::unordered_map<int, int> entityToIndex;
    std::unordered_map<int, int> indexToEntity;

public:
    Pool(int capacity = 100) {
        data.resize(capacity);
        size = 0;
    }

    virtual ~Pool() = default;

    bool IsEmpty() const { return size == 0; }
    int GetSize() const { return size; }
    void Resize(int size) { data.resize(size); }
    void Clear() {
        data.clear();
        entityToIndex.clear();
        indexToEntity.clear();
        size = 0;
    }
    void Set(int entityId, T object) {
        if (const auto idxIt = entityToIndex.find(entityId); idxIt != entityToIndex.end()) {
            const auto idx = idxIt->second;
            data[idx] = object;
            return;
        }
        const int nextIdx = size;
        entityToIndex.emplace(entityId, nextIdx);
        indexToEntity.emplace(nextIdx, entityId);
        if (nextIdx >= data.capacity()) {
            data.resize(nextIdx * 2);
        }
        data[nextIdx] = object;
        size++;
    }
    void Remove(const int entityId) {
        if (size == 0) return;
        const int lastIdx = size - 1;
        if (entityId == indexToEntity.at(lastIdx)) {
            entityToIndex.erase(entityId);
            indexToEntity.erase(lastIdx);
            // data[lastIdx] = nullptr;
            size--;
            return;
        }
        const int emptySocketIdx = entityToIndex.at(entityId);
        const int entityToReplace = indexToEntity.at(lastIdx);
        entityToIndex[entityToReplace] = emptySocketIdx;
        indexToEntity[emptySocketIdx] = entityToReplace;
        data[emptySocketIdx] = data[lastIdx];

        // data[lastIdx] = nullptr;
        indexToEntity.erase(lastIdx);
        entityToIndex.erase(entityId);
        size--;
    }

    void RemoveEntityFromPool(int entityId) override {
        if (Contains(entityId)) {
            Remove(entityId);
        }
    }

    bool Contains(const int entityId) const {
        return entityToIndex.find(entityId) != entityToIndex.end();
    }
    T& Get(const int entityId) {
        int idx = entityToIndex[entityId];
        return static_cast<T&>(data[idx]);
    }
    T& operator [](const int entityId) {
        int idx = entityToIndex[entityId];
        return static_cast<T&>(data[idx]);
    }
};

class Registry {
public:
    Registry() {
        Logger::Log("Registry class created.");
    };

    ~Registry() {
        Logger::Log("Registry class destroyed.");
    };

    void Update();

    Entity CreateEntity();

    void KillEntity(Entity entity);

    template<typename T, typename... TArgs>
    void AddComponent(Entity entity, TArgs &&... args);

    template<typename T>
    void RemoveComponent(Entity entity);

    template<typename T>
    bool HasComponent(Entity entity) const;

    template<typename T>
    T &GetComponent(Entity entity) const;

    template<typename T, typename... TArgs>
    void AddSystem(TArgs &&... args);

    template<typename T>
    void RemoveSystem();

    template<typename T>
    bool HasSystem() const;

    template<typename T>
    T &GetSystem() const;

    void AddTagToEntity(const Entity& entity, const std::string& tag);
    bool EntityHasTag(const Entity& entity, const std::string& tag) const;
    std::optional<Entity> GetEntityByTag(const std::string& tag) const;
    void RemoveTagsFromEntity(const Entity& entity);

    void AddEntityToGroup(const Entity& entity, const std::string &group);
    bool EntityBelongsToGroup(const Entity& entity, const std::string& group) const;
    std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
    void RemoveEntityFromGroups(const Entity& entity);

    void AddEntityToSystems(Entity entity);
    void RemoveEntityFromSystems(Entity entity);

private:
    int countEntities = 0;
    std::deque<int> freeIds;
    std::set<Entity> entitiesToBeAdded;
    std::set<Entity> entitiesToBeKilled;

    std::unordered_map<int, std::string> tagsPerEntities;
    std::unordered_map<std::string, Entity> entitiesPerTags;

    std::unordered_map<int, std::set<std::string>> groupsPerEntities;
    std::unordered_map<std::string, std::set<Entity>> entitiesPerGroups;

    std::vector<std::shared_ptr<IPool> > componentPools;
    std::vector<Signature> entityComponentSignatures;
    std::unordered_map<std::type_index, std::shared_ptr<System> > systems;
};

template<typename T, typename... TArgs>
void Registry::AddComponent(const Entity entity, TArgs &&... args) {
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    if (componentId >= componentPools.size()) {
        componentPools.resize(componentId + 1, nullptr);
    }

    if (!componentPools[componentId]) {
        std::shared_ptr<Pool<T> > newComponentPool = std::make_shared<Pool<T>>(1);
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);

    T newComponent(std::forward<TArgs>(args)...);
    componentPool->Set(entityId, newComponent);
    entityComponentSignatures[entityId].set(componentId);

    Logger::Log("Component Id: " + std::to_string(componentId) + " was added to Entity with id: " + std::to_string(entityId));
};


template<typename T>
void Registry::RemoveComponent(const Entity entity) {
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    std::shared_ptr<Pool<T>> componentPool = std::static_pointer_cast<Pool<T>>(componentPools[componentId]);
    componentPool->Remove(entityId);

    entityComponentSignatures[entityId].set(componentId, false);

    Logger::Log("Component Id: " + std::to_string(componentId) + " was removed from Entity with id: " + std::to_string(entityId));
}

template<typename T>
bool Registry::HasComponent(Entity entity) const {
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    return entityComponentSignatures[entityId].test(componentId);
}

template<typename T>
T &Registry::GetComponent(Entity entity) const {
    const auto componentId = Component<T>::GetId();
    const auto entityId = entity.GetId();

    std::shared_ptr<Pool<T> > componentPool = std::static_pointer_cast<Pool<T> >(componentPools[componentId]);
    return componentPool->Get(entityId);
}

template<typename T, typename... TArgs>
void Registry::AddSystem(TArgs &&... args) {
    std::shared_ptr<T> newSystem = std::make_shared<T>(std::forward<TArgs>(args)...);
    std::type_index systemIdx = std::type_index(typeid(T));
    systems.insert(std::make_pair(systemIdx, newSystem));
};

template<typename T>
void Registry::RemoveSystem() {
    std::type_index systemIdx = std::type_index(typeid(T));
    auto system = systems.find(systemIdx);
    systems.erase(system);
}

template<typename T>
bool Registry::HasSystem() const {
    std::type_index systemIdx = std::type_index(typeid(T));
    return systems.find(systemIdx) != systems.end();
}

template<typename T>
T &Registry::GetSystem() const {
    std::type_index systemIdx = std::type_index(typeid(T));
    auto system = systems.find(systemIdx);
    return *(std::static_pointer_cast<T>(system->second));
}

template<typename T, typename... TArgs>
void Entity::AddComponent(TArgs &&... args) {
    registry->AddComponent<T>(*this, std::forward<TArgs>(args)...);
};

template<typename T>
void Entity::RemoveComponent() {
    registry->RemoveComponent<T>(*this);
};

template<typename T>
bool Entity::HasComponent() const {
    return registry->HasComponent<T>(*this);
};

template<typename T>
T &Entity::GetComponent() const {
    return registry->GetComponent<T>(*this);
};

#endif
