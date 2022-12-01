#pragma once

#include <sol/sol.hpp>

#include "IScriptProxy.h"


class SCRIPTSYSTEM_API CScriptProxy final : public IScriptProxy
{
public:
    CScriptProxy(const char* filename);
    virtual void Execute() override;
    sol::state& GetState();

    template<typename T>
    void PassValueToLuaScript(const char* name, T&& value) {
      lua_script[name] = std::forward<T>(value);
    }
private:
    sol::bytecode bytecode_script;
    sol::state lua_script;
};

