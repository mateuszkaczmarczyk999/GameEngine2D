#ifndef MOVEMENTSYSTEM_HPP
#define MOVEMENTSYSTEM_HPP

#include "../ECS/ECS.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Systems/CollisionRenderSystem.hpp"
#include "../Events/CollisionEvent.hpp"

class MovementSystem : public System {
public:
    MovementSystem(EventBus* eventBus) : eventBus(eventBus) {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidBodyComponent>();
    };

    void SubscribeToEvents() {
        this->eventBus->SubscribeToEvent<MovementSystem, CollisionEvent>(this, &MovementSystem::OnCollision);
    };

    void OnCollision(CollisionEvent &event) {
        auto entityA = event.entityA;
        auto entityB = event.entityB;
        if (entityA.HasGroup("Enemies") && (entityB.HasGroup("Obstacles") || entityB.HasGroup("Water"))) {
            OnEnemyHitObstacle(entityA);
        }
        if (entityB.HasGroup("Enemies") && (entityA.HasGroup("Obstacles") || entityA.HasGroup("Water"))) {
            OnEnemyHitObstacle(entityB);
        }
    };

    void OnEnemyHitObstacle(Entity &enemy) {
        if (enemy.HasComponent<RigidBodyComponent>()) {
            auto& rigidBody = enemy.GetComponent<RigidBodyComponent>();
            if (rigidBody.velocity.x != 0) rigidBody.velocity.x *= -1;
            if (rigidBody.velocity.y != 0) rigidBody.velocity.y *= -1;
        }
    };

    void Update(double dt) {
        for (auto entity: GetSystemEntities()) {
            auto &transform = entity.GetComponent<TransformComponent>();
            const auto &rigidBody = entity.GetComponent<RigidBodyComponent>();

            transform.position.x += rigidBody.velocity.x * dt;
            transform.position.y += rigidBody.velocity.y * dt;

            bool entityOutsideBounds =
                transform.position.x >= Game::mapWidth ||
                transform.position.x <= 0 ||
                transform.position.y >= Game::mapHeight ||
                transform.position.y <= 0;

            if (entityOutsideBounds && (entity.HasGroup("Enemies") || entity.HasGroup("Projectiles"))) entity.Kill();
        }
    };
private:
    EventBus* eventBus;
};

#endif
