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
    struct Vertex_Out;
    
    class Mesh;
    
    class Renderer final
    {
    public:
        Renderer(SDL_Window* windowPtr);
        ~Renderer();

        Renderer(const Renderer&)                = delete;
        Renderer(Renderer&&) noexcept            = delete;
        Renderer& operator=(const Renderer&)     = delete;
        Renderer& operator=(Renderer&&) noexcept = delete;

        void Update(const Timer* timerPtr);
        void Render() const;

    private:
        // Initialization
        void InitializeCamera();
        void InitializeOutputVertices();
        void InitializeTextures();

        // Vertex Transformation
        void TransformFromWorldToProjection(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out) const;
        
        // --- Week 1 ---
        void Render_W1_TODO_0() const;

        // --- Week 2 ---
        void Render_W2_TODO_0() const;
        
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
    };
}
