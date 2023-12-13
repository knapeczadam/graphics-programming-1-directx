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

    class Mesh
    {
    public:
        Mesh(ID3D11Device* devicePtr, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        ~Mesh();

        void Render() const;
        void SetMatrix(const Matrix& viewMatrix, const Matrix& projectionMatrix) const;
        void SetMatrix(const Matrix& worldMatrix, const Matrix& viewMatrix, const Matrix& projectionMatrix) const;
        
        void SetDiffuseMap(const Texture* diffuseTexturePtr) const;
        void SetNormalMap(const Texture* normalMapTexturePtr) const;
        void SetSpecularMap(const Texture* specularTexturePtr) const;
        void SetGlossinessMap(const Texture* glossinessTexturePtr) const;
        
        void SetTime(float time) const;
        inline void SetPassIdx(UINT passIdx) { m_PassIdx = passIdx; }

    private:
        void InitializeEffect();
        void InitializeMatrix();
        void InitializeTextures();
        void InitializeScalars();

        void Draw()     const;
        void LoadPass() const;

    private:
        // From Renderer
        ID3D11Device*        m_DevicePtr        = nullptr;
        ID3D11DeviceContext* m_DeviceContextPtr = nullptr;
        
        ID3D11Buffer*           m_VertexBufferPtr = nullptr;
        ID3D11InputLayout*      m_InputLayoutPtr  = nullptr;
        ID3D11Buffer*           m_IndexBufferPtr  = nullptr;

        // From Effect
        Effect*                              m_EffectPtr                    = nullptr;
        
        ID3DX11EffectTechnique*              m_TechniquePtr                 = nullptr;
        
        ID3DX11EffectMatrixVariable*         m_WorldViewProjectionMatrixPtr = nullptr;

        // Texture variables
        ID3DX11EffectShaderResourceVariable* m_DiffuseMapVariablePtr        = nullptr;
        ID3DX11EffectShaderResourceVariable* m_NormalMapVariablePtr         = nullptr;
        ID3DX11EffectShaderResourceVariable* m_SpecularMapVariablePtr       = nullptr;
        ID3DX11EffectShaderResourceVariable* m_GlossinessMapVariablePtr     = nullptr;

        // Scalar variables
        ID3DX11EffectScalarVariable*         m_TimeVariablePtr              = nullptr;
        
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;
        uint32_t m_NumIndices = 0;

        UINT m_PassIdx = 0;
    };
}
