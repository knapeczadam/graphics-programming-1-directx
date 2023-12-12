#include "pch.h"

// Project includes
#include "Renderer.h"
#include "SceneSelector.h"
#include "Mesh.h"

// DirectX headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

// Standard includes
#include <cassert>

namespace dae
{
#pragma region Global Variables
    std::vector<Vertex> vertices
    {
        {{ 0.0f,  0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}
    };
    
    const std::vector<Vertex> vertices_world
    {
        {{ 0.0f,  3.0f, 2.0f},{1.0f, 0.0f, 0.0f}},
        {{ 3.0f, -3.0f, 2.0f},{0.0f, 0.0f, 1.0f}},
        {{-3.0f, -3.0f, 2.0f},{0.0f, 1.0f, 0.0f}}
    };
    
    std::vector<uint32_t>   indices{0, 1, 2};
    std::vector<Vertex_Out> vertices_out;
#pragma endregion
    
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

        
        // General initialization
        //=======================================================================================================
        InitializeMesh();
        InitializeCamera();
        InitializeOutputVertices();
        InitializeTextures();
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
        result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&m_DXGIFactoryPtr));

        if (FAILED(result))
            return result;

        // 2. Create swap chain
        //=======================================================================================================
        DXGI_SWAP_CHAIN_DESC swapChainDesc{};
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
        SDL_SysWMinfo sysWMInfo{};
        SDL_GetVersion(&sysWMInfo.version);
        SDL_GetWindowWMInfo(m_WindowPtr, &sysWMInfo);
        swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

        // Create swap chain
        //=======================================================================================================
        result = m_DXGIFactoryPtr->CreateSwapChain(m_DevicePtr, &swapChainDesc, &m_SwapChainPtr);
        if (FAILED(result))
            return result;

        // 3. Create DepthStencil (DS) and DepthStencilView (DSV)
        //=======================================================================================================
        D3D11_TEXTURE2D_DESC depthStencilDesc{};
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
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
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

        // 6. Set viewport
        //=======================================================================================================
        D3D11_VIEWPORT viewport;
        viewport.Width    = static_cast<float>(m_Width);
        viewport.Height   = static_cast<float>(m_Height);
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        m_DeviceContextPtr->RSSetViewports(1, &viewport);

        return S_OK;
    }
    
    void Renderer::InitializeCamera()
    {
        // --- WEEK 2 ---
#if W2
#if TODO_0
        m_Camera.Initialize(45.0f, {0.0f, 0.0f, -10.0f});
#endif
#endif
    }

    void Renderer::InitializeOutputVertices()
    {
        // --- WEEK 2 ---
#if W2
#if TODO_0
        vertices_out.resize(vertices_world.size());
#endif
#endif
    }

    void Renderer::InitializeMesh()
    {
        // --- WEEK 1 ---
#if W1
#if TODO_0
        m_MeshPtr = new Mesh(m_DevicePtr, vertices, indices);
#endif
        
        // --- WEEK 2 ---
#elif W2
#if TODO_0
        m_MeshPtr = new Mesh(m_DevicePtr, vertices_out, indices);
#endif
#endif
    }

    void Renderer::InitializeTextures()
    {
        
    }
#pragma endregion

#pragma region Cleanup
    Renderer::~Renderer()
    {
        // Resources are released in reverse order of creation
        // 7. Render Target View
        // 6. Render Target Buffer
        // 5. Depth Stencil View
        // 4. Depth Stencil Buffer
        // 3. Swap Chain
        // 2. Device Context
        // 1. Device
        // 0. DXGI Factory

        if (m_RenderTargetViewPtr)   m_RenderTargetViewPtr->Release();
        if (m_RenderTargetBufferPtr) m_RenderTargetBufferPtr->Release();
        if (m_DepthStencilViewPtr)   m_DepthStencilViewPtr->Release();
        if (m_DepthStencilBufferPtr) m_DepthStencilBufferPtr->Release();
        if (m_SwapChainPtr)          m_SwapChainPtr->Release();
        if (m_DeviceContextPtr)
        {
            m_DeviceContextPtr->ClearState();
            m_DeviceContextPtr->Flush();
            m_DeviceContextPtr->Release();
        }
        if (m_DevicePtr)      m_DevicePtr->Release();
        if (m_DXGIFactoryPtr) m_DXGIFactoryPtr->Release();

        delete m_MeshPtr;
    }
#pragma endregion

#pragma region Update & Render
    void Renderer::Update(const Timer* timerPtr)
    {
        // --- WEEK 2 ---
#if W2
#if TODO_0
        m_Camera.Update(timerPtr);
#endif
#endif
    }

    void Renderer::Render() const
    {
        if (not m_IsInitialized) return;

        // 1. Clear RTV and DSV
        //=======================================================================================================
        const float clearColor[] = {0.0f, 0.0f, 0.3f, 1.0f};
        m_DeviceContextPtr->ClearRenderTargetView(m_RenderTargetViewPtr, clearColor);
        m_DeviceContextPtr->ClearDepthStencilView(m_DepthStencilViewPtr, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        // 2. Set pipeline + invoke draw calls (= render)
        //=======================================================================================================
#if W1
#if TODO_0
        Render_W1_TODO_0();
#endif
#elif W2
#if TODO_0
        Render_W2_TODO_0();
#endif
#endif
        
        // 3. Present backbuffer (swap)
        //=======================================================================================================
        m_SwapChainPtr->Present(0, 0);
    }


#pragma endregion

#pragma region Transformations
    void Renderer::TransformFromWorldToProjection(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out) const
    {
        for (size_t i{0}; i < vertices_in.size(); ++i)
        {
            const Vertex& vertex_in = vertices_in[i];
            Vertex_Out& vertex_out = vertices_out[i];

            // MODEL/OBJECT
            const Vector4 positionIn{vertex_in.position.x, vertex_in.position.y, vertex_in.position.z, 1.0f};
            // WORLD -> VIEW -> PROJECTION
            const Vector4 projectedPos = (m_Camera.m_InverseViewMatrix * m_Camera.m_ProjectionMatrix).TransformPoint(positionIn);
            vertices_out[i].position = projectedPos;
        }
    }
#pragma endregion

#pragma region Week 1
    void Renderer::Render_W1_TODO_0() const
    {
        m_MeshPtr->Render();
    }
#pragma endregion

#pragma region Week 2
    void Renderer::Render_W2_TODO_0() const
    {
        TransformFromWorldToProjection(vertices_world, vertices_out);
    }
#pragma endregion
}
