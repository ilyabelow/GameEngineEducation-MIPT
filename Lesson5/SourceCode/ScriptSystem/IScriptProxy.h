#pragma once
#include "Common.h"

struct Velocity;
class InputHandler;

class SCRIPTSYSTEM_API IScriptProxy
{
public:
    virtual void Execute() = 0;
};