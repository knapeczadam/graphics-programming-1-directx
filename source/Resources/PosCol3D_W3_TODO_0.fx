//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
float4x4  gWorldViewProj : WorldViewProjection;

Texture2D gDiffuseMap    : DiffuseMap;
Texture2D gNormalMap     : NormalMap;
Texture2D gSpecularMap   : SpecularMap;
Texture2D gGlossMap      : GlossMap;

float     gTime          : Time;

#define PI 3.1415926535897932384626433832795

//---------------------------------------------------------------------------
// Sampler States
//---------------------------------------------------------------------------
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT; // or LINEAR, ANISOTROPIC, COMPARISON_MIN_MAG_MIP_POINT, etc.
    AddressU = Wrap; // or Mirror, Clamp, Border, MirrorOnce
    AddressV = Wrap; // or Mirror, Clamp, Border, MirrorOnce
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

//---------------------------------------------------------------------------
// Helper Functions
//---------------------------------------------------------------------------
float4x4 CreateRotation(float yaw)
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

float4 ShadePixel(float3 normal, float4 diffuseColor)
{
    float4 color = (float4)0;
    
    // Normalized light direction
    float3 lightDir = float3(0.577f, -0.577f, 0.577f);
    
    // Observed area
    float3 observedArea = saturate(dot(normal, -lightDir));
    
    // Diffuse lighting
    float kd = 7.0f;
    float4 diffuse = diffuseColor * kd / PI;
    
    color = diffuse * float4(observedArea, 1.0f);
    return color;
}

//---------------------------------------------------------------------------
// Input/Output Structs
//---------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color    : COLOR;
    float2 Uv       : TEXCOORD;
    float3 Normal   : NORMAL;
    float3 Tangent  : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color    : COLOR;
    float2 Uv       : TEXCOORD;
    float3 Normal   : NORMAL;
};

//---------------------------------------------------------------------------
// Vertex Shader
//---------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    input.Position  = mul(input.Position, CreateRotation(gTime));
    output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
    output.Color    = input.Color;
    output.Uv       = input.Uv;
    input.Normal    = mul(input.Normal, CreateRotation(gTime));
    output.Normal   = input.Normal;

    return output;
}

//---------------------------------------------------------------------------
// Pixel Shader
//---------------------------------------------------------------------------
float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    float4 diffuseColor = gDiffuseMap.Sample(samPoint, input.Uv);
    return ShadePixel(input.Normal, diffuseColor);
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