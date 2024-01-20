#pragma once

#include "MathHelpers.h"

namespace dae
{
    struct ColorRGB
    {
        ColorRGB() = default;
        ColorRGB(float _r, float _g, float _b) : r{_r}, g{_g}, b{_b} { }
        ColorRGB(float c) : r{c}, g{c}, b{c} { }
        
        ColorRGB(const ColorRGB& other)                = default;
        ColorRGB(ColorRGB&& other) noexcept            = default;
        ColorRGB& operator=(const ColorRGB& other)     = default;
        ColorRGB& operator=(ColorRGB&& other) noexcept = default;

        ~ColorRGB() = default;

        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;

        void MaxToOne()
        {
            const float maxValue = std::max(r, std::max(g, b));
            if (maxValue > 1.f)
                *this /= maxValue;
        }

        static ColorRGB Lerp(const ColorRGB& c1, const ColorRGB& c2, float factor)
        {
            return {Lerpf(c1.r, c2.r, factor), Lerpf(c1.g, c2.g, factor), Lerpf(c1.b, c2.b, factor)};
        }

#pragma region ColorRGB (Member) Operators
        ColorRGB& operator+=(const ColorRGB& c)
        {
            r += c.r;
            g += c.g;
            b += c.b;
            return *this;
        }

        ColorRGB& operator-=(const ColorRGB& c)
        {
            r -= c.r;
            g -= c.g;
            b -= c.b;
            return *this;
        }

        ColorRGB& operator*=(const ColorRGB& c)
        {
            r *= c.r;
            g *= c.g;
            b *= c.b;
            return *this;
        }

        ColorRGB& operator/=(const ColorRGB& c)
        {
            r /= c.r;
            g /= c.g;
            b /= c.b;
            return *this;
        }
#pragma endregion
    };

#pragma region ColorRGB (Global) Operators
    inline ColorRGB operator+(const ColorRGB& lhs, const ColorRGB& rhs)
    {
        return {lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b};
    }

    inline ColorRGB operator-(const ColorRGB& lhs, const ColorRGB& rhs)
    {
        return {lhs.r - rhs.r, lhs.g - rhs.g, lhs.b - rhs.b};
    }

    inline ColorRGB operator*(const ColorRGB& lhs, const ColorRGB& rhs)
    {
        return {lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b};
    }

    inline ColorRGB operator/(const ColorRGB& lhs, const ColorRGB& rhs)
    {
        return {lhs.r / rhs.r, lhs.g / rhs.g, lhs.b / rhs.b};
    }
#pragma endregion

    namespace colors
    {
        static ColorRGB Red        = {1, 0, 0};
        static ColorRGB Blue       = {0, 0, 1};
        static ColorRGB Green      = {0, 1, 0};
        static ColorRGB Yellow     = {1, 1, 0};
        static ColorRGB Cyan       = {0, 1, 1};
        static ColorRGB Magenta    = {1, 0, 1};
        static ColorRGB White      = {1, 1, 1};
        static ColorRGB Black      = {0, 0, 0};
        static ColorRGB Gray       = {0.5f, 0.5f, 0.5f};
        static ColorRGB Dielectric = {0.04f, 0.04f, 0.04f};
    }
}
