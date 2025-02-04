#include "ECS.hpp"

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
