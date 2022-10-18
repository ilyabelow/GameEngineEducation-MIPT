#include "ScriptProxy.h"

CScriptProxy::CScriptProxy(const std::string& filename)
{
    lua_script.open_libraries(sol::lib::base);
    auto load_res = lua_script.load_file(filename);
    assert(load_res.valid());
    sol::protected_function target = load_res;
    m_script = target.dump();
}

void CScriptProxy::Execute()
{
    auto res = lua_script.safe_script(m_script.as_string_view(), sol::script_throw_on_error);
    assert(res.valid());
}

sol::state& CScriptProxy::GetState()
{
    return lua_script;
}
