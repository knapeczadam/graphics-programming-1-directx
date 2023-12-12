#include "pch.h"

#include "Mesh.h"
#include "Effect.h"
#include "SceneSelector.h"

#include <cassert>

namespace dae
{
    Mesh::Mesh(ID3D11Device* devicePtr, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
        : m_DevicePtr{devicePtr},
          m_Vertices{vertices},
          m_Indices{indices}
    {
        InitializeEffect();
        
        m_TechniquePtr = m_EffectPtr->GetTechniqueByName("DefaultTechnique");
        
        if (not m_TechniquePtr->IsValid())
            assert(false and "Failed to create technique!");

        InitializeMatrix();


        m_DevicePtr->GetImmediateContext(&m_DeviceContextPtr);

        // Create Vertex Layout
        //=======================================================================================================
#if W2 and TODO_1
        static constexpr uint32_t numElements{3};
#else
        static constexpr uint32_t numElements{2};
#endif
        
        D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

        vertexDesc[0].SemanticName      = "POSITION";
        vertexDesc[0].Format            = DXGI_FORMAT_R32G32B32_FLOAT;
        vertexDesc[0].AlignedByteOffset = 0;
        vertexDesc[0].InputSlotClass    = D3D11_INPUT_PER_VERTEX_DATA;

        vertexDesc[1].SemanticName      = "COLOR";
        vertexDesc[1].Format            = DXGI_FORMAT_R32G32B32_FLOAT;
        vertexDesc[1].AlignedByteOffset = 12;
        vertexDesc[1].InputSlotClass    = D3D11_INPUT_PER_VERTEX_DATA;
        
#if W2
#if TODO_1
        vertexDesc[2].SemanticName      = "TEXCOORD";
        vertexDesc[2].Format            = DXGI_FORMAT_R32G32_FLOAT;
        vertexDesc[2].AlignedByteOffset = 24;
        vertexDesc[2].InputSlotClass    = D3D11_INPUT_PER_VERTEX_DATA;
#endif
#endif
        
        // Create Input Layout
        //=======================================================================================================
        D3DX11_PASS_DESC passDesc{};
        m_TechniquePtr->GetPassByIndex(0)->GetDesc(&passDesc);

        HRESULT result = m_DevicePtr->CreateInputLayout(
            vertexDesc,
            numElements,
            passDesc.pIAInputSignature,
            passDesc.IAInputSignatureSize,
            &m_InputLayoutPtr);

        if (FAILED(result))
            assert(false and "Failed to create input layout!");

        // Create Vertex Buffer
        //=======================================================================================================
        D3D11_BUFFER_DESC bd{};
        bd.Usage          = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth      = sizeof(Vertex) * static_cast<uint32_t>(m_Vertices.size());
        bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags      = 0;

        D3D11_SUBRESOURCE_DATA initData{};
        initData.pSysMem = m_Vertices.data();

        result = m_DevicePtr->CreateBuffer(&bd, &initData, &m_VertexBufferPtr);

        if (FAILED(result))
            assert(false and "Failed to create vertex buffer!");

        // Create Index Buffer
        //=======================================================================================================
        m_NumIndices = static_cast<uint32_t>(m_Indices.size());
        
        bd.Usage          = D3D11_USAGE_IMMUTABLE;
        bd.ByteWidth      = sizeof(uint32_t)         * m_NumIndices;
        bd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags      = 0;
        
        initData.pSysMem  = m_Indices.data();

        result = m_DevicePtr->CreateBuffer(&bd, &initData, &m_IndexBufferPtr);

        if (FAILED(result))
            assert(false and "Failed to create index buffer!");
    }
    
    void Mesh::InitializeEffect()
    {
        // --- WEEK 1 ---
#if W1
#if TODO_0
        m_EffectPtr = new Effect(m_DevicePtr, L"Resources/PosCol3D_W1_TODO_0.fx");
#endif
        
        // --- WEEK 2 ---
#elif W2
#if TODO_0
        m_EffectPtr = new Effect(m_DevicePtr, L"Resources/PosCol3D_W2_TODO_0.fx");
#elif TODO_1
        m_EffectPtr = new Effect(m_DevicePtr, L"Resources/PosCol3D_W2_TODO_1.fx");
#endif
#endif
    }

    void Mesh::InitializeMatrix()
    {
        // --- WEEK 2 ---
#if W2
#if TODO_0
        m_WorldViewProjectionMatrixPtr = m_EffectPtr->GetVariableByName("gWorldViewProj")->AsMatrix();
        if (not m_WorldViewProjectionMatrixPtr->IsValid())
            assert(false and "Failed to create matrix variable!");
#elif TODO_1
        m_WorldViewProjectionMatrixPtr = m_EffectPtr->GetVariableByName("gWorldViewProj")->AsMatrix();
        if (not m_WorldViewProjectionMatrixPtr->IsValid())
            assert(false and "Failed to create matrix variable!");
#endif
#endif
    }

    Mesh::~Mesh()
    {
        if (m_VertexBufferPtr) m_VertexBufferPtr->Release();
        if (m_InputLayoutPtr)  m_InputLayoutPtr->Release();
        if (m_IndexBufferPtr)  m_IndexBufferPtr->Release();

        delete m_EffectPtr;
    }

    void Mesh::Render() const
    {
        // 1. Set Primitive Topology
        //=======================================================================================================
        m_DeviceContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 2. Set Input Layout
        //=======================================================================================================
        m_DeviceContextPtr->IASetInputLayout(m_InputLayoutPtr);

        // 3. Set VertexBuffer
        //=======================================================================================================
        constexpr UINT stride = sizeof(Vertex);
        constexpr UINT offset = 0;
        m_DeviceContextPtr->IASetVertexBuffers(0, 1, &m_VertexBufferPtr, &stride, &offset);

        // 4. Set IndexBuffer
        //=======================================================================================================
        m_DeviceContextPtr->IASetIndexBuffer(m_IndexBufferPtr, DXGI_FORMAT_R32_UINT, 0);

        // 5. Draw
        //=======================================================================================================
        D3DX11_TECHNIQUE_DESC techDesc{};
        m_TechniquePtr->GetDesc(&techDesc);
        
        for (UINT p = 0; p < techDesc.Passes; ++p)
        {
            m_TechniquePtr->GetPassByIndex(p)->Apply(0, m_DeviceContextPtr);
            m_DeviceContextPtr->DrawIndexed(m_NumIndices, 0, 0);
        }
    }

    void Mesh::UpdateMatrices(const Matrix& viewMatrix, const Matrix& projectionMatrix) const
    {
        const Matrix worldViewProjectionMatrix = viewMatrix * projectionMatrix;
        m_WorldViewProjectionMatrixPtr->SetMatrix(reinterpret_cast<const float*>(&worldViewProjectionMatrix));
    }

    void Mesh::UpdateMatrices(const Matrix& worldMatrix, const Matrix& viewMatrix, const Matrix& projectionMatrix) const
    {
        const Matrix worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
        m_WorldViewProjectionMatrixPtr->SetMatrix(reinterpret_cast<const float*>(&worldViewProjectionMatrix));
    }
}
