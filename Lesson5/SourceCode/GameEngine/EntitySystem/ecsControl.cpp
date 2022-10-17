#include "ecsControl.h"
#include "ecsSystems.h"
#include "ecsPhys.h"
#include "flecs.h"
#include "../InputHandler.h"

#include "ecsMesh.h"
#include "RenderProxy.h"

void register_ecs_control_systems(flecs::world& ecs)
{
  static auto inputQuery = ecs.query<InputHandlerPtr>();
  static auto toCreateQuery = ecs.query<Created>();
  static bool shootKeyPressed = false;

  ecs.system<Velocity, const Speed, const Controllable>()
    .each([&](flecs::entity e, Velocity& vel, const Speed& spd, const Controllable&)
      {
        inputQuery.each([&](InputHandlerPtr input)
          {
            float deltaVelX = 0.f;
            float deltaVelZ = 0.f;

            if (input.ptr->GetInputState().test(eIC_GoLeft))
              deltaVelX -= spd;
            if (input.ptr->GetInputState().test(eIC_GoRight))
              deltaVelX += spd;

            if (input.ptr->GetInputState().test(eIC_GoForward))
              deltaVelZ += spd;
            if (input.ptr->GetInputState().test(eIC_GoBackward))
              deltaVelZ -= spd;

            float velNormSquared = deltaVelX * deltaVelX + deltaVelZ * deltaVelZ;
            if (velNormSquared > spd * spd) {
              deltaVelX *= 0.707f;
              deltaVelZ *= 0.707f;
            }

            vel.x += deltaVelX * e.delta_time();
            vel.z += deltaVelZ * e.delta_time();
          });
      });

  ecs.system<const Position, Velocity, const Controllable, const BouncePlane, const JumpSpeed>()
    .each([&](const Position& pos, Velocity& vel, const Controllable&, const BouncePlane& plane, const JumpSpeed& jump)
      {
        inputQuery.each([&](InputHandlerPtr input)
          {
            constexpr float planeEpsilon = 0.1f;
            if (plane.x * pos.x + plane.y * pos.y + plane.z * pos.z < plane.w + planeEpsilon)
              if (input.ptr->GetInputState().test(eIC_Jump))
                vel.y = jump.val;
          });
      });

  ecs.system<const Position, const Velocity*, GiveGun>()
    .each([&](flecs::entity e, const Position& pos, const Velocity* vel, GiveGun& gun) {
    inputQuery.each([&](InputHandlerPtr input) {
      if (input.ptr->GetInputState().test(eIC_Shoot) && gun.numberOfBullets > 0) {
        if (!shootKeyPressed) {
          int bulletsToShoot = 1;
          int bulletsShot = 0;
          toCreateQuery.each([&](flecs::entity new_e, Created&) {
            if (bulletsShot++ < bulletsToShoot) {
              float newVelocity[] = { 0.f, 0.f, 30.f };
              if (vel) {
                newVelocity[0] += vel->x;
                newVelocity[1] += vel->y;
                newVelocity[2] += vel->z;
              }
              new_e.mut(e)
                .is_a(gun.bullet)
                .add<IcosahedronMesh>()
                .set(Position{ pos.x, pos.y, pos.z + 1.f })
                .set(Velocity{ newVelocity[0], newVelocity[1], newVelocity[2] })
                .remove<Created>();
            }
            });
          shootKeyPressed = true;
          gun.numberOfBullets--;
        }
      }
      else
        shootKeyPressed = false;
      });
      });

  ecs.system<GiveGun, ReloadTimer>()
    .each([&](flecs::entity e, GiveGun& gun, ReloadTimer& timer) {
    if (gun.numberOfBullets == 0)
      timer.timeElapsed += ecs.delta_time();

    if (timer.timeElapsed > timer.time) {
      gun.numberOfBullets += timer.numberOfBulletsToAdd;
      timer.timeElapsed = 0;
    }
      });
}

