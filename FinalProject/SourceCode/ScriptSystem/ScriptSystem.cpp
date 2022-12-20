#include "ScriptProxy.h"
#include "ScriptSystem.h"

CScriptSystem::CScriptSystem()
{
    return;
}

void CScriptSystem::Update(float dt) {
    for (auto& script: bytecode_scripts) {
		if (auto script_ptr = script.lock()) {
            script_ptr->PassValueToLuaScript("dt", dt);
            script_ptr->Execute();
		}
    }
}

std::shared_ptr<CScriptProxy> CScriptSystem::CreateProxy(const char* filename) {
	auto ptr = std::make_shared<CScriptProxy>(filename);
    bytecode_scripts.emplace_back(ptr);
    return ptr;
}
