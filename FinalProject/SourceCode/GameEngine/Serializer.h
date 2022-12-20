#pragma once

#include "EntitySystem/ecsPhys.h"



namespace Serializable {
	enum class Type {
		Player,
		Enemy,
		Bullet
	};

	struct Player {
		constexpr static Type type = Type::Player;

		Position pos;
		Velocity vel;

		static Player Serialize(flecs::entity e) {
			Player p;
			p.pos = *e.get<Position>();
			p.vel = *e.get<Velocity>();
			return p;
		}

		void Apply(flecs::entity& e) {
			e.set<Position>(pos);
			e.set< Velocity >(vel);
		}
	};

	struct Enemy {
		constexpr static Type type = Type::Enemy;

		Position pos;
		Velocity vel;

		static Enemy Serialize(flecs::entity e) {
			Enemy p;
			p.pos = *e.get<Position>();
			p.vel = *e.get<Velocity>();

			return p;
		}

		void Apply(flecs::entity& e) {
			e.set<Position>(pos);
			e.set<Velocity>(vel);
		}
	};

	struct Bullet {
		constexpr static Type type = Type::Bullet;

		Position pos;
		Velocity vel;

		static Bullet Serialize(flecs::entity e) {
			Bullet p;
			p.pos = *e.get<Position>();
			p.vel = *e.get<Velocity>();
			return p;
		}

		void Apply(flecs::entity& e) {
			e.set<Position>(pos);
			e.set<Velocity>(vel);
		}
	};
};

#pragma once
