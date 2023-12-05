#include "pch.h"
#include "Renderer.h"
#include "SceneSelector.h"

namespace dae
{
#pragma region Initialization
    Renderer::Renderer(SDL_Window* windowPtr) :
        m_WindowPtr(windowPtr)
    {
        //Initialize
        SDL_GetWindowSize(windowPtr, &m_Width, &m_Height);

        //Initialize DirectX pipeline
        const HRESULT result = InitializeDirectX();
        if (result == S_OK)
        {
            m_IsInitialized = true;
            std::cout << "DirectX is initialized and ready!\n";
        }
        else
        {
            std::cout << "DirectX initialization failed!\n";
        }
    }
    
    HRESULT Renderer::InitializeDirectX()
    {
        return S_FALSE;
    }
#pragma endregion

#pragma region Cleanup
    Renderer::~Renderer()
    {
    }
#pragma endregion

#pragma region Update & Render
    void Renderer::Update(const Timer* timerPtr)
    {
    }

    void Renderer::Render() const
    {
        if (not m_IsInitialized) return;

#if W1
#if TODO_0
        Render_W1_TODO_0();
#endif
#endif
    }
#pragma endregion

#pragma region Week 1
    void Renderer::Render_W1_TODO_0() const
    {
    }
#pragma endregion
}
