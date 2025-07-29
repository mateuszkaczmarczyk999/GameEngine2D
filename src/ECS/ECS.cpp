#include "ECS.hpp"
#include "../Logger/Logger.hpp"

int IComponent::nextId = 0;

int Entity::GetId() const
{
    return id;
}

void Entity::Kill()
{
    registry->KillEntity(*this);
}

void System::AddEntityToSystem(Entity entity)
{
    systemEntities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{
    systemEntities.erase(std::remove_if(systemEntities.begin(), systemEntities.end(), [&entity](Entity other){
        return entity == other;
    }), systemEntities.end());
}

std::vector<Entity> System::GetSystemEntities() const
{
    return systemEntities;
}

const Signature& System::GetComponentSignature() const
{
    return componentSignature;
}

Entity Registry::CreateEntity()
{
    int entityId;

    if (!freeIds.empty())
    {
        entityId = freeIds.front();
        freeIds.pop_front();
    }
    else
    {
        entityId = countEntitis++;
        if (entityId >= entityComponentSignatures.size()) 
        {
            entityComponentSignatures.resize(entityId + 1);
        }
    }

    Entity entity(entityId);
    entity.registry = this;

    entitiesToBeAdded.insert(entity);

    Logger::Log("Entity created with id: " + std::to_string(entityId));

    return entity;
}

void Registry::KillEntity(Entity entity)
{
    entitiesToBeKilled.insert(entity);
    Logger::Log("Entity with id: " + std::to_string(entity.GetId()) + " killed.");
}

void Registry::AddEntityToSystems(Entity entity)
{
    const auto entityId = entity.GetId();
    const Signature& entityComponentSignature = entityComponentSignatures[entityId];

    for (auto& pair: systems)
    {
        std::shared_ptr<System> system = pair.second;
        const Signature& systemComponentSignature = system->GetComponentSignature();
        bool isInterested = (systemComponentSignature & entityComponentSignature) == systemComponentSignature;

        if (isInterested)
        {
            system->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity)
{
    for (auto& pair: systems)
    {
        std::shared_ptr<System> system = pair.second;
        system->RemoveEntityFromSystem(entity);
    }
}

void Registry::Update()
{
    for (auto entity: entitiesToBeAdded)
    {
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();

    for (auto entity: entitiesToBeKilled)
    {
        RemoveEntityFromSystems(entity);
        entityComponentSignatures[entity.GetId()].reset();
        freeIds.push_back(entity.GetId());
    }
    entitiesToBeKilled.clear();
}
