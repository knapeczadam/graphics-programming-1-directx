#include "pch.h"
#include "Renderer.h"

namespace dae
{
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

    Renderer::~Renderer()
    {
    }

    void Renderer::Update(const Timer* timerPtr)
    {
    }


    void Renderer::Render() const
    {
        if (!m_IsInitialized)
            return;
    }

    HRESULT Renderer::InitializeDirectX()
    {
        return S_FALSE;
    }
}
