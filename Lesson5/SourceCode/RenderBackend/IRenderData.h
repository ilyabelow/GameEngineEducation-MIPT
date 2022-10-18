#pragma once

#include "Common.h"

class RENDERBACKEND_API IRenderData
{
public:
	virtual void GetPosition(float* result) = 0;
	virtual void SetPosition(float position[3]) = 0;
	virtual float GetScale() const = 0;
	virtual void SetScale(float val) = 0;
};