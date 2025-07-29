#ifndef COLLISIONSYSTEM_HPP
#define COLLISIONSYSTEM_HPP

#include "../Components/TransformComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include <glm/glm.hpp>
#include <vector>

struct AABB {
    glm::vec2 min;
    glm::vec2 max;

    AABB(const glm::vec2 &origin, int width, int height) {
        min = origin - glm::vec2(width / 2.0f, height / 2.0f);
        max = origin + glm::vec2(width / 2.0f, height / 2.0f);
    };
};

class CollisionSystem : public System {
public:
    CollisionSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    };

    void Update() {
        auto entities = GetSystemEntities();

        for (auto i = entities.begin(); i != entities.end(); i++) {
            Entity entityA = *i;
            auto &transformA = entityA.GetComponent<TransformComponent>();
            auto &colliderA = entityA.GetComponent<BoxColliderComponent>();

            AABB aabbForEntityA(transformA.position + colliderA.offset, colliderA.width, colliderA.height);

            for (auto j = i; j != entities.end(); j++) {
                if (i == j) continue; // Skip self-collision

                Entity entityB = *j;
                auto &transformB = entityB.GetComponent<TransformComponent>();
                auto &colliderB = entityB.GetComponent<BoxColliderComponent>();

                AABB aabbForEntityB(transformB.position + colliderB.offset, colliderB.width, colliderB.height);

                if (CheckAABBCollision(aabbForEntityA, aabbForEntityB)) {
                    // Handle collision logic here
                    Logger::Log(
                        "Collision detected between entity " + std::to_string(entityA.GetId()) + " and entity " +
                        std::to_string(
                            entityB.GetId()));
                }
            }
        }
    };

private:
    bool CheckAABBCollision(const AABB &rectA, const AABB &rectB) {
        return (rectA.min.x < rectB.max.x && rectA.max.x > rectB.min.x &&
                rectA.min.y < rectB.max.y && rectA.max.y > rectB.min.y);
    }
};
#endif
