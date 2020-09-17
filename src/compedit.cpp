#include <compedit.h>
#include "imgui.h"
#include <components.h>
#include <entt/entt.hpp>

template<typename C>
void edit(C& c, entt::entity id) {

}
template<>
void edit<SpriteLocation>(SpriteLocation& data, entt::entity id) {
    ImGui::InputFloat2("SpriteLocation: Location", &data.location.x);
}
template<>
void edit<SphereCollider>(SphereCollider& data, entt::entity id) {
    ImGui::InputFloat("SphereCollider: Radius", &data.radius);
}

template<>
void edit<BulletSpawner>(BulletSpawner& data, entt::entity id) {
    ImGui::InputFloat("BulletSpawner: FireRate", &data.fireRate);
}

template<>
void edit<RenderScale>(RenderScale& data, entt::entity id) {
    ImGui::InputFloat2("RenderScale", &data.scale.x);
}

template<typename C>
void try_edit(entt::registry& registry, entt::entity id) {
    if (registry.has<C>(id)) {
        C& c = registry.get<C>(id);
        edit<C>(c, id);
    }
}

void edit_entity(entt::registry& registry, entt::entity id) {
    try_edit<SpriteLocation>(registry, id);
    try_edit<SphereCollider>(registry, id);
    try_edit<BulletSpawner>(registry, id);
    try_edit<RenderScale>(registry, id);
}
void display_editor(entt::registry& main_reg)
{
    static int selected = -1;
    std::vector<entt::entity> editables;
    std::vector<std::string> editnames;

    EntityDatabase& db = main_reg.ctx<EntityDatabase>();

    static bool bTempEdit = false;
    ImGui::Checkbox("Edit Templates", &bTempEdit);

    if (bTempEdit) {

    
    //auto editview = main_reg.view<Editable, SpriteLocation>();
    for (auto [k,v] : db.templateMap)
    {
        editables.push_back(v);
        editnames.push_back(k);
    }

    if (ImGui::TreeNode("Entity to edit"))
    {
        int n = 0;
       
        for(int n = 0; n < editables.size(); n++){
            auto et = editables[n];
            
            char buf[300];
            sprintf(buf, "%s", editnames[n].c_str());//,et);
           
            if (ImGui::Selectable(buf, selected == n))
                selected = n;
        }

        ImGui::TreePop();
    }
    ImGui::Separator();
    if (selected >= 0) {
        if (selected < editables.size()) {
            auto selname = editnames[selected];
            edit_entity(db.databaseRegistry,editables[selected]);

            if (ImGui::Button("Apply template")) {

                auto stampview = main_reg.view<OriginalTemplate>();
                for (auto e : stampview) {
                    if (stampview.get(e).ogTemplate.compare(selname) == 0)
                    {
                        stamp_entity(db.databaseRegistry, editables[selected], main_reg, e);
                    }
                }
               
            }
        }
    }

    }
    else {
        auto editview = main_reg.view<Editable, SpriteLocation>();
        for (auto et : editview)
        {
            editables.push_back(et);
        }

        if (ImGui::TreeNode("Entity to edit"))
        {
            int n = 0;

            for (int n = 0; n < editables.size(); n++) {
                auto et = editables[n];
                Editable& ed = editview.get<Editable>(et);

                char buf[300];
                sprintf(buf, "%s %d", ed.name.c_str(), et);

                if (ImGui::Selectable(buf, selected == n))
                    selected = n;

            }

            ImGui::TreePop();
        }
        ImGui::Separator();
        if (selected >= 0) {
            if (selected < editables.size()) {
                edit_entity(main_reg, editables[selected]);
            }
        }
    }

}
