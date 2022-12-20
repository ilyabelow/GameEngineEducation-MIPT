#pragma once

#include "flecs.h"
#include "RenderEngine.h"
#include "../InputHandler.h"


class IScriptSystem;

class EntitySystem
{
public:
	EntitySystem() = delete;
	EntitySystem(RenderEngine* renderEngine, InputHandler* inputHandler, IScriptSystem* scriptSystem);

	void Update();

	flecs::entity CreatePlayer(const InputState* a_inputState);
	flecs::entity CreatePlayer();
	flecs::entity CreateBullet();
	flecs::entity CreateTarget();

	void HaltLocalCreation();
	void ResumeLocalCreation();
	void StartSync();
	void StopSync();
	
	void StartTargets();

	std::vector<flecs::entity> m_toSyncedDelete;
	std::vector<flecs::entity> m_toSyncedCreate;
	std::vector<flecs::entity> m_toSync;
	
	flecs::world ecs;
	flecs::entity player;
private:
	int entitiesMaxCount;
	bool m_remoteDelete = false;
};

