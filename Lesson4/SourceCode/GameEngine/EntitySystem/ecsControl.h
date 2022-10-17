#pragma once
#include "flecs.h"

struct Controllable {};
struct Bullet {};
struct Target {};
;
struct JumpSpeed { float val; };
struct GiveGun { flecs::entity bullet; int numberOfBullets; };

struct AddToMagazine { int val; };
struct ReloadTimer { int numberOfBulletsToAdd; float time; float timeElapsed = 0; };
struct Respawnable { flecs::entity e; };
struct RespawnTimer { flecs::entity e; float time; float timeElapsed = 0; };


void register_ecs_control_systems(flecs::world &ecs);

