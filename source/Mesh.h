#pragma once
#include "Renderer.h"

namespace dae
{
    // Forward declarations
    class Effect;
    class Texture;
    
    struct Vertex
    {
        Vector3  position = {0.0f,0.0f, 0.0f};
        ColorRGB color    = colors::White;
        Vector2  uv       = {0.0f, 1.0f};
        Vector3  normal   = {0.0f, 0.0f, 1.0f};
        Vector3  tangent  = {0.0f, 0.0f, 1.0f};
    };

    class Mesh final
    {
    public:
        Mesh(ID3D11Device* devicePtr, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~Mesh();

        Mesh(const Mesh& other)                = delete;
        Mesh(Mesh&& other) noexcept            = delete;
        Mesh& operator=(const Mesh& other)     = delete;
        Mesh& operator=(Mesh&& other) noexcept = delete;

        void Render() const;
        void SetMatrix(const Matrix& viewMatrix, const Matrix& projectionMatrix)                            const;
        void SetMatrix(const Matrix& worldMatrix, const Matrix& viewMatrix, const Matrix& projectionMatrix) const;

        // Texture variables
        void SetDiffuseMap(const Texture* diffuseTexturePtr)       const;
        void SetNormalMap(const Texture* normalMapTexturePtr)      const;
        void SetSpecularMap(const Texture* specularTexturePtr)     const;
        void SetGlossinessMap(const Texture* glossinessTexturePtr) const;

        // Scalar variables
        void SetTime(float time)                             const;
        void SetCameraPosition(const Vector3& viewDirection) const;
        void SetUseNormalMap(bool useNormalMap)              const;
        void SetShadingMode(int shadingMode)                 const;
        void SetAmbient(float* ambient)                      const;
        void SetLightDirection(float* lightDirection)        const;
        void SetLightIntensity(float lightIntensity)         const;
        void SetKD(float kd)                                 const;
        void SetShininess(float shininess)                   const;

        void SetRasterizerState(FillMode fillMode, CullMode cullingMode, bool frontCounterClockwise) const;
        
        void SetPassIdx(UINT passIdx) { m_PassIdx = passIdx; }

    private:
        void InitializeEffect();
        void InitializeMatrix();
        void InitializeTextures();
        void InitializeScalars();

        void Draw()     const;
        void LoadPass() const;

    private:
        //-------------------------------------------------------------------------------------------
        // POINTERS MANAGED BY OTHER CLASSES
        //-------------------------------------------------------------------------------------------
        
        // From Renderer
        ID3D11Device*                        m_DevicePtr                    = nullptr;

        //-------------------------------------------------------------------------------------------
        // POINTERS MANAGED BY THIS CLASS
        //-------------------------------------------------------------------------------------------
        
        // Effect
        Effect*                              m_EffectPtr                    = nullptr;

        // Shader variables
        ID3D11Buffer*                        m_VertexBufferPtr              = nullptr;
        ID3D11InputLayout*                   m_InputLayoutPtr               = nullptr;
        ID3D11Buffer*                        m_IndexBufferPtr               = nullptr;

        // Technique 
        ID3DX11EffectTechnique*              m_TechniquePtr                 = nullptr;
        // Device context created by m_DevicePtr->DetImmediateContext(&m_DeviceContextPtr);
        ID3D11DeviceContext*                 m_DeviceContextPtr             = nullptr;

        // Texture variables
        ID3DX11EffectShaderResourceVariable* m_DiffuseMapVariablePtr        = nullptr;
        ID3DX11EffectShaderResourceVariable* m_NormalMapVariablePtr         = nullptr;
        ID3DX11EffectShaderResourceVariable* m_SpecularMapVariablePtr       = nullptr;
        ID3DX11EffectShaderResourceVariable* m_GlossinessMapVariablePtr     = nullptr;

        // Matrix variables
        ID3DX11EffectMatrixVariable*         m_WorldViewProjectionMatrixPtr = nullptr;
        
        // Scalar variables
        ID3DX11EffectScalarVariable*         m_TimeVariablePtr              = nullptr;
        ID3DX11EffectVectorVariable*         m_CameraPositionVariablePtr    = nullptr;
        ID3DX11EffectScalarVariable*         m_UseNormalMapVariablePtr      = nullptr;
        ID3DX11EffectScalarVariable*         m_ShadingModeVariablePtr       = nullptr;
        ID3DX11EffectVectorVariable*         m_AmbientVariablePtr           = nullptr;
        ID3DX11EffectVectorVariable*         m_LightDirectionVariablePtr    = nullptr;
        ID3DX11EffectScalarVariable*         m_LightIntensityVariablePtr    = nullptr;
        ID3DX11EffectScalarVariable*         m_KDVariablePtr                = nullptr;
        ID3DX11EffectScalarVariable*         m_ShininessVariablePtr         = nullptr;
        
        std::vector<Vertex> m_Vertices  {};
        std::vector<uint32_t> m_Indices {};
        uint32_t m_NumIndices = 0;

        UINT m_PassIdx = 0;
    };
}
