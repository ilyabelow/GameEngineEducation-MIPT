#pragma once

#include <sol/sol.hpp>

#include "IScriptProxy.h"


class SCRIPTSYSTEM_API CScriptProxy final : public IScriptProxy
{
public:
    CScriptProxy(const std::string& filename);
    virtual void Execute() override;
    template<typename T>
    void BindValue(const std::string& name, T&& value)
    {
        lua_script[name] = std::forward<T>(value);
    }
    sol::state& GetState();
private:
    sol::bytecode m_script;
    sol::state lua_script;
};

