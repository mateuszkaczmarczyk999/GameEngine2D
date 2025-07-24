#ifndef COLLISIONSYSTEM_HPP
#define COLLISIONSYSTEM_HPP

#include "../Components/TransformComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include <glm/glm.hpp>
#include <vector>

struct AABB
{
    glm::vec2 min;
    glm::vec2 max;
    
    AABB(const glm::vec2& origin, int width, int height)
    {
        min = origin - glm::vec2(width / 2.0f, height / 2.0f);
        max = origin + glm::vec2(width / 2.0f, height / 2.0f);
    };
};

class CollisionSystem: public System
{
    public:
        CollisionSystem()
        {
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        };

        void Update()
        {
            for (auto entity: GetSystemEntities())
            {
                auto& transform = entity.GetComponent<TransformComponent>();
                auto& collider = entity.GetComponent<BoxColliderComponent>();
                int entittyId = entity.GetId();

                AABB currentAABB(transform.position + collider.offset, collider.width, collider.height);

                for (auto otherEntity: GetSystemEntities())
                {
                    int otherEntityId = otherEntity.GetId();
                    if (entittyId == otherEntityId) continue;

                    auto& otherTransform = otherEntity.GetComponent<TransformComponent>();
                    auto& otherCollider = otherEntity.GetComponent<BoxColliderComponent>();

                    AABB otherAABB(otherTransform.position + otherCollider.offset, otherCollider.width, otherCollider.height);

                    if (CheckAABBCollision(currentAABB, otherAABB))
                    {
                        // Handle collision logic here
                        Logger::Log("Collision detected between entity " + std::to_string(entittyId) + " and entity " + std::to_string(otherEntity.GetId()));
                    }
                }

            }
        };

    private:
        bool CheckAABBCollision(const AABB& rectA, const AABB& rectB)
        {
            return (rectA.min.x < rectB.max.x && rectA.max.x > rectB.min.x &&
                    rectA.min.y < rectB.max.y && rectA.max.y > rectB.min.y);
        }
};
#endif