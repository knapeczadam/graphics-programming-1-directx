#pragma once
#include <SDL_events.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
    class Camera final
    {
    public:
        Camera() = default;
        Camera(const Vector3& _origin, float _fovAngle);
        ~Camera() = default;

        Camera(const Camera& other)                = delete;
        Camera(Camera&& other) noexcept            = delete;
        Camera& operator=(const Camera& other)     = delete;
        Camera& operator=(Camera&& other) noexcept = delete;
        
        void Initialize(float _fovAngle = 90.0f, Vector3 _origin = {0.0f, 0.0f, 0.0f}, float nearPlane = 0.1f, float farPlane = 1000.0f);
        void Update(const Timer* pTimer);

        // --- Mouse ---
        void Scroll(SDL_MouseWheelEvent wheel);
        void IncreaseFOV();
        void DecreaseFOV();

        // --- Getters & Setters ---
        float   GetFOV() const;
        float   GetFOVAngle() const                { return m_FOVAngle;            }
        Vector3 GetPosition() const                { return m_Origin;              }
        float   GetAspectRatio() const             { return m_AspectRatio;         }
        void    SetAspectRatio(float aspect_ratio) { m_AspectRatio = aspect_ratio; }
        void    SetTotalPitch(float pitch)         { m_TotalPitch  = pitch;        }
        void    SetTotalYaw(float yaw)             { m_TotalYaw    = yaw;          }

        // --- Matrix ---
        Matrix GetWorldMatrix()       const { return m_WorldMatrix;       }
        Matrix GetViewMatrix()        const { return m_ViewMatrix;        }
        Matrix GetInverseViewMatrix() const { return m_InverseViewMatrix; }
        Matrix GetProjectionMatrix()  const { return m_ProjectionMatrix;  }
        
        void CalculateViewMatrix();
        void CalculateProjectionMatrix();

    private:
        float CalculateFOV(float angle) const;
        void  CalculateFOV();
        void  MoveCamera(const uint8_t* pKeyboardState, float deltaTime);
        void  RotateCamera(float deltaTime);

    private:
        Matrix m_WorldMatrix       {};
        Matrix m_InverseViewMatrix {};
        Matrix m_ViewMatrix        {};
        Matrix m_ProjectionMatrix  {};

        Vector3 m_Origin      {};
        float   m_FOVAngle    = 0.0f;
        float   m_FOV         = 0.0f;
        float   m_AspectRatio = 0.0f;
        float   m_NearPlane   = 0.0f;
        float   m_FarPlane    = 0.0f;

        Vector3 m_Right   { Vector3::UnitX };
        Vector3 m_Up      { Vector3::UnitY };
        Vector3 m_Forward { Vector3::UnitZ };

        float  m_TotalPitch    = 0.0f;
        float  m_TotalYaw      = 0.0f;
        float  m_KeyboardSpeed = 50.0f;
        float  m_MouseSpeed    = 500.0f;
        float  m_RotationSpeed = 500.0f;
        float  m_ScrollSpeed   = 5.0f;
    };
}
