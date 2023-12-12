#pragma once

namespace dae
{
    // Forward declarations
    class Effect;
    
    struct Vertex
    {
        Vector3 position;
        ColorRGB color;
    };

    struct Vertex_Out
    {
        Vector4 position;
        ColorRGB color;
    };

    class Mesh
    {
    public:
        Mesh(ID3D11Device* devicePtr, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        Mesh(ID3D11Device* devicePtr, const std::vector<Vertex_Out>& vertices, const std::vector<uint32_t>& indices);
        ~Mesh();

        void Render() const;

    private:
        ID3D11Device*        m_DevicePtr        = nullptr;
        ID3D11DeviceContext* m_DeviceContextPtr = nullptr;
        
        ID3D11Buffer*           m_VertexBufferPtr = nullptr;
        ID3D11InputLayout*      m_InputLayoutPtr  = nullptr;
        ID3D11Buffer*           m_IndexBufferPtr  = nullptr;

        Effect*                      m_EffectPtr                    = nullptr;
        ID3DX11EffectTechnique*      m_TechniquePtr                 = nullptr;
        ID3DX11EffectMatrixVariable* m_WorldViewProjectionMatrixPtr = nullptr;
        
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;
        uint32_t m_NumIndices = 0;
    };
}
