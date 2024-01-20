#include "pch.h"
#include "Mesh.h"

// Project includes
#include "Effect.h"
#include "SceneSelector.h"
#include "Texture.h"

// Standard includes
#include <cassert>

namespace dae
{
#pragma region Initialization
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
        InitializeTextures();
        InitializeScalars();

        m_DevicePtr->GetImmediateContext(&m_DeviceContextPtr);

        // Create Vertex Layout
        //=======================================================================================================
        
        // --- WEEK 1 ---
#if W1
        static constexpr uint32_t numElements{2};
#elif W2
        
        // --- WEEK 2 ---
#if TODO_0
        static constexpr uint32_t numElements{2};
#elif TODO_1
        static constexpr uint32_t numElements{3};
#elif TODO_2
        static constexpr uint32_t numElements{3};
#elif TODO_3
        static constexpr uint32_t numElements{3};
#endif
        
        // --- WEEK 3 ---
#elif W3
#if TODO_0
        static constexpr uint32_t numElements{5};
#endif
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
        
        // --- WEEK 2 ---
#if W2
#if TODO_1
        vertexDesc[2].SemanticName      = "TEXCOORD";
        vertexDesc[2].Format            = DXGI_FORMAT_R32G32_FLOAT;
        vertexDesc[2].AlignedByteOffset = 24;
        vertexDesc[2].InputSlotClass    = D3D11_INPUT_PER_VERTEX_DATA;
#elif TODO_2
        vertexDesc[2].SemanticName      = "TEXCOORD";
        vertexDesc[2].Format            = DXGI_FORMAT_R32G32_FLOAT;
        vertexDesc[2].AlignedByteOffset = 24;
        vertexDesc[2].InputSlotClass    = D3D11_INPUT_PER_VERTEX_DATA;
#elif TODO_3
        vertexDesc[2].SemanticName      = "TEXCOORD";
        vertexDesc[2].Format            = DXGI_FORMAT_R32G32_FLOAT;
        vertexDesc[2].AlignedByteOffset = 24;
        vertexDesc[2].InputSlotClass    = D3D11_INPUT_PER_VERTEX_DATA;
#endif
        
        // --- WEEK 3 ---
#elif W3
#if TODO_0
        vertexDesc[2].SemanticName      = "TEXCOORD";
        vertexDesc[2].Format            = DXGI_FORMAT_R32G32_FLOAT;
        vertexDesc[2].AlignedByteOffset = 24;
        vertexDesc[2].InputSlotClass    = D3D11_INPUT_PER_VERTEX_DATA;

        vertexDesc[3].SemanticName      = "NORMAL";
        vertexDesc[3].Format            = DXGI_FORMAT_R32G32B32_FLOAT;
        vertexDesc[3].AlignedByteOffset = 32;
        vertexDesc[3].InputSlotClass    = D3D11_INPUT_PER_VERTEX_DATA;

        vertexDesc[4].SemanticName      = "TANGENT";
        vertexDesc[4].Format            = DXGI_FORMAT_R32G32B32_FLOAT;
        vertexDesc[4].AlignedByteOffset = 44;
        vertexDesc[4].InputSlotClass    = D3D11_INPUT_PER_VERTEX_DATA;
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
#elif TODO_2
        m_EffectPtr = new Effect(m_DevicePtr, L"Resources/PosCol3D_W2_TODO_2.fx");
#elif TODO_3
        m_EffectPtr = new Effect(m_DevicePtr, L"Resources/PosCol3D_W2_TODO_3.fx");
#endif
        
        // --- WEEK 3 ---
#elif W3
#if TODO_0
        m_EffectPtr = new Effect(m_DevicePtr, L"Resources/PosCol3D_W3_TODO_0.fx");
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
            assert(false and "Failed to create matrix variable: gWorldViewProj!");
#elif TODO_2
        m_WorldViewProjectionMatrixPtr = m_EffectPtr->GetVariableByName("gWorldViewProj")->AsMatrix();
        if (not m_WorldViewProjectionMatrixPtr->IsValid())
            assert(false and "Failed to create matrix variable: gWorldViewProj!");
#elif TODO_3
        m_WorldViewProjectionMatrixPtr = m_EffectPtr->GetVariableByName("gWorldViewProj")->AsMatrix();
        if (not m_WorldViewProjectionMatrixPtr->IsValid())
            assert(false and "Failed to create matrix variable: gWorldViewProj!");
#endif

        // --- WEEK 3 ---
#elif W3
#if TODO_0
        m_WorldViewProjectionMatrixPtr = m_EffectPtr->GetVariableByName("gWorldViewProj")->AsMatrix();
        if (not m_WorldViewProjectionMatrixPtr->IsValid())
            assert(false and "Failed to create matrix variable: gWorldViewProj!");
#endif
#endif
    }

    void Mesh::InitializeTextures()
    {
        // --- WEEK 2 ---
#if W2
#if TODO_1
        m_DiffuseMapVariablePtr = m_EffectPtr->GetVariableByName("gDiffuseMap")->AsShaderResource();
        if (not m_DiffuseMapVariablePtr->IsValid())
            assert(false and "Failed to create texture variable: gDiffuseMap!");
#elif TODO_2
        m_DiffuseMapVariablePtr = m_EffectPtr->GetVariableByName("gDiffuseMap")->AsShaderResource();
        if (not m_DiffuseMapVariablePtr->IsValid())
            assert(false and "Failed to create texture variable: gDiffuseMap!");
#elif TODO_3
        m_DiffuseMapVariablePtr = m_EffectPtr->GetVariableByName("gDiffuseMap")->AsShaderResource();
        if (not m_DiffuseMapVariablePtr->IsValid())
            assert(false and "Failed to create texture variable: gDiffuseMap!");
#endif
        
        // --- WEEK 3 ---
#elif W3
#if TODO_0
        m_DiffuseMapVariablePtr = m_EffectPtr->GetVariableByName("gDiffuseMap")->AsShaderResource();
        if (not m_DiffuseMapVariablePtr->IsValid())
            assert(false and "Failed to create texture variable: gDiffuseMap!");
        
        m_NormalMapVariablePtr = m_EffectPtr->GetVariableByName("gNormalMap")->AsShaderResource();
        if (not m_NormalMapVariablePtr->IsValid())
            assert(false and "Failed to create texture variable: gNormalMap!");

        m_SpecularMapVariablePtr = m_EffectPtr->GetVariableByName("gSpecularMap")->AsShaderResource();
        if (not m_SpecularMapVariablePtr->IsValid())
            assert(false and "Failed to create texture variable: gSpecularMap!");

        m_GlossinessMapVariablePtr = m_EffectPtr->GetVariableByName("gGlossMap")->AsShaderResource();
        if (not m_GlossinessMapVariablePtr->IsValid())
            assert(false and "Failed to create texture variable: gGlossMap!");
#endif
#endif
    }

    void Mesh::InitializeScalars()
    {
        // --- WEEK 2 ---
#if W2
#if TODO_3
        m_TimeVariablePtr = m_EffectPtr->GetVariableByName("gTime")->AsScalar();
        if (not m_TimeVariablePtr->IsValid())
            assert(false and "Failed to create scalar variable: gTime!");
#endif
        
        // --- WEEK 3 ---
#elif W3
#if TODO_0
        m_TimeVariablePtr = m_EffectPtr->GetVariableByName("gTime")->AsScalar();
        if (not m_TimeVariablePtr->IsValid())
            assert(false and "Failed to create scalar variable: gTime!");

        m_CameraPositionVariablePtr = m_EffectPtr->GetVariableByName("gCameraPos")->AsVector();
        if (not m_CameraPositionVariablePtr->IsValid())
            assert(false and "Failed to create scalar variable: gCameraPos!");

        m_UseNormalMapVariablePtr = m_EffectPtr->GetVariableByName("gUseNormalMap")->AsScalar();
        if (not m_UseNormalMapVariablePtr->IsValid())
            assert(false and "Failed to create scalar variable: gUseNormalMap!");

        m_ShadingModeVariablePtr = m_EffectPtr->GetVariableByName("gShadingMode")->AsScalar();
        if (not m_ShadingModeVariablePtr->IsValid())
            assert(false and "Failed to create scalar variable: gShadingMode!");

        m_AmbientVariablePtr = m_EffectPtr->GetVariableByName("gAmbientColor")->AsVector();
        if (not m_AmbientVariablePtr->IsValid())
            assert(false and "Failed to create scalar variable: gAmbientColor!");

        m_LightDirectionVariablePtr = m_EffectPtr->GetVariableByName("gLightDir")->AsVector();
        if (not m_LightDirectionVariablePtr->IsValid())
            assert(false and "Failed to create scalar variable: gLightDir!");

        m_LightIntensityVariablePtr = m_EffectPtr->GetVariableByName("gLightIntensity")->AsScalar();
        if (not m_LightIntensityVariablePtr->IsValid())
            assert(false and "Failed to create scalar variable: gLightIntensity!");

        m_KDVariablePtr = m_EffectPtr->GetVariableByName("gKD")->AsScalar();
        if (not m_KDVariablePtr->IsValid())
            assert(false and "Failed to create scalar variable: gKD!");

        m_ShininessVariablePtr = m_EffectPtr->GetVariableByName("gShininess")->AsScalar();
        if (not m_ShininessVariablePtr->IsValid())
            assert(false and "Failed to create scalar variable: gShininess!");
#endif
#endif
    }
#pragma endregion

#pragma region Cleanup
    Mesh::~Mesh()
    {
        SAFE_RELEASE(m_VertexBufferPtr)
        SAFE_RELEASE(m_InputLayoutPtr)
        SAFE_RELEASE(m_IndexBufferPtr)

        SAFE_RELEASE(m_WorldViewProjectionMatrixPtr)

        // Texture variables
        SAFE_RELEASE(m_DiffuseMapVariablePtr)
        SAFE_RELEASE(m_NormalMapVariablePtr)
        SAFE_RELEASE(m_SpecularMapVariablePtr) 
        SAFE_RELEASE(m_GlossinessMapVariablePtr)

        // Scalar variables
        SAFE_RELEASE(m_TimeVariablePtr)
        SAFE_RELEASE(m_CameraPositionVariablePtr)
        SAFE_RELEASE(m_UseNormalMapVariablePtr)
        SAFE_RELEASE(m_ShadingModeVariablePtr)
        SAFE_RELEASE(m_AmbientVariablePtr)
        SAFE_RELEASE(m_LightDirectionVariablePtr)
        SAFE_RELEASE(m_LightIntensityVariablePtr)
        SAFE_RELEASE(m_KDVariablePtr)
        SAFE_RELEASE(m_ShininessVariablePtr)

        SAFE_RELEASE(m_TechniquePtr)
        
        delete m_EffectPtr;
    }
#pragma endregion

#pragma region Render & Draw
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
        Draw();
    }

    void Mesh::Draw() const
    {
        // --- WEEK 1 ---
#if W1
        D3DX11_TECHNIQUE_DESC techDesc{};
        m_TechniquePtr->GetDesc(&techDesc);
        
        for (UINT p = 0; p < techDesc.Passes; ++p)
        {
            m_TechniquePtr->GetPassByIndex(p)->Apply(0, m_DeviceContextPtr);
            m_DeviceContextPtr->DrawIndexed(m_NumIndices, 0, 0);
        }
        
        // --- WEEK 2 ---
#elif W2
#if TODO_0
        D3DX11_TECHNIQUE_DESC techDesc{};
        m_TechniquePtr->GetDesc(&techDesc);
        
        for (UINT p = 0; p < techDesc.Passes; ++p)
        {
            m_TechniquePtr->GetPassByIndex(p)->Apply(0, m_DeviceContextPtr);
            m_DeviceContextPtr->DrawIndexed(m_NumIndices, 0, 0);
        }
#elif TODO_1
        D3DX11_TECHNIQUE_DESC techDesc{};
        m_TechniquePtr->GetDesc(&techDesc);
        
        for (UINT p = 0; p < techDesc.Passes; ++p)
        {
            m_TechniquePtr->GetPassByIndex(p)->Apply(0, m_DeviceContextPtr);
            m_DeviceContextPtr->DrawIndexed(m_NumIndices, 0, 0);
        }
#elif TODO_2
        LoadPass();
#elif TODO_3
        LoadPass();
#endif
        
        // --- WEEK 3 ---
#elif W3
#if TODO_0
        LoadPass();
#endif
#endif
    }
#pragma endregion

#pragma region Pass
    void Mesh::LoadPass() const
    {
        D3DX11_TECHNIQUE_DESC techDesc;
        m_TechniquePtr->GetDesc(&techDesc);
        if (m_PassIdx < techDesc.Passes)
        {
            m_TechniquePtr->GetPassByIndex(m_PassIdx)->Apply(0, m_DeviceContextPtr);
            m_DeviceContextPtr->DrawIndexed(m_NumIndices, 0, 0);
        }
    }
#pragma endregion

#pragma region Setters
    void Mesh::SetMatrix(const Matrix& viewMatrix, const Matrix& projectionMatrix) const
    {
        const Matrix worldViewProjectionMatrix = viewMatrix * projectionMatrix;
        m_WorldViewProjectionMatrixPtr->SetMatrix(reinterpret_cast<const float*>(&worldViewProjectionMatrix));
    }

    void Mesh::SetMatrix(const Matrix& worldMatrix, const Matrix& viewMatrix, const Matrix& projectionMatrix) const
    {
        const Matrix worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
        m_WorldViewProjectionMatrixPtr->SetMatrix(reinterpret_cast<const float*>(&worldViewProjectionMatrix));
    }

    void Mesh::SetDiffuseMap(const Texture* diffuseTexturePtr) const
    {
        if (diffuseTexturePtr)
            m_DiffuseMapVariablePtr->SetResource(diffuseTexturePtr->GetSRV());
    }

    void Mesh::SetNormalMap(const Texture* normalMapTexturePtr) const
    {
        if (normalMapTexturePtr)
            m_NormalMapVariablePtr->SetResource(normalMapTexturePtr->GetSRV());
    }

    void Mesh::SetSpecularMap(const Texture* specularTexturePtr) const
    {
        if (specularTexturePtr)
            m_SpecularMapVariablePtr->SetResource(specularTexturePtr->GetSRV());
    }

    void Mesh::SetGlossinessMap(const Texture* glossinessTexturePtr) const
    {
        if (glossinessTexturePtr)
            m_GlossinessMapVariablePtr->SetResource(glossinessTexturePtr->GetSRV());
    }

    void Mesh::SetTime(float time) const
    {
        m_TimeVariablePtr->SetFloat(time);
    }

    void Mesh::SetCameraPosition(const Vector3& viewDirection) const
    {
        m_CameraPositionVariablePtr->SetFloatVector(reinterpret_cast<const float*>(&viewDirection));
    }

    void Mesh::SetUseNormalMap(bool useNormalMap) const
    {
        m_UseNormalMapVariablePtr->SetBool(useNormalMap);
    }

    void Mesh::SetShadingMode(int shadingMode) const
    {
        m_ShadingModeVariablePtr->SetInt(shadingMode);
    }

    void Mesh::SetAmbient(float* ambient) const
    {
        m_AmbientVariablePtr->SetFloatVector(ambient);
    }

    void Mesh::SetLightDirection(float* lightDirection) const
    {
        m_LightDirectionVariablePtr->SetFloatVector(lightDirection);
    }

    void Mesh::SetLightIntensity(float lightIntensity) const
    {
        m_LightIntensityVariablePtr->SetFloat(lightIntensity);
    }

    void Mesh::SetKD(float kd) const
    {
        m_KDVariablePtr->SetFloat(kd);
    }

    void Mesh::SetShininess(float shininess) const
    {
        m_ShininessVariablePtr->SetFloat(shininess);
    }

    void Mesh::SetRasterizerState(FillMode fillMode, CullMode cullingMode, bool frontCounterClockwise) const
    {
        ID3D11RasterizerState* rasterizerStatePtr{};
        m_DeviceContextPtr->RSGetState(&rasterizerStatePtr);
        
        D3D11_RASTERIZER_DESC rasterizerDesc{};
        rasterizerDesc.FrontCounterClockwise = frontCounterClockwise ? TRUE : FALSE;
        rasterizerDesc.DepthBias             = 0;
        rasterizerDesc.DepthBiasClamp        = 0.0f;
        rasterizerDesc.SlopeScaledDepthBias  = 0.0f;
        rasterizerDesc.DepthClipEnable       = TRUE;
        rasterizerDesc.ScissorEnable         = FALSE;
        rasterizerDesc.MultisampleEnable     = FALSE;
        rasterizerDesc.AntialiasedLineEnable = FALSE;

        switch (fillMode)
        {
            case FillMode::Solid:
                rasterizerDesc.FillMode = D3D11_FILL_SOLID;
                break;
            case FillMode::Wireframe:
                rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
                break;
            default:
                rasterizerDesc.FillMode = D3D11_FILL_SOLID;
                break;
        }
        
        switch (cullingMode)
        {
            case CullMode::None:
                rasterizerDesc.CullMode = D3D11_CULL_NONE;
                break;
            case CullMode::Front:
                rasterizerDesc.CullMode = D3D11_CULL_FRONT;
                break;
            case CullMode::Back:
                rasterizerDesc.CullMode = D3D11_CULL_BACK;
                break;
            default:
                rasterizerDesc.CullMode = D3D11_CULL_BACK;
                break;
        }
        
        m_DevicePtr->CreateRasterizerState(&rasterizerDesc, &rasterizerStatePtr);
        m_DeviceContextPtr->RSSetState(rasterizerStatePtr);
        
        SAFE_RELEASE(rasterizerStatePtr)
    }
#pragma endregion
}
