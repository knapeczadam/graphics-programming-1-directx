#pragma once
#include <SDL_events.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
    class Camera
    {
    public:
        Camera() = default;
        Camera(const Vector3& _origin, float _fovAngle);
        
        void Initialize(float _fovAngle = 90.0f, Vector3 _origin = {0.0f, 0.0f, 0.0f}, float nearPlane = 1.0f, float farPlane = 100.0f);
        void Update(Timer* pTimer);

        float GetFOV() const;
        void Scroll(SDL_MouseWheelEvent wheel);
        void IncreaseFOV();
        void DecreaseFOV();
        void SetTotalPitch(float pitch);
        void SetTotalYaw(float yaw);
        
        void CalculateViewMatrix();
        void CalculateProjectionMatrix();
        inline float GetAspectRatio() const { return m_AspectRatio; }
        inline void SetAspectRatio(float aspect_ratio) { m_AspectRatio = aspect_ratio; }
        inline Vector3 GetPosition() const { return m_Origin; }

    private:
        float CalculateFOV(float angle) const;
        void CalculateFOV();
        void MoveCamera(const uint8_t* pKeyboardState, float deltaTime);
        void RotateCamera(float deltaTime);

    public:
        Matrix m_InverseViewMatrix {};
        Matrix m_ViewMatrix        {};
        Matrix m_ProjectionMatrix  {};

    private:
        Vector3 m_Origin      {};
        float   m_FOVAngle    {0.0f};
        float   m_FOV         {0.0f};
        float   m_AspectRatio {0.0f};
        float   m_NearPlane   {0.0f};
        float   m_FarPlane    {0.0f};

        Vector3 m_Right   {Vector3::UnitX};
        Vector3 m_Up      {Vector3::UnitY};
        Vector3 m_Forward {Vector3::UnitZ};

        float  m_TotalPitch    {0.0f};
        float  m_TotalYaw      {0.0f};
        Matrix m_CameraToWorld {};
        float  m_Speed         {20.0f};
        float  m_RotationSpeed {100.0f};
        float  m_ScrollSpeed   {5.0f};
    };
}
