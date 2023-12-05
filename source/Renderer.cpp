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

        // Create DXGI factory
        //=======================================================================================================
        IDXGIFactory* dxgiFactoryPtr = nullptr;
        result = CreateDXGIFactory(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactoryPtr));

        if (FAILED(result))
            return result;

        // 2. Create swap chain
        //=======================================================================================================
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        swapChainDesc.BufferDesc.Width                   = m_Width;
        swapChainDesc.BufferDesc.Height                  = m_Height;
        swapChainDesc.BufferDesc.RefreshRate.Numerator   = 1;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
        swapChainDesc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDesc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
        swapChainDesc.SampleDesc.Count                   = 1;
        swapChainDesc.SampleDesc.Quality                 = 0;
        swapChainDesc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount                        = 1;
        swapChainDesc.Windowed                           = TRUE;
        swapChainDesc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.Flags                              = 0;

        // Get handle to window (HWND) from the SDL backbuffer
        //=======================================================================================================
        SDL_SysWMinfo sysWMInfo;
        SDL_GetVersion(&sysWMInfo.version);
        SDL_GetWindowWMInfo(m_WindowPtr, &sysWMInfo);
        swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

        // Create swap chain
        //=======================================================================================================
        result = dxgiFactoryPtr->CreateSwapChain(m_DevicePtr, &swapChainDesc, &m_SwapChainPtr);
        if (FAILED(result))
            return result;

        // 3. Create DepthStencil (DS) and DepthStencilView (DSV)
        //=======================================================================================================
        D3D11_TEXTURE2D_DESC depthStencilDesc;
        depthStencilDesc.Width              = m_Width;
        depthStencilDesc.Height             = m_Height;
        depthStencilDesc.MipLevels          = 1;
        depthStencilDesc.ArraySize          = 1;
        depthStencilDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.SampleDesc.Count   = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.Usage              = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags     = 0;
        depthStencilDesc.MiscFlags          = 0;

        // View
        //=======================================================================================================
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        depthStencilViewDesc.Format             = depthStencilDesc.Format;
        depthStencilViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;

        result = m_DevicePtr->CreateTexture2D(&depthStencilDesc, nullptr, &m_DepthStencilBufferPtr);
        if (FAILED(result))
            return result;

        result = m_DevicePtr->CreateDepthStencilView(m_DepthStencilBufferPtr, &depthStencilViewDesc, &m_DepthStencilViewPtr);
        if (FAILED(result))
            return result;

        // 4. Create RenderTarget (RT) and RenderTargetView (RTV)
        //=======================================================================================================

        // Resource
        result = m_SwapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_RenderTargetBufferPtr));
        if (FAILED(result))
            return result;

        // View
        result = m_DevicePtr->CreateRenderTargetView(m_RenderTargetBufferPtr, nullptr, &m_RenderTargetViewPtr);
        if (FAILED(result))
            return result;

        // 5. Bind RTV and DSV to Output Merger Stage
        //=======================================================================================================
        m_DeviceContextPtr->OMSetRenderTargets(1, &m_RenderTargetViewPtr, m_DepthStencilViewPtr);

        
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
