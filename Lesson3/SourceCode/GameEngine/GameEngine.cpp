// GameEngine.cpp : Defines the entry point for the application.
//

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <random>

#include "GameEngine.h"
#include "RenderEngine.h"
#include "RenderThread.h"
#include "CubeGameObject.h"
#include "GameTimer.h"
#include "InputHandler.h"

#define CONTROLLABLE 1
#define JUMPING      2
#define OSCILLATING  3

const float startVelocity = 8.f;

std::random_device dev;
std::mt19937 generator(dev());
std::uniform_int_distribution<int> uniform(1, 3);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // Enable run-time memory check for debug builds.
#if defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    GameTimer timer;

    RenderEngine* renderEngine = new RenderEngine(hInstance);
    RenderThread* renderThread = renderEngine->GetRT();
    InputHandler* inputHandler = new InputHandler();

    int cubeTypes[100];
    GameObject* cubes[100];
    float startingCoordinates[100][2];

    for (size_t i = 0; i < 100; ++i) {
        cubeTypes[i] = uniform(generator);
        startingCoordinates[i][0] = -22.5f + 5 * (i % 10);
        startingCoordinates[i][1] = -22.5f + 5 * (i / 10);
        cubes[i] = new CubeGameObject();
        renderThread->EnqueueCommand(RC_CreateCubeRenderObject, cubes[i]->GetRenderProxy());
    }

    MSG msg = { 0 };

    timer.Start();
    timer.Reset();

    float controllableCubeNewPositionX = 0.0f;
    float jumpingCubeNewPositionY = 0.0f;
    float velocityJumpingCube = startVelocity;

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
            inputHandler->Update();

            float t = 0;
            timer.Tick();
            t = sin(timer.TotalTime() * 4.f);

            float velocityControllableCube = 0.0f;
            if (inputHandler->GetInputState().test(eIC_GoLeft))
                velocityControllableCube -= 3.0f;
            if (inputHandler->GetInputState().test(eIC_GoRight))
                velocityControllableCube += 3.0f;
            controllableCubeNewPositionX += velocityControllableCube * timer.DeltaTime();

            velocityJumpingCube -= 9.8f * timer.DeltaTime();
            jumpingCubeNewPositionY += velocityJumpingCube * timer.DeltaTime();
            if (jumpingCubeNewPositionY < 0.f) {
                jumpingCubeNewPositionY = 0.f;
                velocityJumpingCube = startVelocity;
            }

            for (size_t i = 0; i < 100; ++i) {
                switch (cubeTypes[i]) {
                case CONTROLLABLE:
                    cubes[i]->SetPosition(startingCoordinates[i][0] + controllableCubeNewPositionX, 0.0f, startingCoordinates[i][1]);
                    break;
                case JUMPING:
                    cubes[i]->SetPosition(startingCoordinates[i][0], jumpingCubeNewPositionY, startingCoordinates[i][1]);
                    break;
                case OSCILLATING:
                    cubes[i]->SetPosition(startingCoordinates[i][0], 0, startingCoordinates[i][1] + t);
                    break;
                default:
                    break;
                }
            }

            renderThread->OnEndFrame();
        }
    }

    return (int) msg.wParam;
}
