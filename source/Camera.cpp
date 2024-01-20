#include "pch.h"
#include "Camera.h"

#include "imgui.h"

namespace dae
{
#pragma region Initialization
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
#pragma endregion

#pragma region Update
    void Camera::Update(const Timer* pTimer)
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
#pragma endregion

#pragma region FOV
    float Camera::GetFOV() const
    {
        return CalculateFOV(m_FOVAngle);
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

    float Camera::CalculateFOV(float angle) const
    {
        const float halfAlpha{(angle * 0.5f) * TO_RADIANS};
        return std::tanf(halfAlpha);
    }

    void Camera::CalculateFOV()
    {
        m_FOV = CalculateFOV(m_FOVAngle);
    }
#pragma endregion

#pragma region Movement
    void Camera::Scroll(SDL_MouseWheelEvent wheel)
    {
        // Check whether the mouse is over the ImGui window
        if (ImGui::GetIO().WantCaptureMouse)
        {
            return;
        }
        
        if (wheel.y > 0) // scroll up
        {
            m_Origin += m_Forward * m_ScrollSpeed;
        }
        else if (wheel.y < 0) // scroll down
        {
            m_Origin -= m_Forward * m_ScrollSpeed;
        }
    }
    
    void Camera::MoveCamera(const uint8_t* pKeyboardState, float deltaTime)
    {
        const float speedMultiplier = pKeyboardState[SDL_SCANCODE_LSHIFT] or pKeyboardState[SDL_SCANCODE_RSHIFT] ? 2.0f : 1.0f;

        // Left/Right
        if (pKeyboardState[SDL_SCANCODE_A] or pKeyboardState[SDL_SCANCODE_LEFT])
        {
            m_Origin -= m_Right * deltaTime * m_KeyboardSpeed * speedMultiplier;
        }
        else if (pKeyboardState[SDL_SCANCODE_D] or pKeyboardState[SDL_SCANCODE_RIGHT])
        {
            m_Origin += m_Right * deltaTime * m_KeyboardSpeed * speedMultiplier;
        }

        // Up/Down
        if (pKeyboardState[SDL_SCANCODE_W] or pKeyboardState[SDL_SCANCODE_UP])
        {
            m_Origin += m_Forward * deltaTime * m_KeyboardSpeed * speedMultiplier;
        }
        else if (pKeyboardState[SDL_SCANCODE_S] or pKeyboardState[SDL_SCANCODE_DOWN])
        {
            m_Origin -= m_Forward * deltaTime * m_KeyboardSpeed * speedMultiplier;
        }
    }

    void Camera::RotateCamera(float deltaTime)
    {
        // Check whether the mouse is over the ImGui window
        if (ImGui::GetIO().WantCaptureMouse)
        {
            return;
        }
        
        int mouseX{}, mouseY{};
        const int threshold{1};
        
        const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
        
        mouseX = mouseX > threshold ? 1 : mouseX < -threshold ? -1 : 0;
        mouseY = mouseY > threshold ? 1 : mouseY < -threshold ? -1 : 0;
        
        const bool leftMouseButtonDown  = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
        const bool rightMouseButtonDown = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
        
        if (leftMouseButtonDown and rightMouseButtonDown)
        {
            m_Origin += m_Up * static_cast<float>(mouseY * -1) * deltaTime * m_MouseSpeed;
        }
        else if (leftMouseButtonDown)
        {
            m_Origin += m_Forward * static_cast<float>(mouseY * -1) * deltaTime * m_MouseSpeed;
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
#pragma endregion

#pragma region Matrix
    void Camera::CalculateViewMatrix()
    {
        m_ViewMatrix        = Matrix::CreateLookAtLH(m_Origin, m_Forward, m_Up, m_Right);
        m_InverseViewMatrix = Matrix::Inverse(m_ViewMatrix);
    }

    void Camera::CalculateProjectionMatrix()
    {
        m_ProjectionMatrix = Matrix::CreatePerspectiveFovLH(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
    }
#pragma endregion
}
