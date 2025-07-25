#ifndef COLLISIONRENDERSYSTEM_HPP
#define COLLISIONRENDERSYSTEM_HPP

#include "../Components/TransformComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <SDL2/SDL.h>

struct AABB2D
{
    glm::vec2 min;
    glm::vec2 max;

    AABB2D(const glm::vec2 &origin, int width, int height)
    {
        min = origin - glm::vec2(width / 2.0f, height / 2.0f);
        max = origin + glm::vec2(width / 2.0f, height / 2.0f);
    };
};

class CollisionRenderSystem : public System
{
    public:
        CollisionRenderSystem()
        {
            RequireComponent<TransformComponent>();
            RequireComponent<BoxColliderComponent>();
        };

        void Update(SDL_Renderer* renderer)
        {
            auto entities = GetSystemEntities();

            for (auto i = entities.begin(); i != entities.end(); i++)
            {
                Entity entityA = *i;
                auto &transformA = entityA.GetComponent<TransformComponent>();
                auto &colliderA = entityA.GetComponent<BoxColliderComponent>();

                AABB2D aabbForEntityA(transformA.position + colliderA.offset, colliderA.width, colliderA.height);

                for (auto j = i; j != entities.end(); j++)
                {
                    if (i == j) continue; // Skip self-collision
                    
                    Entity entityB = *j;
                    auto &transformB = entityB.GetComponent<TransformComponent>();
                    auto &colliderB = entityB.GetComponent<BoxColliderComponent>();

                    AABB2D aabbForEntityB(transformB.position + colliderB.offset, colliderB.width, colliderB.height);

                    if (CheckAABBCollision(aabbForEntityA, aabbForEntityB))
                    {
                        // Handle collision logic here
                        Logger::Log("Collision detected between entity " + std::to_string(entityA.GetId()) + " and entity " + std::to_string(entityB.GetId()));
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    }
                    else
                    {
                        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                    }
                }

                
                SDL_Rect rect = {
                    static_cast<int>(transformA.position.x + colliderA.offset.x),
                    static_cast<int>(transformA.position.y + colliderA.offset.y),
                    colliderA.width,
                    colliderA.height,
                };
                SDL_RenderDrawRect(renderer, &rect);
            }
        };

    private:
        bool CheckAABBCollision(const AABB2D &rectA, const AABB2D &rectB)
        {
            return (rectA.min.x < rectB.max.x && rectA.max.x > rectB.min.x &&
                    rectA.min.y < rectB.max.y && rectA.max.y > rectB.min.y);
        }
    };
#endif