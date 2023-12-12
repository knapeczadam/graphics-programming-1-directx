#include "pch.h"
#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>

#include <algorithm>
#include <iostream>

namespace dae
{
    Texture::Texture(SDL_Surface* pSurface) :
        m_SurfacePtr{pSurface},
        m_SurfacePixelsPtr{(uint32_t*)pSurface->pixels}
    {
    }

    Texture::~Texture()
    {
        if (m_SurfacePtr)
        {
            SDL_FreeSurface(m_SurfacePtr);
            m_SurfacePtr = nullptr;
        }
    }

    Texture* Texture::LoadFromFile(const std::string& path)
    {
        SDL_Surface* pSurface = IMG_Load(path.c_str());
        if (!pSurface)
        {
            std::cout << "Texture::LoadFromFile() failed: " << SDL_GetError() << std::endl;
            return nullptr;
        }
        return new Texture(pSurface);
    }

    /**
     * \brief Sample the correct texel for the given uv
     * \param uv 
     * \return 
     */
    ColorRGB Texture::Sample(const Vector2& uv) const
    {
        // Clamp uv to [0, 1]
        float x {std::clamp(uv.x, 0.f, 1.f)};
        float y {std::clamp(uv.y, 0.f, 1.f)};

        // Calculate the index of the pixel
        x = x * static_cast<float>(m_SurfacePtr->w);
        y = y * static_cast<float>(m_SurfacePtr->h);
        const int index{static_cast<int>(y) * m_SurfacePtr->w + static_cast<int>(x)};

        // Get the pixel at the index
        const uint32_t pixel{m_SurfacePixelsPtr[index]};

        // Convert the pixel to a ColorRGB, range [0, 1]
        uint8_t r, g, b;
        SDL_GetRGB(pixel, m_SurfacePtr->format, &r, &g, &b);
        return ColorRGB{static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f};
    }
}
