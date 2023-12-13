#include "pch.h"

// Project includes
#include "Renderer.h"
#include "SceneSelector.h"
#include "Mesh.h"
#include "Texture.h"
#include "Utils.h"

// DirectX headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

// ImGui includes
#include "imgui.h"
#include "imgui_impl_dx11.h"

// Standard includes
#include <cassert>

namespace dae
{
#pragma region Global Variables
    const std::vector<Vertex> triangle_vertices_ndc
    {
        {{ 0.0f,  0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}
    };
    
    const std::vector<Vertex> triangle_vertices_world
    {
        {{ 0.0f,  3.0f, 2.0f},{1.0f, 0.0f, 0.0f}},
        {{ 3.0f, -3.0f, 2.0f},{0.0f, 0.0f, 1.0f}},
        {{-3.0f, -3.0f, 2.0f},{0.0f, 1.0f, 0.0f}}
    };

    const std::vector<Vertex> quad_vertices_world
    {
        Vertex{{-3,  3, -2}, {}, {0, 0}},
        Vertex{{ 3,  3, -2}, {}, {1, 0}},
        Vertex{{-3, -3, -2}, {}, {0, 1}},
        Vertex{{ 3, -3, -2}, {}, {1, 1}}
    };
    
    std::vector<uint32_t> triangle_indices {0, 1, 2};
    std::vector<uint32_t> quad_indices     {0, 1,  2, 2, 1, 3};

    std::vector<Vertex>   vehicle_vertices {};
    std::vector<uint32_t> vehicle_indices  {};
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

        // Setup Platform/Renderer backends - Part 2
        ImGui_ImplDX11_Init(m_DevicePtr, m_DeviceContextPtr);
        
        // General initialization
        //=======================================================================================================
        InitializeObjects();
        InitializeMesh();
        InitializeCamera();
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
        const float aspectRatio{static_cast<float>(m_Width) / static_cast<float>(m_Height)};
        m_Camera.SetAspectRatio(aspectRatio);
        
        // --- WEEK 2 ---
#if W2
#if TODO_0
        m_Camera.Initialize(45.0f, {0.0f, 0.0f, -10.0f});
#elif TODO_1
        m_Camera.Initialize(45.0f, {0.0f, 0.0f, -10.0f});
#elif TODO_2
        m_Camera.Initialize(45.0f, {0.0f, 0.0f, -10.0f});
#elif TODO_3
        m_Camera.Initialize(45.0f, {0.0f, 0.0f, -50.0f});
#endif
        
        // --- WEEK 3 ---
#elif W3
#if TODO_0
        m_Camera.Initialize(45.0f, {0.0f, 0.0f, -50.0f});
#endif
#endif
    }

    void Renderer::InitializeMesh()
    {
        // --- WEEK 1 ---
#if W1
#if TODO_0
        m_MeshPtr = new Mesh(m_DevicePtr, triangle_vertices_ndc, triangle_indices);
#endif
        
        // --- WEEK 2 ---
#elif W2
#if TODO_0
        m_MeshPtr = new Mesh(m_DevicePtr, triangle_vertices_world, triangle_indices);
#elif TODO_1
        m_MeshPtr = new Mesh(m_DevicePtr, quad_vertices_world, quad_indices);
#elif TODO_2
        m_MeshPtr = new Mesh(m_DevicePtr, quad_vertices_world, quad_indices);
#elif TODO_3
        m_MeshPtr = new Mesh(m_DevicePtr, vehicle_vertices, vehicle_indices);
#endif
        
        // --- WEEK 3 ---
#elif W3
#if TODO_0
        m_MeshPtr = new Mesh(m_DevicePtr, vehicle_vertices, vehicle_indices);
#endif
#endif
    }

    void Renderer::InitializeTextures()
    {
        // --- WEEK 2 ---
#if W2
#if TODO_1
        m_TexturePtr = Texture::LoadFromFile(m_UVGrid2TexturePath, m_DevicePtr);
        m_MeshPtr->SetDiffuseMap(m_TexturePtr);
#elif TODO_2
        m_TexturePtr = Texture::LoadFromFile(m_UVGrid2TexturePath, m_DevicePtr);
        m_MeshPtr->SetDiffuseMap(m_TexturePtr);
#elif TODO_3
        m_DiffuseTexturePtr = Texture::LoadFromFile(m_DiffuseTexturePath, m_DevicePtr);
        m_MeshPtr->SetDiffuseMap(m_DiffuseTexturePtr);
#endif
        
        // --- WEEK 3 ---
#elif W3
#if TODO_0
        m_DiffuseTexturePtr    = Texture::LoadFromFile(m_DiffuseTexturePath,    m_DevicePtr);
        m_NormalTexturePtr     = Texture::LoadFromFile(m_NormalTexturePath,     m_DevicePtr);
        m_SpecularTexturePtr   = Texture::LoadFromFile(m_SpecularTexturePath,   m_DevicePtr);
        m_GlossinessTexturePtr = Texture::LoadFromFile(m_GlossinessTexturePath, m_DevicePtr);
        
        m_MeshPtr->SetDiffuseMap(m_DiffuseTexturePtr);
        m_MeshPtr->SetNormalMap(m_NormalTexturePtr);
        m_MeshPtr->SetSpecularMap(m_SpecularTexturePtr);
        m_MeshPtr->SetGlossinessMap(m_GlossinessTexturePtr);
#endif
#endif
    }

    void Renderer::InitializeObjects()
    {
#if W2
#if TODO_3
        Utils::ParseOBJ(m_VehiclePath, vehicle_vertices, vehicle_indices);
#endif
#elif W3
#if TODO_0
        Utils::ParseOBJ(m_VehiclePath, vehicle_vertices, vehicle_indices);
#endif
#endif
    }

#pragma endregion

#pragma region Cleanup
    Renderer::~Renderer()
    {
        // DirectX
        //=======================================================================================================
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

        // General
        //=======================================================================================================
        delete m_MeshPtr;

        delete m_TexturePtr;
        delete m_DiffuseTexturePtr;
        delete m_GlossinessTexturePtr;
        delete m_NormalTexturePtr;
        delete m_SpecularTexturePtr;
    }
#pragma endregion

#pragma region Update & Render
    void Renderer::Update(const Timer* timerPtr)
    {
        // --- WEEK 2 ---
#if W2
#if TODO_0
        m_Camera.Update(timerPtr);
        m_MeshPtr->SetMatrix(m_Camera.GetInverseViewMatrix(), m_Camera.GetProjectionMatrix());
#elif TODO_1
        m_Camera.Update(timerPtr);
        m_MeshPtr->SetMatrix(m_Camera.GetInverseViewMatrix(), m_Camera.GetProjectionMatrix());
#elif TODO_2
        m_Camera.Update(timerPtr);
        m_MeshPtr->SetMatrix(m_Camera.GetInverseViewMatrix(), m_Camera.GetProjectionMatrix());
#elif TODO_3
        m_Camera.Update(timerPtr);
        
        m_MeshPtr->SetMatrix(m_Camera.GetInverseViewMatrix(), m_Camera.GetProjectionMatrix());
        m_MeshPtr->SetTime(m_AccTime);
        
        Rotate(timerPtr->GetElapsed());
#endif
#elif W3
#if TODO_0
        m_Camera.Update(timerPtr);
        
        m_MeshPtr->SetMatrix(m_Camera.GetInverseViewMatrix(), m_Camera.GetProjectionMatrix());
        m_MeshPtr->SetCameraPosition(m_Camera.GetPosition());
        
        m_MeshPtr->SetUseNormalMap(m_UseNormalMap);
        m_MeshPtr->SetTime(m_AccTime);
        m_MeshPtr->SetShadingMode(static_cast<UINT>(m_CurrentShadingMode));
        
        m_MeshPtr->SetAmbient(m_Ambient);
        m_MeshPtr->SetLightDirection(m_LightDirection);
        m_MeshPtr->SetLightIntensity(m_LightIntensity);
        m_MeshPtr->SetKD(m_KD);
        m_MeshPtr->SetShininess(m_Shininess);
        
        Rotate(timerPtr->GetElapsed());
#endif
#endif
    }

    void Renderer::Render()
    {
        if (not m_IsInitialized) return;

        // 1. Clear RTV and DSV
        //=======================================================================================================
        const float clearColor[] = {m_BackgroundColor[0], m_BackgroundColor[1], m_BackgroundColor[2], 1.0f};
        m_DeviceContextPtr->ClearRenderTargetView(m_RenderTargetViewPtr, clearColor);
        m_DeviceContextPtr->ClearDepthStencilView(m_DepthStencilViewPtr, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        // 2. Set pipeline + invoke draw calls (= render)
        //=======================================================================================================
        
        // --- WEEK 1 ---
#if W1
#if TODO_0
        Render_W1_TODO_0();
#endif
        
        // --- WEEK 2 ---
#elif W2
#if TODO_0
        Render_W2_TODO_0();
#elif TODO_1
        Render_W2_TODO_1();
#elif TODO_2
        Render_W2_TODO_2();
#elif TODO_3
        Render_W2_TODO_3();
#endif
        
        // --- WEEK 3 ---
#elif W3
#if TODO_0
        Render_W3_TODO_0();
#endif
#endif

        // 3. UI
        //=======================================================================================================
        RenderUI();
        
        // 4. Present backbuffer (swap)
        //=======================================================================================================
        m_SwapChainPtr->Present(0, 0);
    }
#pragma endregion

#pragma region Settings
    void Renderer::CycleSamplerState()
    {
        m_SamplerState = static_cast<SamplerState>((static_cast<int>(m_SamplerState) + 1) % static_cast<int>(SamplerState::COUNT));
        m_MeshPtr->SetPassIdx(static_cast<UINT>(m_SamplerState));
        UpdateSamplerStateString();
    }

    void Renderer::CycleShadingMode()
    {
        m_CurrentShadingMode = static_cast<ShadingMode>(
            (static_cast<int>(m_CurrentShadingMode) + 1) % static_cast<int>(ShadingMode::COUNT)
            );
        UpdateShadingModeString();
    }

    void Renderer::Rotate(float deltaTime)
    {
        if (m_Rotate)
        {
            m_AccTime += deltaTime;
        }
    }

    void Renderer::ToggleRotation()
    {
        m_Rotate = not m_Rotate;
    }

    void Renderer::ToggleNormalVisibility()
    {
        m_UseNormalMap = not m_UseNormalMap;
    }
#pragma endregion
    
#pragma region UI
    void Renderer::CreateUI()
    {
        // ImGui Window
        ImGui::Begin("Properties");
        ImGui::Text("Shading  mode: %s", m_CurrentShadingModeString.c_str());
        ImGui::Text("Sampler state: %s", m_SamplerStateString.c_str());
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::ColorEdit3("Background color", m_BackgroundColor);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Checkbox("Normal map", &m_UseNormalMap);
        ImGui::Checkbox("Rotate", &m_Rotate);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::ColorEdit3("Ambient", m_Ambient);
        ImGui::SliderFloat3("Light direction", m_LightDirection, -1.0f, 1.0f);
        ImGui::SliderFloat("Light intensity", &m_LightIntensity, 0.0f, 20.0f);
        ImGui::SliderFloat("KD (Diffuse reflection coefficient)", &m_KD, 0.0f, 20.0f);
        ImGui::SliderFloat("Shininess", &m_Shininess, 0.0f, 100.0f);
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Take screenshot"))
        {
            // TakeScreenshot();
        }
        ImGui::SameLine();
        if (ImGui::Button("Start benchmark"))
        {
            // StartBenchmark();
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::End();
        
        // Rendering
        ImGui::Render();
        // This line makes the object transparent
        // m_DeviceContextPtr->OMSetRenderTargets(1, &m_RenderTargetViewPtr, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    void Renderer::RenderUI()
    {
#if W3
#if TODO_0
        CreateUI();
#endif
#endif
    }
#pragma endregion

#pragma region Helper Functions
    void Renderer::UpdateSamplerStateString()
    {
        switch (m_SamplerState)
        {
            case SamplerState::Point:
                m_SamplerStateString = "POINT";
                break;
            case SamplerState::Linear:
                m_SamplerStateString = "LINEAR";
                break;
            case SamplerState::Anisotropic:
                m_SamplerStateString = "ANISOTROPIC";
                break;
        }
    }

    void Renderer::UpdateShadingModeString()
    {
        switch (m_CurrentShadingMode)
        {
        case ShadingMode::BoundingBox:
            m_CurrentShadingModeString = "BOUNDING BOX";
            break;
        case ShadingMode::DepthBuffer:
            m_CurrentShadingModeString = "DEPTH BUFFER";
            break;
        case ShadingMode::ObservedArea:
            m_CurrentShadingModeString = "OBSERVED AREA";
            break;
        case ShadingMode::Diffuse:
            m_CurrentShadingModeString = "DIFFUSE";
            break;
        case ShadingMode::Specular:
            m_CurrentShadingModeString = "SPECULAR";
            break;
        case ShadingMode::Combined:
            m_CurrentShadingModeString = "COMBINED";
            break;
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
        m_MeshPtr->Render();
    }

    void Renderer::Render_W2_TODO_1() const
    {
        m_MeshPtr->Render();
    }

    void Renderer::Render_W2_TODO_2() const
    {
        m_MeshPtr->Render();
    }

    void Renderer::Render_W2_TODO_3() const
    {
        m_MeshPtr->Render();
    }
#pragma endregion

#pragma region Week 3
    void Renderer::Render_W3_TODO_0() const
    {
        m_MeshPtr->Render();
    }
#pragma endregion
}
