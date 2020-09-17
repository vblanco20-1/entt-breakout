#pragma once
#include <components.h>

entt::entity build_bullet(std::string btemplate, entt::registry& registry, Vec2f velocity, Vec2f location, float rotation);
bool apply_template(std::string_view template_name, EntityDatabase& db, entt::registry& dest_reg, entt::entity dest_et);