#ifndef ECS_HPP
#define ECS_HPP

#include <bitset>
#include <vector>

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
        template <typename TComponent> void DefineComponent();

    private:
        Signature componentSignature;
        std::vector<Entity> systemEntities;
};

template <typename TComponent>
void System::DefineComponent()
{
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
};

class Registry
{

};

#endif