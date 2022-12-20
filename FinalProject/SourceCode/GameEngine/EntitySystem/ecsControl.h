#pragma once
#include "flecs.h"
#include <sstream>
#include "../InputHandler.h"
#include "../Serializer.h"

struct Controllable {};
struct Bullet {};
struct Target {};
;
struct JumpSpeed { float val; };
struct GiveGun { flecs::entity bullet; int numberOfBullets; bool shootKeyPressed = false; };

struct AddToMagazine { int val; };

struct ReloadTimer {
	int numberOfBulletsToAdd; float time; float timeElapsed = 0;

	//ReloadTimer(std::string string) {
 //   //std::stringstream ss(string);
 //   //ss >> numberOfBulletsToAdd;
 //   //ss.ignore();
 //   //ss >> time;
 //   numberOfBulletsToAdd = 6;
 //   time = 2.f;
	//}
};
struct Respawnable { flecs::entity e; };
struct RespawnTimer { flecs::entity e; float time; float timeElapsed = 0;};
struct InputStatePtr { const InputState* inputState; };
struct Destruct {};
struct Remote {};
struct Local {};

struct Sync {
	Serializable::Type type;
	float timeToSync;
	float timeElapsed = 0;
};

void register_ecs_control_systems(flecs::world& ecs);

