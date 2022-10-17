#pragma once

#include "../RenderObject.h"

namespace
{
	struct PosColorVertex
	{
		float m_x;
		float m_y;
		float m_z;
		uint32_t m_abgr;
	};
};

class Cube : public RenderObject
{
public:
	Cube();

private:
	// This is a regular octahedron
	inline static const PosColorVertex s_cubeVertices[] =
	{
		{ 1.0f,  1.0f,  0.0f,  0xff0000ff },
		{-1.0f,  1.0f,  0.0f,  0xff00ff00 },
		{-1.0f, -1.0f,  0.0f,  0xff00ffff },
		{ 1.0f, -1.0f,  0.0f,  0xffff0000 },
		{ 0.0f,  0.0f,  1.41f, 0xffffffff },
		{ 0.0f,  0.0f, -1.41f, 0xff000000 },
	};

	inline static const uint16_t s_cubeTriList[] =
	{
		0, 4, 1,
		1, 4, 2,
		2, 4, 3,
		3, 4, 0,
		0, 1, 5,
		1, 2, 5,
		2, 3, 5, 
		3, 0, 5
	};
};

