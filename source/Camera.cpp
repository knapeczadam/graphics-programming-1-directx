#include "pch.h"
#include "Camera.h"

namespace dae
{
    Camera::Camera(const Vector3& _origin, float _fovAngle)
        : m_Origin{_origin}
          , m_FOVAngle{_fovAngle}
    {
    }

    void Camera::Initialize(float _fovAngle, Vector3 _origin, float _nearPlane, float _farPlane)
    {
        m_FOVAngle = _fovAngle;
        m_FOV = CalculateFOV(m_FOVAngle);

        m_Origin = _origin;
        m_NearPlane = _nearPlane;
        m_FarPlane = _farPlane;
    }

    void Camera::Update(Timer* pTimer)
    {
        const float deltaTime = pTimer->GetElapsed();

        //Camera Update Logic

        //Keyboard Input
        const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
        MoveCamera(pKeyboardState, deltaTime);
        RotateCamera(deltaTime);

        //Update Matrices
        CalculateViewMatrix();
        CalculateProjectionMatrix();
        //Try to optimize this - should only be called once or when fov/aspectRatio changes
    }

    float Camera::GetFOV() const
    {
        return CalculateFOV(m_FOVAngle);
    }

    void Camera::Scroll(SDL_MouseWheelEvent wheel)
    {
        if (wheel.y > 0) // scroll up
        {
            m_Origin += m_Forward * m_ScrollSpeed;
        }
        else if (wheel.y < 0) // scroll down
        {
            m_Origin -= m_Forward * m_ScrollSpeed;
        }
    }

    void Camera::IncreaseFOV()
    {
        ++m_FOVAngle;
        CalculateFOV();
    }

    void Camera::DecreaseFOV()
    {
        --m_FOVAngle;
        CalculateFOV();
    }

    void Camera::SetTotalPitch(float pitch)
    {
        m_TotalPitch = pitch;
    }

    void Camera::SetTotalYaw(float yaw)
    {
        m_TotalYaw = yaw;
    }

    float Camera::CalculateFOV(float angle) const
    {
        const float halfAlpha{(angle * 0.5f) * TO_RADIANS};
        return std::tanf(halfAlpha);
    }

    void Camera::CalculateFOV()
    {
        m_FOV = CalculateFOV(m_FOVAngle);
    }

    void Camera::MoveCamera(const uint8_t* pKeyboardState, float deltaTime)
    {
        if (pKeyboardState[SDL_SCANCODE_A])
        {
            m_Origin -= m_Right * deltaTime * m_Speed;
        }
        else if (pKeyboardState[SDL_SCANCODE_D])
        {
            m_Origin += m_Right * deltaTime * m_Speed;
        }
        if (pKeyboardState[SDL_SCANCODE_W])
        {
            m_Origin += m_Forward * deltaTime * m_Speed;
        }
        else if (pKeyboardState[SDL_SCANCODE_S])
        {
            m_Origin -= m_Forward * deltaTime * m_Speed;
        }
    }

    void Camera::RotateCamera(float deltaTime)
    {
        int mouseX{}, mouseY{};
        const int threshold{1};
        
        const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
        
        mouseX = mouseX > threshold ? 1 : mouseX < -threshold ? -1 : 0;
        mouseY = mouseY > threshold ? 1 : mouseY < -threshold ? -1 : 0;
        
        const bool leftMouseButtonDown  = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
        const bool rightMouseButtonDown = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
        
        if (leftMouseButtonDown and rightMouseButtonDown)
        {
            m_Origin += m_Up * static_cast<float>(mouseY * -1) * deltaTime * m_Speed;
        }
        else if (leftMouseButtonDown)
        {
            m_Origin += m_Forward * static_cast<float>(mouseY * -1) * deltaTime * m_Speed;
            m_TotalYaw += static_cast<float>(mouseX) * m_RotationSpeed * deltaTime;
        }
        else if (rightMouseButtonDown)
        {
            m_TotalYaw += static_cast<float>(mouseX) * m_RotationSpeed * deltaTime;
            m_TotalPitch += static_cast<float>(mouseY * -1) * m_RotationSpeed * deltaTime;
        }
        if (mouseX or mouseY)
        {
            m_Forward = Matrix::CreateRotation(m_TotalPitch * TO_RADIANS, m_TotalYaw * TO_RADIANS, 0.0f).TransformVector(
                Vector3::UnitZ);
            // TODO: verify if this is needed
            //forward.Normalize();
        }
    }

    void Camera::CalculateViewMatrix()
    {
        m_ViewMatrix        = Matrix::CreateLookAtLH(m_Origin, m_Forward, m_Up, m_Right);
        m_InverseViewMatrix = Matrix::Inverse(m_ViewMatrix);
    }

    void Camera::CalculateProjectionMatrix()
    {
        m_ProjectionMatrix = Matrix::CreatePerspectiveFovLH(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
    }

}
