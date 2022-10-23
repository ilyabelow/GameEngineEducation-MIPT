#pragma once

#include "Common.h"

class RENDERENGINE_API RenderProxy
{
public:
	RenderProxy() = default;

	void GetPosition(float* result)
	{
		result[0] = m_vPosition[0];
		result[1] = m_vPosition[1];
		result[2] = m_vPosition[2];
	}
	void SetPosition(float position[3]);

	bool ShouldBeRendered() const;
	void SetRenderState(bool state);

	float m_vPosition[3];
	bool m_renderState;
};

