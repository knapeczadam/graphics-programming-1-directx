//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
float4x4  gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap    : DiffuseMap;
float     gTime          : Time;

//---------------------------------------------------------------------------
// Sampler States
//---------------------------------------------------------------------------
SamplerState samPoint
{
    Filter   = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerState samLinear
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerState samAnisotropic
{
    Filter   = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = WRAP;
};

//---------------------------------------------------------------------------
// Helper Functions
//---------------------------------------------------------------------------
float4x4 CreateRotationMatrix(float yaw)
{
    float4x4 rotation = (float4x4)0;
    
    rotation[0][0] = cos(yaw);
    rotation[0][2] = sin(yaw);
    rotation[1][1] = 1.0f;
    rotation[2][0] = -sin(yaw);
    rotation[2][2] = cos(yaw);
    rotation[3][3] = 1.0f;
    
    return rotation;
}

//---------------------------------------------------------------------------
// Input/Output Structs
//---------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color    : COLOR;
    float2 Uv       : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color    : COLOR;
    float2 Uv       : TEXCOORD;
};

//---------------------------------------------------------------------------
// Vertex Shader
//---------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    input.Position  = mul(float4(input.Position, 1.0f), CreateRotationMatrix(gTime)).xyz;
    output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
    output.Color    = input.Color;
    output.Uv       = input.Uv;

    return output;
}

//---------------------------------------------------------------------------
// Pixel Shader
//---------------------------------------------------------------------------
float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(samPoint, input.Uv);
}

float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(samLinear, input.Uv);
}

float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(samAnisotropic, input.Uv);
}

//---------------------------------------------------------------------------
// Techniques
//---------------------------------------------------------------------------
technique11 DefaultTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Point() ) );
    }
    
    pass P1
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Linear() ) );
    }
    
    pass P2
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Anisotropic() ) );
    }
}