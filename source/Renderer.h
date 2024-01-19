#pragma once

// Project includes
#include "Camera.h"
#include "SceneSelector.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
    // Forward declarations
    struct Vertex;
    class Texture;
    class Mesh;
    
    enum class SamplerState
    {
        Point,
        Linear,
        Anisotropic,

        COUNT
    };
    
    enum class ShadingMode
    {
        BoundingBox = -2,
        DepthBuffer = -1,
        ObservedArea = 0,// Lambert Cosine Law
        Diffuse,
        Specular, 
        Combined, // (Diffuse + Specular) * ObservedArea

        COUNT = 4
    };

    enum class CullMode
    {
        None,
        Front,
        Back,

        COUNT
    };

    enum class FillMode
    {
        Solid,
        Wireframe,

        COUNT
    };
    
    class Renderer final
    {
    public:
        Renderer(SDL_Window* windowPtr, Timer* timerPtr);
        ~Renderer();

        Renderer(const Renderer&)                = delete;
        Renderer(Renderer&&) noexcept            = delete;
        Renderer& operator=(const Renderer&)     = delete;
        Renderer& operator=(Renderer&&) noexcept = delete;

        void Update(const Timer* timerPtr);
        void Render();

        // Settings
        void CycleSamplerState();
        void CycleShadingMode();
        void CycleCullMode();
        void CycleFillMode();
        
        void ToggleRotation();
        void ToggleNormalVisibility();
        void ToggleAlphaBlending();
        void ToggleFireFX();
        void ToggleUniformClearColor();
        void ToggleFPSCounter();
        void ToggleFrontCounterClockwise();

        bool UseFPSCounter() const { return m_UseFPSCounter; }
        
        void TakeScreenshot() const;

        inline Camera& GetCamera() { return m_Camera; }

    private:
        // Initialization
        void InitializeCamera();
        void InitializeMesh();
        void InitializeTextures();
        void InitializeObjects();

        // Helper functions
        void UpdateSamplerStateString();
        void UpdateShadingModeString();
        void UpdateCullModeString();
        void UpdateFillModeString();
        void Rotate(float deltaTime);

        // UI
        void CreateUI();
        void RenderUI();

        // Debug
        void PrintDebugInfo() const;

        // --- Week 1 ---
        void Render_W1_TODO_0() const;

        // --- Week 2 ---
        void Render_W2_TODO_0() const;
        void Render_W2_TODO_1() const;
        void Render_W2_TODO_2() const;
        void Render_W2_TODO_3() const;
        
        // --- Week 3 ---
        void Render_W3_TODO_0() const;
       
        // DIRECTX
        HRESULT InitializeDirectX();

    private:
        SDL_Window* m_WindowPtr = nullptr;
        Timer*      m_TimerPtr  = nullptr;

        int m_Width  = 0;
        int m_Height = 0;

        bool m_IsInitialized = false;
        
        // DIRECTX
        IDXGIFactory1*          m_DXGIFactoryPtr        = nullptr;
        ID3D11Device*           m_DevicePtr             = nullptr;
        ID3D11DeviceContext*    m_DeviceContextPtr      = nullptr;
        IDXGISwapChain*         m_SwapChainPtr          = nullptr;
        ID3D11Texture2D*        m_DepthStencilBufferPtr = nullptr;
        ID3D11DepthStencilView* m_DepthStencilViewPtr   = nullptr;
        ID3D11Resource*         m_RenderTargetBufferPtr = nullptr;
        ID3D11RenderTargetView* m_RenderTargetViewPtr   = nullptr;

        Camera m_Camera {};
        Mesh*  m_MeshPtr   =   nullptr;
        Mesh*  m_FireFXMeshPtr =   nullptr;
        
        // Path
#if CUSTOM_PATH
        const std::string m_ResourcesPath         = "../_Resources/";
#else
        const std::string m_ResourcesPath         = "Resources/";
#endif
        const std::string m_DiffuseTexturePath    = m_ResourcesPath + "vehicle_diffuse.png";
        const std::string m_GlossinessTexturePath = m_ResourcesPath + "vehicle_gloss.png";
        const std::string m_NormalTexturePath     = m_ResourcesPath + "vehicle_normal.png";
        const std::string m_SpecularTexturePath   = m_ResourcesPath + "vehicle_specular.png";
        const std::string m_UVGrid2TexturePath    = m_ResourcesPath + "uv_grid_2.png";
        const std::string m_FireFXTexturePath     = m_ResourcesPath + "fireFX_diffuse.png";
        
        const std::string m_VehiclePath           = m_ResourcesPath + "vehicle.obj";
        const std::string m_FireFXPath            = m_ResourcesPath + "fireFX.obj";
        
        // General texture
        Texture* m_TexturePtr = nullptr;

        // Vehicle
        Texture* m_DiffuseTexturePtr    = nullptr;
        Texture* m_GlossinessTexturePtr = nullptr;
        Texture* m_NormalTexturePtr     = nullptr;
        Texture* m_SpecularTexturePtr   = nullptr;

        // FireFX
        Texture* m_FireFXTexturePtr = nullptr;

        SamplerState m_SamplerState = SamplerState::Point;
        std::string  m_SamplerStateString ="POINT";

        ShadingMode m_ShadingMode       = ShadingMode::Combined;
        std::string m_ShadingModeString = "COMBINED";

        CullMode m_CullMode = CullMode::None;
        std::string m_CullModeString = "NONE";

        FillMode m_FillMode = FillMode::Solid;
        std::string m_FillModeString = "SOLID";

        bool  m_Rotate  = true;
        float m_AccTime = 0.0f;
        
        // Debug
        bool m_UseNormalMap             = true;
        bool m_UseAlphaBlending         = true;
        bool m_UseFireFX                = true;
        bool m_UseClearColor            = false;
        bool m_UseFPSCounter            = false;
        bool m_UseFrontCounterClockwise = false;

        bool m_ShowUI = true;

        UINT m_WithAlphaBlendingPassIdx    = 3;
        UINT m_WithoutAlphaBlendingPassIdx = 4;

        // Lighting
        float m_Ambient[3]        = {0.03f, 0.03f, 0.03f}; // 8, 8, 8
        float m_LightDirection[3] = {0.577f,  -0.577f, 0.577f}; 
        float m_LightIntensity    = 1.0f;
        float m_KD                = 7.0f; // Diffuse  reflection coefficient
        float m_Shininess         = 25.0f;
        
        float m_BackgroundColor[3] = {0.39f, 0.59f, 0.93f};
    };
}
