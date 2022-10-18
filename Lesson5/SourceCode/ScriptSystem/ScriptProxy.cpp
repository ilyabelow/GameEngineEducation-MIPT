#include "ScriptProxy.h"

CScriptProxy::CScriptProxy(const char* filename) {
    lua_script.open_libraries(sol::lib::base);
    sol::load_result load_res = lua_script.load_file(filename);
    sol::protected_function target = load_res;
    bytecode_script = target.dump();
}

void CScriptProxy::Execute() {
    sol::protected_function_result res = lua_script.safe_script(bytecode_script.as_string_view(), sol::script_throw_on_error);
}

sol::state& CScriptProxy::GetState() {
    return lua_script;
}
