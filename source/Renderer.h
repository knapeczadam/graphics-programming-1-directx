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
    
    class Renderer final
    {
    private:
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
            DepthBuffer,
            ObservedArea, // Lambert Cosine Law
            Diffuse,
            Specular, 
            Combined, // (Diffuse + Specular) * ObservedArea

            COUNT = 4
        };
        
    public:
        Renderer(SDL_Window* windowPtr);
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
        void ToggleRotation();
        void ToggleNormalVisibility();

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
        void Rotate(float deltaTime);

        // UI
        void CreateUI();
        void RenderUI();

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

        Camera m_Camera  {};
        Mesh*  m_MeshPtr = nullptr;
        
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
        const std::string m_VehiclePath           = m_ResourcesPath + "vehicle.obj";
        
        // General texture
        Texture* m_TexturePtr = nullptr;

        // Vehicle
        Texture* m_DiffuseTexturePtr    = nullptr;
        Texture* m_GlossinessTexturePtr = nullptr;
        Texture* m_NormalTexturePtr     = nullptr;
        Texture* m_SpecularTexturePtr   = nullptr;

        SamplerState m_SamplerState = SamplerState::Point;
        std::string  m_SamplerStateString ="POINT";

        ShadingMode m_PreviousShadingMode      = ShadingMode::Combined;
        ShadingMode m_CurrentShadingMode       = ShadingMode::Combined;
        std::string m_CurrentShadingModeString = "COMBINED";

        bool  m_Rotate  = true;
        float m_AccTime = 0.0f;
        
        // Debug
        bool m_UseNormalMap = true;

        // Lighting
        float m_Ambient[3]        = {0.03f, 0.03f, 0.03f}; // 8, 8, 8
        float m_LightDirection[3] = {0.577f,  -0.577f, 0.577f}; 
        float m_LightIntensity    = 1.0f;
        float m_KD                = 7.0f; // Diffuse  reflection coefficient
        float m_Shininess         = 25.0f;
        
        float m_BackgroundColor[3] = {0.3921f, 0.3921f, 0.3921f}; // 100, 100, 100
    };
}
