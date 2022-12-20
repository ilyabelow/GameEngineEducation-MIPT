// GameEngine.cpp : Defines the entry point for the application.
//

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "GameEngine.h"
#include "RenderEngine.h"
#include "RenderThread.h"
#include "GameTimer.h"
#include "InputHandler.h"
#include "EntitySystem/EntitySystem.h"
#include "../ScriptSystem/ScriptSystem.h"
#include "NetworkManager.h"

#include <format>
#include <sstream>
#include <iostream>
#include <memory>

#define Log(fmt, ...) \
{ \
	std::stringstream ss; \
	ss << std::format("{}({},{}): ", __FILE__, __LINE__, 0); \
	ss << std::format(fmt, __VA_ARGS__); \
	ss << '\n'; \
	std::cout << ss.str() << std::endl; \
	OutputDebugStringA(ss.str().c_str()); \
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // Enable run-time memory check for debug builds.
#if defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


    MSG msg = { 0 };

    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);

    GameTimer timer;

    RenderEngine* renderEngine = new RenderEngine(hInstance);
    RenderThread* renderThread = renderEngine->GetRT();
    InputHandler* inputHandler = new InputHandler();
    CScriptSystem* scriptSystem = new CScriptSystem();

    EntitySystem* entitySystem = new EntitySystem(renderEngine, inputHandler, scriptSystem);
    auto networkManager = std::make_unique<NetworkManager>(inputHandler, entitySystem);


    timer.Start();
    timer.Reset();


    // Main message loop:
    while (msg.message != (WM_QUIT | WM_CLOSE))
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            networkManager->OnFrameBegin();

            inputHandler->Update();
            scriptSystem->Update(timer.DeltaTime());
            entitySystem->Update();  

            networkManager->OnFrameEnd();

            timer.Tick();

            renderThread->OnEndFrame();
            auto pos = inputHandler->GetMouseCoordinates();
            //Log("x = {}, y = {}", pos.x, pos.y);
        }
    }

    return (int) msg.wParam;
}
