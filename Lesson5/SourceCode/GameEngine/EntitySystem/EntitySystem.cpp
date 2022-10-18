#include "EntitySystem.h"
#include "ecsSystems.h"
#include "ecsMesh.h"
#include "ecsControl.h"
#include "ecsPhys.h"
#include "ecsScript.h"

EntitySystem::EntitySystem(RenderEngine* renderEngine, InputHandler* inputHandler, IScriptSystem* scriptSystem):
    creatableMaxCount(30)
{
    ecs.entity("inputHandler")
        .set(InputHandlerPtr{ inputHandler });
    ecs.entity("renderEngine")
        .set(RenderEnginePtr{ renderEngine })
        .set(CubeMeshes{});
    ecs.entity("scriptSystem")
        .set(ScriptSystemPtr{ scriptSystem });

    auto projectile = ecs.prefab()
        .set_override(Position{ 0.0f, 0.0f, 0.0f })
        .set_override(Velocity{ 0.0f, 0.0f, 0.0f })
        .set(Hitbox{ 0.25f, 0.25f, 0.25f })
        .set(Bounciness{ 0.8f })
        .set(BouncePlane{ 0.f, 1.f, 0.f, -5.0f })
        .set(Gravity{ 0.f, -9.8065f, 0.f })
        .set(FrictionAmount{ 0.7f })
        .add<Bullet>();

    register_ecs_mesh_systems(ecs);
    register_ecs_control_systems(ecs);
    register_ecs_phys_systems(ecs);
    register_ecs_script_systems(ecs);

    for (int i = 0; i < creatableMaxCount; i++)
        ecs.entity().add<Creatable>();

    auto cubeControl = ecs.entity()
        .set(Position{ 0.f, 0.f, 0.f })
        .set(Velocity{ 0.f, 0.f, 0.f })
        .set(FrictionAmount{ 2.0f })
        .set(Gravity{ 0.f, -9.8065f, 0.f })
        .set(BouncePlane{ 0.f, 1.f, 0.f, 0.f })
        .set(Bounciness{ 0.3f })
        .set(GiveGun{ projectile, 3 })
        .set(ReloadTimer{ 3, 2.0f })
        .set(Scripts(
            "../../../Assets/Scripts/control-movement.lua",
            "../../../Assets/Scripts/control-shoot.lua"))
        .add<CubeMesh>();

    auto cubeEnemy = ecs.prefab()
        .set_override(Position{ 0.f, 0.f, 0.f })
        .set_override(Velocity{ 0.f, 0.f, 0.f })
        .set(BouncePlane{ 0.f, 1.f, 0.f, -5.0f })
        .set(Bounciness{ 1.0f })
        .set(Hitbox{ 0.75f, 0.75f, 0.75f })
        .set(AddToMagazine{ 3 })
        .set(Gravity{ 0.f, -9.8065f, 0.f })
        .add<Target>();

    int enemyCount = 5;
    for (int i = 0; i < enemyCount; i++)
    {
        ecs.entity()
            .is_a(cubeEnemy)
            .set(Position{ 5.0f, 0.0f, (-enemyCount / 2 + i) * 2.0f})
            .add<CubeMesh>()
            .set(Respawnable{ cubeEnemy });
    }
}

void EntitySystem::Update()
{
    static auto creatableQuery = ecs.query<Creatable>();
    ecs.progress();
    int creatableCount = 0;
    creatableQuery.each([&](Creatable&) { creatableCount += 1; });
    for (int i = creatableCount; i < creatableMaxCount; i++)
        ecs.entity().add<Creatable>();
}
