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
        SDL_Window* m_WindowPtr {nullptr};

        int m_Width  {0};
        int m_Height {0};

        bool m_IsInitialized{false};

        //DIRECTX
        HRESULT InitializeDirectX();
        //...
    };
}
