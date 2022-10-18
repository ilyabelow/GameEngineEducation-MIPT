#include "ScriptSystem.h"
#include "ScriptProxy.h"

CScriptSystem::CScriptSystem()
{
    return;
}

void CScriptSystem::Update(float dt)
{
    for (auto& script : m_scripts)
    {
        script->BindValue("delta_time", dt);
        script->Execute();
    }
}

std::shared_ptr<CScriptProxy> CScriptSystem::CreateProxy(const char* filename)
{
    return m_scripts.emplace_back(std::make_unique<CScriptProxy>(filename));
}
