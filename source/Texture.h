#pragma once
#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"

namespace dae
{
    struct Vector2;

    class Texture final
    {
    public:
        Texture() = default;
        ~Texture();

        Texture(const Texture& other)                = delete;
        Texture(Texture&& other) noexcept            = delete;
        Texture& operator=(const Texture& other)     = delete;
        Texture& operator=(Texture&& other) noexcept = delete;

        static Texture* LoadFromFile(const std::string& path);
        static Texture* LoadFromFile(const std::string& path, ID3D11Device* devicePtr);
        ColorRGB Sample(const Vector2& uv) const;
        inline ID3D11ShaderResourceView* GetSRV() const { return m_SRVPtr; }

    private:
        Texture(SDL_Surface* pSurface);
        Texture(SDL_Surface* pSurface, ID3D11Device* devicePtr);

        void FreeSurface();

        SDL_Surface* m_SurfacePtr       = nullptr;
        uint32_t*    m_SurfacePixelsPtr = nullptr;

        // DirectX
        ID3D11ShaderResourceView* m_SRVPtr      = nullptr;
        ID3D11Texture2D*          m_ResourcePtr = nullptr;
    };
}

