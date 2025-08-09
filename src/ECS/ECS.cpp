#include "ECS.hpp"
#include "../Logger/Logger.hpp"

int IComponent::nextId = 0;

int Entity::GetId() const {
    return id;
}
void Entity::Kill() {
    registry->KillEntity(*this);
}
void Entity::AddTag(const std::string& tag) const {
    registry->AddTagToEntity(*this, tag);
};
bool Entity::HasTag(const std::string& tag) const {
    return registry->EntityHasTag(*this, tag);
};
void Entity::RemoveTag() const {
    registry->RemoveTagsFromEntity(*this);
};
void Entity::AddGroup(const std::string& group) {
    registry->AddEntityToGroup(*this, group);
};
bool Entity::HasGroup(const std::string& group) const {
    return registry->EntityBelongsToGroup(*this, group);
};
void Entity::RemoveGroups() const {
    registry->RemoveEntityFromGroups(*this);
};

void System::AddEntityToSystem(Entity entity) {
    systemEntities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity) {
    systemEntities.erase(std::remove_if(systemEntities.begin(), systemEntities.end(), [&entity](Entity other) {
        return entity == other;
    }), systemEntities.end());
}

std::vector<Entity> System::GetSystemEntities() const {
    return systemEntities;
}

const Signature &System::GetComponentSignature() const {
    return componentSignature;
}

Entity Registry::CreateEntity() {
    int entityId;

    if (!freeIds.empty()) {
        entityId = freeIds.front();
        freeIds.pop_front();
    } else {
        entityId = countEntities++;
        if (entityId >= entityComponentSignatures.size()) {
            entityComponentSignatures.resize(entityId + 1);
        }
    }

    Entity entity(entityId);
    entity.registry = this;

    entitiesToBeAdded.insert(entity);

    Logger::Log("Entity created with id: " + std::to_string(entityId));

    return entity;
}

void Registry::KillEntity(Entity entity) {
    entitiesToBeKilled.insert(entity);
    Logger::Log("Entity with id: " + std::to_string(entity.GetId()) + " killed.");
}

void Registry::AddEntityToSystems(Entity entity) {
    const auto entityId = entity.GetId();
    const Signature &entityComponentSignature = entityComponentSignatures[entityId];

    for (auto &pair: systems) {
        std::shared_ptr<System> system = pair.second;
        const Signature &systemComponentSignature = system->GetComponentSignature();
        bool isInterested = (systemComponentSignature & entityComponentSignature) == systemComponentSignature;

        if (isInterested) {
            system->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity) {
    for (auto &pair: systems) {
        std::shared_ptr<System> system = pair.second;
        system->RemoveEntityFromSystem(entity);
    }
}

void Registry::AddTagToEntity(const Entity& entity, const std::string& tag) {
    if (const auto entityIt = tagsPerEntities.find(entity.GetId()); entityIt != tagsPerEntities.end()) {
        entitiesPerTags.erase(entityIt->second);
        tagsPerEntities.erase(entityIt);
    }
    if (const auto tagIt = entitiesPerTags.find(tag); tagIt != entitiesPerTags.end()) {
        tagsPerEntities.erase(tagIt->second.GetId());
        entitiesPerTags.erase(tagIt);
    }
    tagsPerEntities.emplace(entity.GetId(), tag);
    entitiesPerTags.emplace(tag, entity);
    Logger::Log("Entity with id: " + std::to_string(entity.GetId()) + " tagged with: " + tag);
}

bool Registry::EntityHasTag(const Entity& entity, const std::string& tag) const {
    if (const auto it = tagsPerEntities.find(entity.GetId()); it != tagsPerEntities.end()) {
        return it->second == tag;
    }
    return false;
}

std::optional<Entity> Registry::GetEntityByTag(const std::string& tag) const {
    if (const auto tagIt = entitiesPerTags.find(tag); entitiesPerTags.end() != tagIt) {
        return tagIt->second;
    }
    return std::nullopt;
}

void Registry::RemoveTagsFromEntity(const Entity& entity) {
    const auto entityIt = tagsPerEntities.find(entity.GetId());
    if (entityIt == tagsPerEntities.end()) return;

    const std::string tag = entityIt->second;
    tagsPerEntities.erase(entityIt);
    entitiesPerTags.erase(tag);
    Logger::Log("Tag: " + tag + " removed from entity: " + std::to_string(entity.GetId()));
}

void Registry::AddEntityToGroup(const Entity& entity, const std::string& group) {
    auto& groups = groupsPerEntities[entity.GetId()];
    auto& entities = entitiesPerGroups[group];

    const auto resultLeft = groups.insert(group).second;
    const auto resultRight = entities.insert(entity).second;

    if (resultLeft != resultRight) {
        if (resultLeft) groups.erase(group);
        if (resultRight) entities.erase(entity);
        Logger::Err("Group mapping desync detected for Entity: " + std::to_string(entity.GetId()));
    }
}

bool Registry::EntityBelongsToGroup(const Entity& entity, const std::string& group) const {
    if (const auto it = entitiesPerGroups.find(group); it != entitiesPerGroups.end()) {
        return it->second.find(entity) != it->second.end();
    }
    return false;
}

std::vector<Entity> Registry::GetEntitiesByGroup(const std::string& group) const {
    if (const auto it = entitiesPerGroups.find(group); it != entitiesPerGroups.end()) {
        return std::vector<Entity>(it->second.begin(), it->second.end());
    }
    return {};
};

void Registry::RemoveEntityFromGroups(const Entity& entity) {
    const auto groupsIt = groupsPerEntities.find(entity.GetId());
    if (groupsIt == groupsPerEntities.end()) return;

    const std::vector<std::string> groups(groupsIt->second.begin(), groupsIt->second.end());

    for (const auto& group: groups) {
        if (const auto it = entitiesPerGroups.find(group); it != entitiesPerGroups.end()) {
            it->second.erase(entity);
            if (it->second.empty()) entitiesPerGroups.erase(it);
        }
    }
    groupsPerEntities.erase(groupsIt);
};

void Registry::Update() {
    for (auto entity: entitiesToBeAdded) {
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();

    for (auto entity: entitiesToBeKilled) {
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetId()].reset();

        for (const auto& pool: componentPools) {
            pool->RemoveEntityFromPool(entity.GetId());
        }

        freeIds.push_back(entity.GetId());

        RemoveTagsFromEntity(entity);
        RemoveEntityFromGroups(entity);
    }
    entitiesToBeKilled.clear();
}
