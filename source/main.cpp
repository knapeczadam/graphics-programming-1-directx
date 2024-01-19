#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Renderer.h"

// ImGui includes
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_sdl2.h"

using namespace dae;

void ShutDown(SDL_Window* windowPtr)
{
    SDL_DestroyWindow(windowPtr);
    SDL_Quit();
}

void StartImGuiFrame()
{
    // Start the Dear ImGui frame
    // Needs to be branched because of this:
    // Assertion failed: (g.FrameCount == 0 || g.FrameCountEnded == g.FrameCount) && "Forgot to call Render() or EndFrame() at the end of the previous frame?",
#if W3
#if TODO_0
        // Assertion failed: (g.IO.DeltaTime > 0.0f || g.FrameCount == 0) && "Need a positive DeltaTime!"
        // Fixed imgui.cpp:9426
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
#endif
#endif
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
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends - Part 1
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(windowPtr, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;
    ImGui_ImplWin32_Init(hwnd);
    // ImGui_ImplDX11_Init is called in Renderer constructor

    //Initialize "framework"
    const auto timerPtr    = new Timer();
    const auto rendererPtr = new Renderer(windowPtr, timerPtr);

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
            ImGui_ImplSDL2_ProcessEvent(&e);
            
            switch (e.type)
            {
            case SDL_QUIT:
                isLooping = false;
                break;
            case SDL_KEYUP:
                switch (e.key.keysym.scancode)
                {
                case SDL_SCANCODE_F1:
                    rendererPtr->ToggleFrontCounterClockwise();
                    break;
                case SDL_SCANCODE_F2:
                    rendererPtr->ToggleAlphaBlending();
                    break;
                case SDL_SCANCODE_F3:
                    rendererPtr->CycleShadingMode();
                    break;
                case SDL_SCANCODE_F4:
                    rendererPtr->CycleSamplerState();
                    break;
                case SDL_SCANCODE_F5:
                    rendererPtr->ToggleRotation();
                    break;
                case SDL_SCANCODE_F6:
                    rendererPtr->ToggleNormalVisibility();
                    break;
                case SDL_SCANCODE_F7:
                    rendererPtr->ToggleFireFX(); 
                    break;
                case SDL_SCANCODE_F8:
                    rendererPtr->ToggleUniformClearColor(); 
                    break;
                case SDL_SCANCODE_F9:
                    rendererPtr->ToggleFPSCounter(); 
                    break;
                case SDL_SCANCODE_F10:
                    rendererPtr->CycleFillMode(); 
                    break;
                case SDL_SCANCODE_F11:
                    rendererPtr->CycleCullMode(); 
                    break;
                case SDL_SCANCODE_F12:
                    rendererPtr->TakeScreenshot(); 
                    break;
                }
                break;
            case SDL_MOUSEWHEEL:
                rendererPtr->GetCamera().Scroll(e.wheel);
                break;
            }
        }
        
        StartImGuiFrame();

        //--------- Update ---------
        rendererPtr->Update(timerPtr);

        //--------- Render ---------
        rendererPtr->Render();

        //--------- Timer ---------
        timerPtr->Update();

        printTimer += timerPtr->GetElapsed();
        if (rendererPtr->UseFPSCounter() and printTimer >= 1.f)
        {
            printTimer = 0.f;
            std::cout << "dFPS: " << timerPtr->GetdFPS() << std::endl;
        }
    }
    timerPtr->Stop();
    
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    //Shutdown "framework"
    delete rendererPtr;
    delete timerPtr;

    ShutDown(windowPtr);
    return 0;
}
