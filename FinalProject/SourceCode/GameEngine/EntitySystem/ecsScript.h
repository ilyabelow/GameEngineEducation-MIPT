#pragma once
#include <vector>
#include <string>
#include <memory>
#include "flecs.h"

class CScriptProxy;
class EntitySystem;

struct Scripts {
    template<typename... Args>
    Scripts(Args... args) : names{ args... } {}
    std::vector<std::string> names;
};

struct ScriptProxyPtr {
	std::shared_ptr<CScriptProxy> ptr;
};

void initialize_script_state(flecs::world& ecs, std::shared_ptr<CScriptProxy> scriptProxy);
void register_ecs_script_systems(flecs::world& ecs);
