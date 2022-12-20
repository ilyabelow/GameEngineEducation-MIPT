#include "ecsPhys.h"
#include <stdlib.h>

#include "ecsMesh.h"
#include "ecsControl.h"
#include "ecsSystems.h"
#include "ObjectRenderProxy.h"
#include <iostream>

static float rand_flt(float from, float to)
{
  return from + (float(rand()) / RAND_MAX) * (to - from);
}

static bool HitboxesIntersect(const Position& pos, const Hitbox& hitbox, const Position& other_pos, const Hitbox& other_hitbox) {
  return
    (abs(pos.x - other_pos.x) < (hitbox.x + other_hitbox.x)) &&
    (abs(pos.y - other_pos.y) < (hitbox.y + other_hitbox.y)) &&
    (abs(pos.z - other_pos.z) < (hitbox.z + other_hitbox.z));
}

void register_ecs_phys_systems(flecs::world& ecs)
{  
  static auto hitboxQuery = ecs.query<
    const Position, const Hitbox, const Target, const AddToMagazine*, RenderProxyPtr*, Respawnable*
  >();
  static auto giveGunQuery = ecs.query<GiveGun>();
  static auto toCreateQuery = ecs.query<Created>();
  static auto bulletQuery = ecs.query<Bullet>();

  ecs.system<Velocity, const Gravity, BouncePlane*, Position*>()
    .each([&](flecs::entity e, Velocity& vel, const Gravity& grav, BouncePlane* plane, Position* pos)
      {
        if (plane && pos)
        {
          constexpr float planeEpsilon = 0.1f;
          if (plane->x * pos->x + plane->y * pos->y + plane->z * pos->z < plane->w + planeEpsilon)
            return;
        }
        vel.x += grav.x * e.delta_time();
        vel.y += grav.y * e.delta_time();
        vel.z += grav.z * e.delta_time();
      });

  ecs.system<const Position, const BouncePlane, const SelfDestruct, DestroyTimer*>()
    .each([&](flecs::entity e, const Position& pos, const BouncePlane& plane, const SelfDestruct& destroyable, DestroyTimer* timer) {
    float dotPos = plane.x * pos.x + plane.y * pos.y + plane.z * pos.z;
    if (dotPos <= plane.w && !timer)
      e.set(DestroyTimer{ destroyable.countdown });
      });

  ecs.system<Velocity, Position, const BouncePlane, const Bounciness>()
    .each([&](Velocity& vel, Position& pos, const BouncePlane& plane, const Bounciness& bounciness)
      {
        float dotPos = plane.x * pos.x + plane.y * pos.y + plane.z * pos.z;
        float dotVel = plane.x * vel.x + plane.y * vel.y + plane.z * vel.z;
        if (dotPos < plane.w)
        {
          pos.x -= (dotPos - plane.w) * plane.x;
          pos.y -= (dotPos - plane.w) * plane.y;
          pos.z -= (dotPos - plane.w) * plane.z;

          vel.x -= (1.f + bounciness.val) * plane.x * dotVel;
          vel.y -= (1.f + bounciness.val) * plane.y * dotVel;
          vel.z -= (1.f + bounciness.val) * plane.z * dotVel;
        }
      });


  ecs.system<Velocity, const FrictionAmount>()
    .each([&](flecs::entity e, Velocity& vel, const FrictionAmount& friction)
      {
        vel.x -= vel.x * friction.val * e.delta_time();
        vel.y -= vel.y * friction.val * e.delta_time();
        vel.z -= vel.z * friction.val * e.delta_time();
      });


  ecs.system<Position, const Velocity>()
    .each([&](flecs::entity e, Position& pos, const Velocity& vel)
      {
        pos.x += vel.x * e.delta_time();
        pos.y += vel.y * e.delta_time();
        pos.z += vel.z * e.delta_time();
      });


  ecs.system<Position, const ShiverAmount>()
    .each([&](flecs::entity e, Position& pos, const ShiverAmount& shiver)
      {
        pos.x += rand_flt(-shiver.val, shiver.val);
        pos.y += rand_flt(-shiver.val, shiver.val);
        pos.z += rand_flt(-shiver.val, shiver.val);
      });

  ecs.system<const Position, const Hitbox, const Bullet>()
    .each([&](flecs::entity e, const Position& pos, const Hitbox& box, const Bullet& bullet) {
    hitboxQuery.each([&](
      flecs::entity other, const Position& other_pos, const Hitbox& other_box,
      const Target& enemy, const AddToMagazine* magBonus, RenderProxyPtr* other_proxy, Respawnable* respawn
      ) {
        if (e.id() != other.id() && HitboxesIntersect(pos, box, other_pos, other_box)) {
          if (magBonus) {
              giveGunQuery.each(
                  [&](GiveGun& gun)
                  {
                    gun.numberOfBullets += magBonus->val;
                  }
              );
          }


          if (respawn) {
            int targetsToSpawn = 1;
            int targetsSpawned = 0;
            toCreateQuery.each([&](flecs::entity new_e, const Created& created) {
              if (targetsSpawned++ < targetsToSpawn)
                new_e.mut(e)
                .set(Position{ other_pos.x, 0.f, other_pos.z })
                .set(RespawnTimer{ respawn->e, 5.0f })
                .remove<Created>();
              });

            other.remove< Respawnable>();
            
            other.add<Destruct>();
            e.add<Destruct>();
          }
        }
      });

      });

  ecs.system<DestroyTimer>("Bullet destruction on timer")
    .each([&](flecs::entity e, DestroyTimer& timer) {
    timer.timeElapsed += ecs.delta_time();
    if (timer.timeElapsed > timer.time) {
		e.add<Destruct>();
    }
      });

  ecs.system<RespawnTimer>()
    .each([&](flecs::entity e, RespawnTimer& timer) {
    timer.timeElapsed += ecs.delta_time();
    if (timer.timeElapsed > timer.time)
      e.is_a(timer.e)
       .add<Local>()
      .add<CubeMesh>()
      .set(Respawnable{ timer.e })
      .remove<RespawnTimer>();
      });
}

