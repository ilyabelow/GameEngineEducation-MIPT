#pragma once
#include <vector>
#include "flecs.h"

struct CubeMesh {};
struct IcosahedronMesh {};
struct RenderProxyPtr
{
	class RenderProxy* ptr;
};

struct DestroyTimer {
	float time;
	float timeElapsed = 0;
};
struct Created {};

struct CubeMeshes {
	std::vector<RenderProxy*> meshes;
};

struct IcosahedraMeshes {
	std::vector<RenderProxy*> meshes;
};

void register_ecs_mesh_systems(flecs::world &ecs);

