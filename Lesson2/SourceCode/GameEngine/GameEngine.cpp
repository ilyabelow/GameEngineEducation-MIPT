// GameEngine.cpp : Defines the entry point for the application.
//

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "GameEngine.h"
#include "RenderEngine.h"
#include "RenderThread.h"
#include "CubeGameObject.h"
#include "GameTimer.h"

#include "inih/INIReader.h"
#include <unordered_map>

#define VK_W    0x57
#define VK_A    0x41
#define VK_S    0x53
#define VK_D    0x44

bool isKeyPressed(unsigned int key) {
    return GetKeyState(key) & 0x8000;
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

    std::unordered_map<std::string, unsigned int> inputMap;
    inputMap["W"] = VK_W;
    inputMap["A"] = VK_A;
    inputMap["S"] = VK_S;
    inputMap["D"] = VK_D;
    inputMap["LEFT ARROW"]  =  VK_LEFT;
    inputMap["RIGHT ARROW"] = VK_RIGHT;
    inputMap["UP ARROW"]    =    VK_UP;
    inputMap["DOWN ARROW"]  =  VK_DOWN;

    std::unordered_map<std::string, unsigned int> controls;

    INIReader reader("actionmap.ini");
    std::set<std::string> fields = reader.GetFields("Keyboard");

    for (const std::string& field : fields) {
        std::string command = field;
        std::string symbol = reader.Get("Keyboard", field, "");

        for (char& letter : symbol)
            letter = std::toupper(letter);

        controls[command] = inputMap[symbol];
    }

    GameTimer timer;

    RenderEngine* renderEngine = new RenderEngine(hInstance);
    RenderThread* renderThread = renderEngine->GetRT();

    GameObject* cube = new CubeGameObject();
    renderThread->EnqueueCommand(RC_CreateCubeRenderObject, cube->GetRenderProxy());

    MSG msg = { 0 };

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
            float t = 0;
            timer.Tick();

            float moveX = 0.f, moveY = 0.f, moveZ = 0.f;

            if (isKeyPressed(controls["Move Up"])) {
                moveZ += 3.f * timer.DeltaTime();
            }
            if (isKeyPressed(controls["Move Down"])) {
                moveZ -= 3.f * timer.DeltaTime();
            }

            if (isKeyPressed(controls["Move Right"])) {
                moveX += 3.f * timer.DeltaTime();
            }
            if (isKeyPressed(controls["Move Left"])) {
                moveX -= 3.f * timer.DeltaTime();
            }


            cube->Move(moveX, moveY, moveZ);

            renderThread->OnEndFrame();
        }
    }

    return (int) msg.wParam;
}
