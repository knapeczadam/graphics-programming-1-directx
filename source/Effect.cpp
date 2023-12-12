#include "pch.h"
#include "Effect.h"

namespace dae
{
    Effect::Effect(ID3D11Device* devicePtr, const std::wstring& assetFile)
    {
        m_EffectPtr = LoadEffect(devicePtr, assetFile);
    }

    Effect::~Effect()
    {
        if (m_EffectPtr)    m_EffectPtr->Release();
        if (m_TechniquePtr) m_TechniquePtr->Release();
    }

    ID3DX11EffectTechnique* Effect::GetTechniqueByIndex(int index) const
    {
        return m_EffectPtr->GetTechniqueByIndex(index);
    }

    ID3DX11EffectTechnique* Effect::GetTechniqueByName(const std::string& name) const
    {
        return m_EffectPtr->GetTechniqueByName(name.c_str());
    }

    ID3DX11EffectVariable* Effect::GetVariableByName(const std::string& name) const
    {
        return m_EffectPtr->GetVariableByName(name.c_str());
    }

    ID3DX11Effect* Effect::LoadEffect(ID3D11Device* devicePtr, const std::wstring& assetFile)
    {
        HRESULT result;
        ID3D10Blob* errorBlobPtr = nullptr;
        ID3DX11Effect* effectPtr = nullptr;

        DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
        shaderFlags |= D3DCOMPILE_DEBUG;
        shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        result = D3DX11CompileEffectFromFile(
            assetFile.c_str(),
            nullptr,
            nullptr,
            shaderFlags,
            0,
            devicePtr,
            &effectPtr,
            &errorBlobPtr);

        if (FAILED(result))
        {
            if (errorBlobPtr != nullptr)
            {
                const char* errorsPtr = static_cast<const char*>(errorBlobPtr->GetBufferPointer());

                std::wstringstream ss;
                for (unsigned int i = 0; i < errorBlobPtr->GetBufferSize(); ++i)
                    ss << errorsPtr[i];

                OutputDebugStringW(ss.str().c_str());
                errorBlobPtr->Release();
                errorBlobPtr = nullptr;

                std::wcout << ss.str() << std::endl;
            }
            else
            {
                std::wstringstream ss;
                ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
                std::wcout << ss.str() << std::endl;
                return nullptr;
            }
        }

        return effectPtr;
    }
}
