#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include "../Logger/Logger.hpp"
#include "../ECS/ECS.hpp"
#include "../Components/Transform.hpp"
#include "../Components/RigidBody.hpp"

class MovementSystem: public System
{
    public:
        MovementSystem()
        {
            RequireComponent<TransformComponent>();
            RequireComponent<RigidBodyComponent>();
        };

        void Update(double dt)
        {
            for (auto entity: GetSystemEntities())
            {
                auto& transform = entity.GetComponent<TransformComponent>();
                const auto rigidBody = entity.GetComponent<RigidBodyComponent>();

                transform.position.x += rigidBody.velocity.x * dt;
                transform.position.y += rigidBody.velocity.y * dt;
            }
        };
};

#endif