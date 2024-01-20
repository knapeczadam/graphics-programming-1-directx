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

#define RED_TEXT(text) "\033[1;31m" text "\033[0m"
#define GREEN_TEXT(text) "\033[1;32m" text "\033[0m"
#define MAGENTA_TEXT(text) "\033[1;35m" text "\033[0m"
#define YELLOW_TEXT(text) "\033[1;33m" text "\033[0m"

#define ONE_TAB "\t"
#define TWO_TABS "\t\t"
#define THREE_TABS "\t\t\t"

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
    std::vector<Vertex>   fireFx_vertices  {};
    std::vector<uint32_t> fireFx_indices   {};
#pragma endregion
    
#pragma region Initialization
    Renderer::Renderer(SDL_Window* windowPtr, Timer* timerPtr)
        : m_WindowPtr(windowPtr),
          m_TimerPtr(timerPtr)
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

        // Debug
        //=======================================================================================================
        PrintDebugInfo();
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

        // Create debug interface
        //=======================================================================================================
        m_DevicePtr->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&m_DebugPtr));

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
        m_MeshPtr       = new Mesh(m_DevicePtr, vehicle_vertices, vehicle_indices);
        m_FireFXMeshPtr = new Mesh(m_DevicePtr, fireFx_vertices,  fireFx_indices);
        m_FireFXMeshPtr->SetPassIdx(m_WithAlphaBlendingPassIdx);
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

        m_FireFXTexturePtr = Texture::LoadFromFile(m_FireFXTexturePath, m_DevicePtr);
        m_FireFXMeshPtr->SetDiffuseMap(m_FireFXTexturePtr);
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
        Utils::ParseOBJ(m_FireFXPath, fireFx_vertices, fireFx_indices);
#endif
#endif
    }

#pragma endregion

#pragma region Cleanup
    Renderer::~Renderer()
    {
        // General
        //=======================================================================================================
        delete m_MeshPtr;
        delete m_FireFXMeshPtr;
        
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

        SAFE_RELEASE(m_RenderTargetViewPtr)
        SAFE_RELEASE(m_RenderTargetBufferPtr)
        SAFE_RELEASE(m_DepthStencilViewPtr)
        SAFE_RELEASE(m_DepthStencilBufferPtr)
        SAFE_RELEASE(m_SwapChainPtr)
        if (m_DeviceContextPtr)
        {
            m_DeviceContextPtr->ClearState();
            m_DeviceContextPtr->Flush();
            SAFE_RELEASE(m_DeviceContextPtr)
        }
        SAFE_RELEASE(m_DevicePtr)
        SAFE_RELEASE(m_DXGIFactoryPtr)

        // Textures
        delete m_TexturePtr;
        delete m_DiffuseTexturePtr;
        delete m_GlossinessTexturePtr;
        delete m_NormalTexturePtr;
        delete m_SpecularTexturePtr;
        delete m_FireFXTexturePtr;

        // DirectX Debug
        //=======================================================================================================
        if (m_DebugPtr)
        {
            m_DebugPtr->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
            SAFE_RELEASE(m_DebugPtr)
        }
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

        // Vehicle
        m_MeshPtr->SetMatrix(m_Camera.GetInverseViewMatrix(), m_Camera.GetProjectionMatrix());
        m_MeshPtr->SetCameraPosition(m_Camera.GetPosition());
        
        m_MeshPtr->SetUseNormalMap(m_UseNormalMap);
        m_MeshPtr->SetTime(m_AccTime);
        m_MeshPtr->SetShadingMode(static_cast<int>(m_ShadingMode));
        
        m_MeshPtr->SetAmbient(m_Ambient);
        m_MeshPtr->SetLightDirection(m_LightDirection);
        m_MeshPtr->SetLightIntensity(m_LightIntensity);
        m_MeshPtr->SetKD(m_KD);
        m_MeshPtr->SetShininess(m_Shininess);

        // FireFX
        m_FireFXMeshPtr->SetMatrix(m_Camera.GetInverseViewMatrix(), m_Camera.GetProjectionMatrix());
        m_FireFXMeshPtr->SetTime(m_AccTime);
        m_FireFXMeshPtr->SetPassIdx(m_UseAlphaBlending ? m_WithAlphaBlendingPassIdx : m_WithoutAlphaBlendingPassIdx);
        
        Rotate(timerPtr->GetElapsed());
#endif
#endif
    }

    void Renderer::Render()
    {
        if (not m_IsInitialized) return;

        // 1. Clear RTV and DSV
        //=======================================================================================================
        float clearColor[] = {m_CurrentBackgroundColor[0], m_CurrentBackgroundColor[1], m_CurrentBackgroundColor[2], 1.0f};
        if (m_UseClearColor)
        {
            clearColor[0] = m_ClearColor[0];
            clearColor[1] = m_ClearColor[1];
            clearColor[2] = m_ClearColor[2];
        }
        else
        {
            clearColor[0] = m_CurrentBackgroundColor[0];
            clearColor[1] = m_CurrentBackgroundColor[1];
            clearColor[2] = m_CurrentBackgroundColor[2];
        }
        
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
        m_ShadingMode = static_cast<ShadingMode>(
            (static_cast<int>(m_ShadingMode) + 1) % static_cast<int>(ShadingMode::COUNT)
            );
        UpdateShadingModeString();
    }

    void Renderer::CycleCullMode()
    {
        m_CullMode = static_cast<CullMode>((static_cast<int>(m_CullMode) + 1) % static_cast<int>(CullMode::COUNT));
        m_MeshPtr->SetRasterizerState(m_FillMode, m_CullMode, m_UseFrontCounterClockwise);
        UpdateCullModeString();
    }

    void Renderer::CycleFillMode()
    {
        m_FillMode = static_cast<FillMode>((static_cast<int>(m_FillMode) + 1) % static_cast<int>(FillMode::COUNT));
        m_MeshPtr->SetRasterizerState(m_FillMode, m_CullMode, m_UseFrontCounterClockwise);
        UpdateFillModeString();
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
        std::string onOff = m_Rotate ? "ON" : "OFF";
        std::cout << GREEN_TEXT("**(HARDWARE) Vehicle Rotation ") << MAGENTA_TEXT("" + onOff + "") << '\n';
    }

    void Renderer::ToggleNormalVisibility()
    {
        m_UseNormalMap = not m_UseNormalMap;
        std::string onOff = m_UseNormalMap ? "ON" : "OFF";
        std::cout << GREEN_TEXT("**(HARDWARE) NormalMap ") << MAGENTA_TEXT("" + onOff + "") << '\n';
    }

    void Renderer::ToggleAlphaBlending()
    {
        m_UseAlphaBlending = not m_UseAlphaBlending;
        std::string onOff = m_UseAlphaBlending ? "ON" : "OFF";
        std::cout << GREEN_TEXT("**(HARDWARE) Alpha Blending ") << MAGENTA_TEXT("" + onOff + "") << '\n';
    }

    void Renderer::ToggleFireFX()
    {
        m_UseFireFX = not m_UseFireFX;
        std::string onOff = m_UseFireFX ? "ON" : "OFF";
        std::cout << GREEN_TEXT("**(HARDWARE) FireFX ") << MAGENTA_TEXT("" + onOff + "") << '\n';
    }

    void Renderer::ToggleUniformClearColor()
    {
        m_UseClearColor = not m_UseClearColor;
        std::string onOff = m_UseClearColor ? "ON" : "OFF";
        std::cout << GREEN_TEXT("**(HARDWARE) Uniform ClearColor ") << MAGENTA_TEXT("" + onOff + "") << '\n';
    }

    void Renderer::ToggleFPSCounter()
    {
        m_UseFPSCounter = not m_UseFPSCounter;
        std::string onOff = m_UseFPSCounter ? "ON" : "OFF";
        std::cout << GREEN_TEXT("**(HARDWARE) Print FPS ") << MAGENTA_TEXT("" + onOff + "") << '\n';
    }

    void Renderer::ToggleFrontCounterClockwise()
    {
        m_UseFrontCounterClockwise = not m_UseFrontCounterClockwise;
        std::string onOff = m_UseFrontCounterClockwise ? "ON" : "OFF";
        std::cout << GREEN_TEXT("**(HARDWARE) Front Counter Clockwise ") << MAGENTA_TEXT("" + onOff + "") << '\n';
    }

    void Renderer::TakeScreenshot() const
    {
        // Get the back buffer
        ID3D11Texture2D* pBackBuffer;
        m_SwapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    
        // Create a staging texture
        D3D11_TEXTURE2D_DESC desc;
        pBackBuffer->GetDesc(&desc);
        desc.Usage = D3D11_USAGE_STAGING;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.BindFlags = 0;
    
        ID3D11Texture2D* pStagingTexture;
        m_DevicePtr->CreateTexture2D(&desc, nullptr, &pStagingTexture);
    
        // Copy the back buffer to the staging texture
        m_DeviceContextPtr->CopyResource(pStagingTexture, pBackBuffer);
    
        // Map the staging texture to access its data
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        m_DeviceContextPtr->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);
    
        // Save the data to a BMP file using SDL2_image
        SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
            mappedResource.pData,
            desc.Width,
            desc.Height,
            32, // Assuming 32-bit pixel format
            mappedResource.RowPitch,
            0x000000FF,
            0x0000FF00,
            0x00FF0000,
            0x00000000
        );
    
        bool error = SDL_SaveBMP(surface, "screenshot.bmp");
    
        SDL_FreeSurface(surface);
        m_DeviceContextPtr->Unmap(pStagingTexture, 0);
    
        // Release resources
        pStagingTexture->Release();
        pBackBuffer->Release();

        if (not error)
        {
            std::cout << GREEN_TEXT("**(HARDWARE) Screenshot taken!") << '\n';
        }
        else
        {
            std::cout << RED_TEXT("**(HARDWARE) Screenshot failed!") << '\n';
        }
    }
#pragma endregion
    
#pragma region UI
    void Renderer::CreateUI()
    {
        // ImGui Window
        if (m_ShowUI)
        {
            ImGui::Begin("Properties", &m_ShowUI);
            ImGui::Text("F3:  Shading  mode: %s", m_ShadingModeString.c_str());
            ImGui::Text("F4:  Sampler state: %s", m_SamplerStateString.c_str());
            ImGui::Text("F10: FillMode     : %s", m_FillModeString.c_str());
            ImGui::Text("F11: CullMode     : %s", m_CullModeString.c_str());
        
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::ColorEdit3("Background color", m_CurrentBackgroundColor);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        
            ImGui::Checkbox("F1: Front Counter Clockwise", &m_UseFrontCounterClockwise);
            ImGui::Checkbox("F2: Alpha blending", &m_UseAlphaBlending);
            ImGui::Checkbox("F5: Rotate", &m_Rotate);
            ImGui::Checkbox("F6: Normal map", &m_UseNormalMap);
            ImGui::Checkbox("F7: FireFX", &m_UseFireFX);
            ImGui::Checkbox("F8: Uniform ClearColor", &m_UseClearColor);
            ImGui::Checkbox("F9: FPS", &m_UseFPSCounter);
        
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
                TakeScreenshot();
            }
            ImGui::SameLine();
            if (ImGui::Button("Start benchmark"))
            {
                m_TimerPtr->StartBenchmark();
            }

            if (m_UseFPSCounter)
            {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }
        
            ImGui::End();
        }
        ImGui::Render();
        
        // Rendering
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
        std::cout << GREEN_TEXT("**(HARDWARE) Sampler State = ") << MAGENTA_TEXT("" + m_SamplerStateString + "") << '\n';
    }

    void Renderer::UpdateShadingModeString()
    {
        switch (m_ShadingMode)
        {
        case ShadingMode::BoundingBox:
            m_ShadingModeString = "BOUNDING BOX";
            break;
        case ShadingMode::DepthBuffer:
            m_ShadingModeString = "DEPTH BUFFER";
            break;
        case ShadingMode::ObservedArea:
            m_ShadingModeString = "OBSERVED AREA";
            break;
        case ShadingMode::Diffuse:
            m_ShadingModeString = "DIFFUSE";
            break;
        case ShadingMode::Specular:
            m_ShadingModeString = "SPECULAR";
            break;
        case ShadingMode::Combined:
            m_ShadingModeString = "COMBINED";
            break;
        }
        std::cout << GREEN_TEXT("**(HARDWARE) Shading Mode = ") << MAGENTA_TEXT("" + m_ShadingModeString + "") << '\n';
    }

    void Renderer::UpdateCullModeString()
    {
        switch (m_CullMode)
        {
        case CullMode::Back:
            m_CullModeString = "BACK";
            break;
        case CullMode::Front:
            m_CullModeString = "FRONT";
            break;
        case CullMode::None:
            m_CullModeString = "NONE";
            break;
        }
        std::cout << GREEN_TEXT("**(HARDWARE) CullMode = ") << MAGENTA_TEXT("" + m_CullModeString + "") << '\n';
    }

    void Renderer::UpdateFillModeString()
    {
        switch (m_FillMode)
        {
            case FillMode::Solid:
                m_FillModeString = "SOLID";
                break;
            case FillMode::Wireframe:
                m_FillModeString = "WIREFRAME";
                break;
        }
        std::cout << GREEN_TEXT("**(HARDWARE) FillMode = ") << MAGENTA_TEXT("" + m_FillModeString + "") << '\n';
    }
#pragma endregion

#pragma region Debug
    void Renderer::PrintDebugInfo() const
    {
        const auto onOff = YELLOW_TEXT("(ON/OFF)");
        std::cout << YELLOW_TEXT("[Key Bindings]") << '\n';
        std::cout << '\t' << YELLOW_TEXT("[F1]") << ONE_TAB << YELLOW_TEXT("Front Counter Clockwise") << TWO_TABS << onOff << '\n';
        std::cout << '\t' << YELLOW_TEXT("[F2]") << ONE_TAB << YELLOW_TEXT("Alpha Blending") << THREE_TABS << onOff << '\n';
        std::cout << '\t' << YELLOW_TEXT("[F3]") << ONE_TAB << YELLOW_TEXT("Cycle Shading Mode") << TWO_TABS << YELLOW_TEXT("(COMBINED/OBSERVED AREA/DIFFUSE/SPECULAR)") << '\n';
        std::cout << '\t' << YELLOW_TEXT("[F4]") << ONE_TAB << YELLOW_TEXT("Cycle Sampler State") << TWO_TABS <<  YELLOW_TEXT("(POINT/LINEAR/ANISOTROPIC)") << '\n';
        std::cout << '\t' << YELLOW_TEXT("[F5]") << ONE_TAB << YELLOW_TEXT("Toggle Vehicle Rotation") << TWO_TABS <<  onOff << '\n';
        std::cout << '\t' << YELLOW_TEXT("[F6]") << ONE_TAB << YELLOW_TEXT("Toggle NormalMap") << TWO_TABS << onOff << '\n';
        std::cout << '\t' << YELLOW_TEXT("[F7]") << ONE_TAB << YELLOW_TEXT("Toggle FireFX") << THREE_TABS << onOff << '\n';
        std::cout << '\t' << YELLOW_TEXT("[F8]") << ONE_TAB << YELLOW_TEXT("Toggle Uniform ClearColor") << ONE_TAB << onOff << '\n';
        std::cout << '\t' << YELLOW_TEXT("[F9]") << ONE_TAB << YELLOW_TEXT("Toggle Print FPS") << TWO_TABS << onOff << '\n';
        std::cout << '\t' << YELLOW_TEXT("[10]") << ONE_TAB << YELLOW_TEXT("Cycle FillMode") << THREE_TABS << YELLOW_TEXT("(SOLID/WIREFRAME)") << '\n';
        std::cout << '\t' << YELLOW_TEXT("[11]") << ONE_TAB << YELLOW_TEXT("Cycle CullMode") << THREE_TABS << YELLOW_TEXT("(NONE/FRONT/BACK)") << '\n';
        std::cout << '\t' << YELLOW_TEXT("[12]") << ONE_TAB << YELLOW_TEXT("Take Screenshot") << '\n';
        std::cout << '\n';
        std::cout << '\n';
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
        if (m_UseFireFX) m_FireFXMeshPtr->Render();
    }
#pragma endregion
}
