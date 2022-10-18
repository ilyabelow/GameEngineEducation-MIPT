#pragma once
#include <memory>
#include "Common.h"

class CScriptProxy;

class SCRIPTSYSTEM_API IScriptSystem
{
public:
	virtual void Update(float dt) = 0;
	virtual std::shared_ptr<CScriptProxy> CreateProxy(const char* filename) = 0;
};
