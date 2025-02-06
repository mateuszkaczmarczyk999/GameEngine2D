#include "ECS.hpp"
#include "../Logger/Logger.hpp"

int IComponent::nextId = 0;

int Entity::GetId() const
{
    return id;
}

void System::AddEntityToSystem(Entity entity)
{
    systemEntities.push_back(entity);
}

void System::RemoveEntityToSystem(Entity entity)
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
    entityId = countEntitis++;
    Entity entity(entityId);
    entitiesToBeAdded.insert(entityId);

    if (entityId >= entityComponentSignatures.size()) 
    {
        entityComponentSignatures.resize(entityId + 1);
    }

    Logger::Log("Entity created with id: " + std::to_string(entityId));

    return entity;
}

void Registry::AddEntityToSystems(Entity entity)
{
    const auto entityId = entity.GetId();
    const Signature& entityComponentSignature = entityComponentSignatures[entityId];

    for (auto& pair: systems)
    {
        System* system = pair.second;
        const Signature& systemComponentSignature = system->GetComponentSignature();
        bool isInterested = (systemComponentSignature & entityComponentSignature) == systemComponentSignature;

        if (isInterested)
        {
            system->AddEntityToSystem(entity);
        }
    }
}

void Registry::Update()
{
    for (auto entity: entitiesToBeAdded)
    {
        AddEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();
    //TODO Kill entities in waitlist
}