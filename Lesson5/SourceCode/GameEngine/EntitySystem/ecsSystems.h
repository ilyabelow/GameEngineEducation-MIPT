#pragma once

struct InputHandlerPtr
{
  class InputHandler* ptr;
};

struct RenderEnginePtr
{
  class RenderEngine* ptr;
};

struct ScriptSystemPtr
{
  class IScriptSystem* ptr;
};

struct Creatable {};
