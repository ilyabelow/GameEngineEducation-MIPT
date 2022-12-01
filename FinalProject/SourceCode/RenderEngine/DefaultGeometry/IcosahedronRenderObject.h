#pragma once

#include "../RenderObject.h"

class RENDERENGINE_API IcosahedronRenderObject final : public RenderObject
{
public:
	IcosahedronRenderObject(RenderProxy * pRenderProxy);

	virtual void* GetVertices() const override { return (void*)s_icosahedronVertices; }
	virtual UINT32 GetVerticesSize() const override { return sizeof(s_icosahedronVertices); }
	virtual void* GetIndices() const override { return (void*)s_icosahedronTriList; }
	virtual UINT32 GetIndicesSize() const override { return sizeof(s_icosahedronTriList); }
	virtual std::string GetVsShaderName() const override { return "vs_cubes"; }
	virtual std::string GetPsShaderName() const override { return "fs_cubes"; }

private:
	inline static const PosColorVertex s_icosahedronVertices[] =
	{
		{ 0.000f / 4.f,  0.000f / 4.f,  1.000f / 4.f, 0x00ffff80 },
		{ 0.000f / 4.f,  0.894f / 4.f,  0.447f / 4.f, 0x00ffffff },
		{ 0.851f / 4.f,  0.276f / 4.f,  0.447f / 4.f, 0x00ffffa7 },
		{ 0.526f / 4.f, -0.724f / 4.f,  0.447f / 4.f, 0x00ffff18 },
		{-0.526f / 4.f, -0.724f / 4.f,  0.447f / 4.f, 0x00ffff18 },
		{-0.851f / 4.f,  0.276f / 4.f,  0.447f / 4.f, 0x00ffffa7 },
		{ 0.000f / 4.f,  0.000f / 4.f, -1.000f / 4.f, 0x00ffff80 },
		{ 0.000f / 4.f, -0.894f / 4.f, -0.447f / 4.f, 0x00ffff00 },
		{-0.851f / 4.f, -0.276f / 4.f, -0.447f / 4.f, 0x00ffff58 },
		{-0.526f / 4.f,  0.724f / 4.f, -0.447f / 4.f, 0x00ffffe7 },
		{ 0.526f / 4.f,  0.724f / 4.f, -0.447f / 4.f, 0x00ffffe7 },
		{ 0.851f / 4.f, -0.276f / 4.f, -0.447f / 4.f, 0x00ffff58 },
	};

	inline static const uint16_t s_icosahedronTriList[] =
	{
		 0,  1,  2, // 1st layer
		 0,  2,  3,
		 0,  3,  4,
		 0,  4,  5,
		 0,  5,  1,
		 1, 10,  2, // 2nd layer
		 2, 11,  3,
		 3,  7,  4,
		 4,  8,  5,
		 5,  9,  1,
		10, 11,  2, // 3rd layer
		11,  7,  3,
		 7,  8,  4,
		 8,  9,  5,
		 9, 10,  1,
		 7,  6,  8, // 4th layer
		 8,  6,  9,
		 9,  6, 10,
		10,  6, 11,
		11,  6,  7,
	};
};

