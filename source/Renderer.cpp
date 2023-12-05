#include "pch.h"
#include "Renderer.h"
#include "SceneSelector.h"

// DirectX headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

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
        // 1. Create device and device context
        //=======================================================================================================
        D3D_FEATURE_LEVEL featureLevel      = D3D_FEATURE_LEVEL_11_1;
        uint32_t          createDeviceFlags = 0;
        
#if defined(DEBUG) or defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        HRESULT result = D3D11CreateDevice(
            nullptr,                         // IDXGIAdapter* pAdapter
            D3D_DRIVER_TYPE_HARDWARE,        // D3D_DRIVER_TYPE DriverType
            nullptr,                         // HMODULE Software
            createDeviceFlags,               // UINT Flags
            &featureLevel,                   // const D3D_FEATURE_LEVEL* pFeatureLevels
            1,                               // UINT FeatureLevels
            D3D11_SDK_VERSION,               // UINT SDKVersion
            &m_DevicePtr,                    // ID3D11Device** ppDevice
            nullptr,                         // D3D_FEATURE_LEVEL* pFeatureLevel
            &m_DeviceContextPtr              // ID3D11DeviceContext** ppImmediateContext
            );

        if (FAILED(result))
            return result;

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
