#include "EntitySystem.h"
#include "ecsSystems.h"
#include "ecsMesh.h"
#include "ecsControl.h"
#include "ecsPhys.h"
#include "ecsScript.h"
#include "CreateEntityXML.h"
#include <iostream>
EntitySystem::EntitySystem(RenderEngine* renderEngine, InputHandler* inputHandler, IScriptSystem* scriptSystem):
  entitiesMaxCount(30)
{
  ecs.entity("renderEngine")
    .set(RenderEnginePtr{ renderEngine })
    .set(CubeMeshes{})
    .set(IcosahedraMeshes{});
  ecs.entity("scriptSystem")
    .set(ScriptSystemPtr{ scriptSystem });

  register_ecs_mesh_systems(ecs);
  register_ecs_control_systems(ecs);
  register_ecs_phys_systems(ecs);
  register_ecs_script_systems(ecs);

  
  ecs.system<const Destruct&, Remote*, RenderProxyPtr*, ScriptProxyPtr*, Sync*>("Removing disconnected players")
	  .each([this](flecs::entity e, const Destruct& d, Remote* remote, RenderProxyPtr* rp, ScriptProxyPtr* sp, Sync* sync) {
            if (remote) {
               return;
            }
            if (rp) {
				rp->ptr->SetRenderState(false);
            }
            if (sp) {
                sp->ptr.reset();
            }
            if (m_remoteDelete && sync) {
				m_toSyncedDelete.push_back(e);
			}
            e.destruct();
        }
  );
  for (int i = 0; i < entitiesMaxCount; i++)
    ecs.entity().add<Created>();

  auto bullet = ecs.prefab("bullet_prefab")
    .set(Position{ 0.0f, 0.0f, 0.0f })
    .set(Velocity{ 0.0f, 0.0f, 0.0f })
    .set(Hitbox{ 0.125f, 0.125f, 0.125f })
    .set(Bounciness{ 0.6f })
    .set(BouncePlane{ 0.f, 1.f, 0.f, -0.875f })
    .set(Gravity{ 0.f, -9.8065f, 0.f })
    .set(FrictionAmount{ 0.7f })
    .add<SelfDestruct>()
    .add<Bullet>()
    .set<Sync>({ Serializable::Bullet::type, 1 , 0});
  
  player = CreatePlayer(&inputHandler->GetInputState());

  auto target = ecs.prefab("target")
      .set_override(Position{ 0.f, 1.f, 0.f })
      .set_override(Velocity{ 0.f, 7.f, 0.f })
      .set(BouncePlane{ 0.f, 1.f, 0.f, 0.f })
      .set(Bounciness{ 1.0f })
      .set(Hitbox{ 1.f, 1.f, 1.f })
      .set(AddToMagazine{ 3 })
      .set(Gravity{ 0.f, -9.8065f, 0.f })
      .add<Target>()
      .set<Sync>({ Serializable::Enemy::type, 1 , 0});

  StartTargets();
}

void EntitySystem::Update()
{
  static auto toCreateQuery = ecs.query<Created>();
  ecs.progress();
  int createdEntitiesCount = 0;
  toCreateQuery.each([&](const Created &created) { createdEntitiesCount++; });
  for (int i = createdEntitiesCount; i < entitiesMaxCount; ++i)
    ecs.entity().add<Created>();
}
flecs::entity EntitySystem::CreatePlayer(const InputState* a_inputState)
{
    auto gun = createEntityFromXML(ecs, "../../../Assets/Data/gun.xml")
        .set(GiveGun{ ecs.lookup("bullet_prefab"), 6 })
        .set(InputStatePtr{ a_inputState })
        .set<Sync>({ Serializable::Player::type, 0, 0 });
        //.add<Local>();
    
    return gun;
}

flecs::entity EntitySystem::CreatePlayer()
{
    auto gun = createEntityFromXML(ecs, "../../../Assets/Data/gun.xml")
        .set(GiveGun{ ecs.lookup("bullet_prefab"), 6 })
        .add<Remote>()
        .remove<Scripts>();
    return gun;
}

flecs::entity EntitySystem::CreateBullet()
{
    return ecs.entity()
        .is_a(ecs.lookup("bullet_prefab"))
        .add< Remote>()
        .add<IcosahedronMesh>();
}

flecs::entity EntitySystem::CreateTarget()
{
    auto target = ecs.lookup("target");
    return  ecs.entity()
        .is_a(target)
        .add< Remote>()
        .add<CubeMesh>();
}

void EntitySystem::HaltLocalCreation()
{
    ecs.system<Local>("no_local").each([this](flecs::entity e, Local local) {
            e.remove<Local>().add<Destruct>();
        }
    );
}

void EntitySystem::ResumeLocalCreation()
{
    ecs.lookup("no_local").destruct();
}


void EntitySystem::StartSync()
{
    m_remoteDelete = true;
    ecs.observer<Sync>("OnSetSync").event(flecs::OnSet).each([this](flecs::entity e, Sync l)
        {
            m_toSyncedCreate.push_back(e);
        }
    );
    
    ecs.system<Sync>("sync").each(
        [this](flecs::entity e, Sync sync) {
            sync.timeElapsed -= e.delta_time();
            if (sync.timeElapsed <= 0) {
                sync.timeElapsed = sync.timeToSync;
                m_toSync.push_back(e);
            }
		}
	);
}

void EntitySystem::StopSync()
{
    m_remoteDelete = false;

    ecs.lookup("sync").destruct();
    ecs.lookup("OnSetSync").destruct();
}

void EntitySystem::StartTargets()
{
	flecs::entity target = ecs.lookup("target");
    int targetCount = 5;
    for (int i = 0; i < targetCount; i++) {
        ecs.entity()
            .is_a(target)
            .set(Position{ (-targetCount / 2 + i) * 4.0f, 1.0f, 10.f })
            .set(Velocity{ 0.f, 9.f, 0.f })
            .add<CubeMesh>()
            .add<Local>()
            .set(Respawnable{ target });
    }
}
