#include <vector>

#include "IScriptSystem.h"
#include "ScriptProxy.h"

class SCRIPTSYSTEM_API CScriptSystem final : public IScriptSystem {
public:
	CScriptSystem();
	virtual void Update(float dt) override;
	virtual std::shared_ptr<CScriptProxy> CreateProxy(const char* filename) override;

private:
	std::vector<std::shared_ptr<CScriptProxy>> bytecode_scripts;
};