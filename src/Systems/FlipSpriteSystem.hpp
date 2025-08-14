#ifndef FLIPSPRITESYSTEM_HPP
#define FLIPSPRITESYSTEM_HPP

#include "../ECS/ECS.hpp"
#include "../Components/SpriteComponent.hpp"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/FlipSpriteComponent.hpp"

class FlipSpriteSystem : public System {
public:
    FlipSpriteSystem() {
        RequireComponent<SpriteComponent>();
        RequireComponent<FlipSpriteComponent>();
        RequireComponent<RigidBodyComponent>();
    };

    void Update() {
        for (auto entity: GetSystemEntities()) {
            auto &sprite = entity.GetComponent<SpriteComponent>();
            const auto &flipSprite = entity.GetComponent<FlipSpriteComponent>();
            const auto &rigidBody = entity.GetComponent<RigidBodyComponent>();
            const auto& direction = glm::normalize(rigidBody.velocity);

            if (glm::abs(direction.x) >= glm::abs(direction.y)) {
                if (direction.x >= 0) sprite.assetId = flipSprite.prefixAssetId + flipSprite.flipSuffix[1];
                else sprite.assetId = flipSprite.prefixAssetId + flipSprite.flipSuffix[3];
            } else {
                if (direction.y <= 0) sprite.assetId = flipSprite.prefixAssetId + flipSprite.flipSuffix[0];
                else sprite.assetId = flipSprite.prefixAssetId + flipSprite.flipSuffix[2];
            }
            Logger::Log("FlipSpriteSystem prefixAssetId: " + flipSprite.prefixAssetId);
            Logger::Log("FlipSpriteSystem assetId: " + sprite.assetId);
        }
    };
};

#endif //FLIPSPRITESYSTEM_HPP
