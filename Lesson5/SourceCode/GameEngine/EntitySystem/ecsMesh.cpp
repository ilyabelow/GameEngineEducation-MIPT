#include "ecsMesh.h"
#include "ecsSystems.h"
#include "ecsPhys.h"
#include "flecs.h"
#include "RenderThread.h"
#include "RenderEngine.h"
#include "ObjectRenderProxy.h"

void register_ecs_mesh_systems(flecs::world& ecs)
{
  static auto renderCubesQuery = ecs.query<RenderEnginePtr, CubeMeshes>();
  static auto renderIcosahedraQuery = ecs.query<RenderEnginePtr, IcosahedraMeshes>();
  ecs.system<const CubeMesh>()
    .each([&](flecs::entity e, const CubeMesh& cubeMesh)
      {
        renderCubesQuery.each([&](RenderEnginePtr re, CubeMeshes& meshes)
          {
            RenderProxy* renderProxy = nullptr;
            for (auto mesh : meshes.meshes) {
              if (!mesh->ShouldBeRendered()) {
                renderProxy = mesh;
                break;
              }
            }
            if (!renderProxy) {
              renderProxy = new ObjectRenderProxy;
              re.ptr->GetRT()->EnqueueCommand(RC_CreateCubeRenderObject, renderProxy);
              meshes.meshes.push_back(renderProxy);
            }


            float position[3];
            position[0] = 0.0f;
            position[1] = 0.0f;
            position[2] = 0.0f;

            renderProxy->SetPosition(position);

            renderProxy->SetRenderState(true);
            e.set(RenderProxyPtr{ renderProxy });
            e.remove<CubeMesh>();
          });
      });

  ecs.system<const IcosahedronMesh>()
    .each([&](flecs::entity e, const IcosahedronMesh& cubeMesh)
      {
        renderIcosahedraQuery.each([&](RenderEnginePtr re, IcosahedraMeshes& meshes)
          {
            RenderProxy* renderProxy = nullptr;
            for (auto mesh : meshes.meshes) {
              if (!mesh->ShouldBeRendered()) {
                renderProxy = mesh;
                break;
              }
            }
            if (!renderProxy) {
              renderProxy = new ObjectRenderProxy;
              re.ptr->GetRT()->EnqueueCommand(RC_CreateIcosahedronRenderObject, renderProxy);
              meshes.meshes.push_back(renderProxy);
            }

            float position[3];
            position[0] = 0.0f;
            position[1] = 0.0f;
            position[2] = 0.0f;

            renderProxy->SetPosition(position);

            renderProxy->SetRenderState(true);
            e.set(RenderProxyPtr{ renderProxy });
            e.remove<IcosahedronMesh>();
          });
      });

  ecs.system<RenderProxyPtr, const Position>()
    .each([&](RenderProxyPtr& renderProxy, const Position& pos)
      {
        renderCubesQuery.each([&](RenderEnginePtr re, CubeMeshes&)
          {
            float position[3];
            position[0] = pos.x;
            position[1] = pos.y;
            position[2] = pos.z;

            renderProxy.ptr->SetPosition(position);
          });

        renderIcosahedraQuery.each([&](RenderEnginePtr re, IcosahedraMeshes&)
          {
            float position[3];
            position[0] = pos.x;
            position[1] = pos.y;
            position[2] = pos.z;

            renderProxy.ptr->SetPosition(position);
          });
      });
}

