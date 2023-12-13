#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Renderer.h"

using namespace dae;

void ShutDown(SDL_Window* windowPtr)
{
    SDL_DestroyWindow(windowPtr);
    SDL_Quit();
}

int main(int argc, char* args[])
{
    //Unreferenced parameters
    (void)argc;
    (void)args;

    //Create window + surfaces
    SDL_Init(SDL_INIT_VIDEO);

    constexpr uint32_t width  = 640;
    constexpr uint32_t height = 480;

    SDL_Window* windowPtr = SDL_CreateWindow(
        "DirectX - **Ádám Knapecz (2DAE09)**",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height, 0);

    if (not windowPtr)
        return 1;

    //Initialize "framework"
    const auto timerPtr    = new Timer();
    const auto rendererPtr = new Renderer(windowPtr);

    //Start loop
    timerPtr->Start();
    
    float printTimer = 0.f;
    bool  isLooping  = true;
    
    while (isLooping)
    {
        //--------- Get input events ---------
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_QUIT:
                isLooping = false;
                break;
            case SDL_KEYUP:
                switch (e.key.keysym.scancode)
                {
                case SDL_SCANCODE_F2:
                    rendererPtr->CycleSamplerState();
                    break;
                case SDL_SCANCODE_F5:
                    rendererPtr->ToggleRotation();
                    break;
                case SDL_SCANCODE_F6:
                    rendererPtr->ToggleNormalVisibility();
                    break;
                case SDL_SCANCODE_F7:
                    rendererPtr->CycleShadingMode();
                    break;
                }
                break;
            case SDL_MOUSEWHEEL:
                rendererPtr->GetCamera().Scroll(e.wheel);
                break;
            }
        }

        //--------- Update ---------
        rendererPtr->Update(timerPtr);

        //--------- Render ---------
        rendererPtr->Render();

        //--------- Timer ---------
        timerPtr->Update();

        printTimer += timerPtr->GetElapsed();
        if (printTimer >= 1.f)
        {
            printTimer = 0.f;
            std::cout << "dFPS: " << timerPtr->GetdFPS() << std::endl;
        }
    }
    timerPtr->Stop();

    //Shutdown "framework"
    delete rendererPtr;
    delete timerPtr;

    ShutDown(windowPtr);
    return 0;
}
