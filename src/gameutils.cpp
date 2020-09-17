#pragma once
#include <gameutils.h>
#include <sdl_render.h>

entt::entity build_bullet(std::string btemplate, entt::registry& registry, Vec2f velocity, Vec2f location, float rotation)
{
    EntityDatabase& db = registry.ctx<EntityDatabase>();

    //ball
    auto ball_entity = registry.create();

    apply_template(btemplate, db, registry, ball_entity);

    registry.assign<SpriteLocation>(ball_entity, location);
    registry.get<MovementComponent>(ball_entity).velocity = velocity;

    //translate rotation into what sdl expects
    registry.get<SDL_RenderSprite>(ball_entity).rotation = (-rotation) + 90;

    registry.get<SDL_RenderSprite>(ball_entity).has_rotation = true;

    return ball_entity;
}

bool apply_template(std::string_view template_name, EntityDatabase& db, entt::registry& dest_reg, entt::entity dest_et) {

    if (db.templateMap.find(std::string{ template_name }) == db.templateMap.end()) {
        return false;
    }
    else {

        clone_entity(db.databaseRegistry, db.templateMap[std::string{ template_name }], dest_reg, dest_et);

        dest_reg.assign_or_replace<OriginalTemplate>(dest_et, std::string{ template_name });
        dest_reg.assign_or_replace<GameEntity>(dest_et);
        return true;
    }
}