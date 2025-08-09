#ifndef DAMAGESYSTEM_HPP
#define DAMAGESYSTEM_HPP

#include "../ECS/ECS.hpp"
#include "../Events/EventBus.hpp"
#include "../Events/CollisionEvent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/BoxColliderComponent.hpp"
#include "../Components/HealthComponent.hpp"
#include "../Components/ProjectileComponent.hpp"

class DamageSystem: public System {
public:
    DamageSystem(EventBus* eventBus) {
        this->eventBus = eventBus;
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeToEvents() {
        this->eventBus->SubscribeToEvent<DamageSystem, CollisionEvent>(this, &DamageSystem::OnCollision);
    }

    void OnCollision(CollisionEvent &event) {
        auto& entityA = event.entityA;
        auto& entityB = event.entityB;

        if (entityA.HasGroup("Projectiles") && entityB.HasTag("Player")) {
            OnPlayerDamaged(entityB, entityA);
        }
        if (entityA.HasGroup("Projectiles") && entityB.HasGroup("Enemies")) {
            OnEnemyDamaged(entityB, entityA);;
        }
        if (entityB.HasGroup("Projectiles") && entityA.HasTag("Player")) {
            OnPlayerDamaged(entityA, entityB);
        }
        if (entityB.HasGroup("Projectiles") && entityA.HasGroup("Enemies")) {
            OnEnemyDamaged(entityA, entityB);
        }
    }

    void OnPlayerDamaged(Entity& playerEntity, Entity& projectileEntity) {
        auto& projectile = projectileEntity.GetComponent<ProjectileComponent>();
        if (projectile.affectable) {
            auto& health = playerEntity.GetComponent<HealthComponent>();
            health.healthPercentage -= projectile.damage;
            projectileEntity.Kill();
            if (health.healthPercentage <= 0) playerEntity.Kill();
        }
    }

    void OnEnemyDamaged(Entity& enemyEntity, Entity& projectileEntity) {
        auto& projectile = projectileEntity.GetComponent<ProjectileComponent>();
        if (!projectile.affectable) {
            auto& health = enemyEntity.GetComponent<HealthComponent>();
            health.healthPercentage -= projectile.damage;
            projectileEntity.Kill();
            if (health.healthPercentage <= 0) enemyEntity.Kill();
        }
    }

    void Update() {

    }

private:
    EventBus* eventBus;
};

#endif //DAMAGESYSTEM_HPP
