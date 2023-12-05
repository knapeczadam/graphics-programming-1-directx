#pragma once

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
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
        // --- Week 1 ---
        void Render_W1_TODO_0() const;
        
        // DIRECTX
        HRESULT InitializeDirectX();

    private:
        SDL_Window* m_WindowPtr = nullptr;

        int m_Width  = 0;
        int m_Height = 0;

        bool m_IsInitialized = false;
        
        // DIRECTX
        ID3D11Device*        m_DevicePtr        = nullptr;
        ID3D11DeviceContext* m_DeviceContextPtr = nullptr;
        IDXGISwapChain*      m_SwapChainPtr     = nullptr;
        
    };
}
