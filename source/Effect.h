#pragma once

namespace dae
{
    class Effect final
    {
    public:
        Effect(ID3D11Device* devicePtr, const std::wstring& assetFile);
        ~Effect();

        Effect(const Effect& other)                = delete;
        Effect(Effect&& other) noexcept            = delete;
        Effect& operator=(const Effect& other)     = delete;
        Effect& operator=(Effect&& other) noexcept = delete;

        ID3DX11EffectTechnique* GetTechniqueByIndex(int index)              const;
        ID3DX11EffectTechnique* GetTechniqueByName(const std::string& name) const;
        ID3DX11EffectVariable*  GetVariableByName(const std::string& name)  const;

        static ID3DX11Effect* LoadEffect(ID3D11Device* devicePtr, const std::wstring& assetFile);

    private:
        ID3DX11Effect* m_EffectPtr = nullptr;
    };
}
