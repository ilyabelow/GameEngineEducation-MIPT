#include "ScriptProxy.h"
#include "ScriptSystem.h"

CScriptSystem::CScriptSystem()
{
    return;
}

void CScriptSystem::Update(float dt) {
    for (auto& script: bytecode_scripts) {
        script->PassValueToLuaScript("dt", dt);
        script->Execute();
    }
}

std::shared_ptr<CScriptProxy> CScriptSystem::CreateProxy(const char* filename) {
    return bytecode_scripts.emplace_back(std::make_unique<CScriptProxy>(filename));
}
