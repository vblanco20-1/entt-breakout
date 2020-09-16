#include "components.h"
#include <entt/entt.hpp>
#include "sdl_render.h"

template<typename T>
void copycomp(entt::registry& sourcereg, entt::entity sourceEnt, entt::registry& destreg, entt::entity destEnt) {
    if (sourcereg.has<T>(sourceEnt))
    {
        destreg.assign_or_replace<T>(destEnt, sourcereg.get<T>(sourceEnt));
    }
}

void stamp_entity(entt::registry& sourcereg, entt::entity sourceEnt, entt::registry& destreg, entt::entity destEnt)
{
    copycomp<SDL_RenderSprite>(sourcereg, sourceEnt, destreg, destEnt);
    copycomp<SpriteLocation>(sourcereg, sourceEnt, destreg, destEnt);
    copycomp<RenderScale>(sourcereg, sourceEnt, destreg, destEnt);
    copycomp<Ball>(sourcereg, sourceEnt, destreg, destEnt);
    copycomp<Bullet>(sourcereg, sourceEnt, destreg, destEnt);
    copycomp<Brick>(sourcereg, sourceEnt, destreg, destEnt);
    copycomp<MovementComponent>(sourcereg, sourceEnt, destreg, destEnt);
    copycomp<BossMovementComponent>(sourcereg, sourceEnt, destreg, destEnt);
    copycomp<PlayerInputComponent>(sourcereg, sourceEnt, destreg, destEnt);
    copycomp<BulletData>(sourcereg, sourceEnt, destreg, destEnt);
    copycomp<BulletSpawner>(sourcereg, sourceEnt, destreg, destEnt);
    copycomp<SphereCollider>(sourcereg, sourceEnt, destreg, destEnt);    
}
