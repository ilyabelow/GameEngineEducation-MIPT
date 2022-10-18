#include "ecsScript.h"
#include "ecsSystems.h"
#include "ecsPhys.h"
#include "ecsControl.h"
#include "ecsMesh.h"
#include "../ScriptSystem/ScriptSystem.h"
#include "../ScriptSystem/ScriptProxy.h"
#include "../GameEngine/Input.h"
#include "../GameEngine/InputHandler.h"


void register_ecs_script_systems(flecs::world& ecs)
{
  static auto scriptSystemQuery = ecs.query<ScriptSystemPtr>();
  static auto inputHandlerQuery = ecs.query<InputHandlerPtr>();
  ecs.system<Scripts>()
    .each([&](flecs::entity e, Scripts& scripts)
    {
      inputHandlerQuery.each([&](InputHandlerPtr& inputHandler){
      scriptSystemQuery.each([&](ScriptSystemPtr& scriptSystem)
      {
        for (const auto& script : scripts.names)
        {
          auto scriptProxy = scriptSystem.ptr->CreateProxy(script.c_str());
          init_script_state(ecs, scriptProxy);
          scriptProxy->BindValue("input_handler", std::ref(*inputHandler.ptr));
          scriptProxy->BindValue("entity_id", e.id());
        }
      });});
      e.remove<Scripts>();
    });
}

void init_script_state(flecs::world& ecs, std::shared_ptr<CScriptProxy> scriptProxy)
{
    sol::state& state = scriptProxy->GetState();
    state.new_usertype<Position>("Position",
        "x", &Position::x,
        "y", &Position::y,
        "z", &Position::z);
    state.new_usertype<Velocity>("Velocity",
        "x", &Velocity::x,
        "y", &Velocity::y,
        "z", &Velocity::z);
    state.new_usertype<BouncePlane>("BouncePlane",
        "w", &BouncePlane::w,
        "x", &BouncePlane::x,
        "y", &BouncePlane::y,
        "z", &BouncePlane::z);
    state.new_usertype<std::bitset<eIC_Max>>("Bitset",
        "test", &std::bitset<eIC_Max>::test);
    state.new_usertype<InputHandler>("InputHandler",
        "getInputState", &InputHandler::GetInputState);
    state.new_usertype<ReloadTimer>("ReloadTimer",
        "mag", &ReloadTimer::numberOfBulletsToAdd,
        "time", &ReloadTimer::time,
        "acc", &ReloadTimer::timeElapsed);
    state.new_usertype<GiveGun>("Shootable",
        "projectile", &GiveGun::bullet,
        "mag", &GiveGun::numberOfBullets,
        "shoot_pressed", &GiveGun::shootKeyPressed);
    state.new_usertype<flecs::world>("World",
        "createEntity", [](flecs::world& world) {return world.entity(); },
        "getEntity", [](flecs::world& world, flecs::entity_t id) { return world.entity(id); });
    state.new_usertype<flecs::entity>("Entity",
        "getVelocity", [](flecs::entity e) { return e.get<Velocity>(); },
        "setVelocity", [](flecs::entity e, float x, float y, float z) { e.set(Velocity{ x, y, z }); },
        "getBouncePlane", [](flecs::entity e) { return e.get<BouncePlane>(); },
        "getPosition", [](flecs::entity e) { return e.get<Position>(); },
        "setPosition", [](flecs::entity e, float x, float y, float z) { e.set(Position{ x, y, z }); },
        "addCubeMesh", [](flecs::entity e) { e.add<CubeMesh>(); },
        "setParent", [](flecs::entity e, flecs::entity parent) { e.is_a(parent); },
        "getShoot", [](flecs::entity e) { return e.get<GiveGun>(); },
        "getReloadTimer", [](flecs::entity e) { return e.get<ReloadTimer>(); });
    state["eIC_GoLeft"] = eIC_GoLeft;
    state["eIC_GoRight"] = eIC_GoRight;
    state["eIC_GoForward"] = eIC_GoForward;
    state["eIC_GoBackward"] = eIC_GoBackward;
    state["eIC_Jump"] = eIC_Jump;
    state["eIC_Shoot"] = eIC_Shoot;
    state["world"] = std::ref(ecs);
}
