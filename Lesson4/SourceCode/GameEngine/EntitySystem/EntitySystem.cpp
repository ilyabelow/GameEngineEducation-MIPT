#include "EntitySystem.h"
#include "ecsSystems.h"
#include "ecsMesh.h"
#include "ecsControl.h"
#include "ecsPhys.h"

EntitySystem::EntitySystem(RenderEngine* renderEngine, InputHandler* inputHandler) :
  entitiesMaxCount(10)
{
  ecs.entity("inputHandler")
    .set(InputHandlerPtr{ inputHandler });
  ecs.entity("renderEngine")
    .set(RenderEnginePtr{ renderEngine })
    .set(CubeMeshes{})
    .set(IcosahedraMeshes{});

  auto bullet = ecs.prefab()
    .set(Position{ 0.0f, 0.0f, -100.0f })
    .set(Velocity{ 0.0f, 0.0f, 0.0f })
    .set(Hitbox{ 0.125f, 0.125f, 0.125f })
    .set(Bounciness{ 0.f })
    .set(BouncePlane{ 0.f, 1.f, 0.f, -0.875f })
    .set(Gravity{ 0.f, -9.8065f, 0.f })
    .set(FrictionAmount{ 0.7f })
    .add<SelfDestruct>()
    .add<Bullet>();

  register_ecs_mesh_systems(ecs);
  register_ecs_control_systems(ecs);
  register_ecs_phys_systems(ecs);

  for (int i = 0; i < entitiesMaxCount; i++)
    ecs.entity().add<Created>();

  auto gun = ecs.entity()
    .set(Position{ 0.f, 0.f, -3.f })
    .set(Velocity{ 0.f, 0.f, 0.f })
    .set(Speed{ 10.f })
    .set(FrictionAmount{ 2.0f })
    .set(JumpSpeed{ 10.f })
    .set(Gravity{ 0.f, -9.8065f, 0.f })
    .set(BouncePlane{ 0.f, 1.f, 0.f, 0.f })
    .set(Bounciness{ 0.3f })
    .set(GiveGun{ bullet, 6 })
    .set(ReloadTimer{ 6, 2.0f })
    .add<Controllable>()
    .add<CubeMesh>();

  auto target = ecs.prefab()
    .set_override(Position{ 0.f, 1.f, 0.f })
    .set_override(Velocity{ 0.f, 7.f, 0.f })
    .set(BouncePlane{ 0.f, 1.f, 0.f, 0.f })
    .set(Bounciness{ 1.0f })
    .set(Hitbox{ 1.f, 1.f, 1.f })
    .set(AddToMagazine{ 3 })
    .set(Gravity{ 0.f, -9.8065f, 0.f })
    .add<Target>();

  int targetCount = 5;
  for (int i = 0; i < targetCount; i++) {
    ecs.entity()
      .is_a(target)
      .set(Position{(-targetCount/2 + i) * 4.0f, 1.0f, 10.f})
      .set(Velocity{ 0.f, 9.f, 0.f })
      .add<CubeMesh>()
      .set(Respawnable{target});
  }
}

void EntitySystem::Update()
{
  static auto toCreateQuery = ecs.query<Created>();
  ecs.progress();
  int createdEntitiesCount = 0;
  toCreateQuery.each([&](Created&) { createdEntitiesCount++; });
  for (int i = createdEntitiesCount; i < entitiesMaxCount; ++i)
    ecs.entity().add<Created>();
}