#ifndef CAMERAFOLLOWSYSTEM_HPP
#define CAMERAFOLLOWSYSTEM_HPP

#include "../ECS/ECS.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/CameraFollowComponent.hpp"
#include <SDL2/SDL.h>

class CameraFollowSystem : public System {
public:
    CameraFollowSystem(SDL_Rect& cameraFrame): cameraFrame(cameraFrame) {
        RequireComponent<CameraFollowComponent>();
        RequireComponent<TransformComponent>();
    };

    void Update() {
        for (auto entity: GetSystemEntities()) {
            const auto& transform = entity.GetComponent<TransformComponent>();

            cameraFrame.x = (cameraFrame.x >= 0 && cameraFrame.x + cameraFrame.w / 2 <= cameraFrame.w) ? (transform.position.x - cameraFrame.w / 2) : cameraFrame.x;
            cameraFrame.y = (cameraFrame.y >= 0 && cameraFrame.y + cameraFrame.h / 2 <= cameraFrame.h) ? (transform.position.y - cameraFrame.h / 2) : cameraFrame.y;

            cameraFrame.x = cameraFrame.x < 0 ? 0 : cameraFrame.x;
            cameraFrame.y = cameraFrame.y < 0 ? 0 : cameraFrame.y;
            cameraFrame.x = cameraFrame.x + cameraFrame.w / 2 > cameraFrame.w ? cameraFrame.w / 2 : cameraFrame.x;
            cameraFrame.y = cameraFrame.y + cameraFrame.h / 2 > cameraFrame.h ? cameraFrame.h / 2 : cameraFrame.y;
        }
    };

private:
    SDL_Rect& cameraFrame;
};

#endif //CAMERAFOLLOWSYSTEM_HPP
