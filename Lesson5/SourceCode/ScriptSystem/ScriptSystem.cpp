#include "ScriptSystem.h"


CScriptSystem::CScriptSystem()
{
    return;
}

void CScriptSystem::Update() {

}

IScriptProxy* CScriptSystem::CreateProxy(const char* filename) {
  return nullptr;
}

void CScriptSystem::ProcessScript(IScriptProxy* scriptProxy) {

}
