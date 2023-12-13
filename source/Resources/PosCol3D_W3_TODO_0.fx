//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
float4x4  gWorldViewProj : WorldViewProjection;

Texture2D gDiffuseMap    : DiffuseMap;
Texture2D gNormalMap     : NormalMap;
Texture2D gSpecularMap   : SpecularMap;
Texture2D gGlossMap      : GlossMap;

float     gTime          : Time;
float3    gCameraPos     : CameraPos;
bool      gUseNormalMap  : UseNormalMap;
int       gShadingMode   : ShadingMode;

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

float4 ShadePixel(float3 normal, float3 tangent, float3 viewDir, float4 diffuseColor, float3 normalColor, float4 specularColor, float gloss)
{
    float4 color = (float4)0;
    
    // Binormal
    float3 binormal = cross(normal, tangent);
    
    // Tangent-space transformation matrix
    float3x3 tangentSpace = float3x3(tangent, binormal, normal);
    
    // Remap normal from [0, 1] to [-1, 1]
    normalColor = normalColor * 2.0f - 1.0f;
    
    // Transform normal from tangent-space to world-space
    normal = gUseNormalMap ? mul(normalColor, tangentSpace) : normal;
    
    // Ambient lighting
    float3 ambient = float3(0.03f, 0.03f, 0.03f);
    
    // Normalized light direction
    float3 lightDir = float3(0.577f, -0.577f, 0.577f);
    
    // Observed area
    float3 observedArea = saturate(dot(normal, -lightDir));
    
    // Diffuse lighting
    float kd = 7.0f;
    float4 diffuse = diffuseColor * kd / PI;
    
    // Phong specular lighting
    float shininess = 25.0f;
    float3 reflectedLight = reflect(-lightDir, normal);
    float cosAlpha = saturate(dot(reflectedLight, -viewDir));
    float phong =  specularColor * pow(cosAlpha, gloss * shininess);
    
    if (gShadingMode == 0)
    {
        color = float4(observedArea, 1.0f);
    } 
    else if (gShadingMode == 1)
    {
        color = diffuse * float4(observedArea, 1.0f);
    }
    else if (gShadingMode == 2)
    {
        color = phong * float4(observedArea, 1.0f);
    }
    else
    {
        color = (diffuse + phong + float4(ambient, 1.0f)) * float4(observedArea, 1.0f);
    }
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
    float3 Tangent  : TANGENT;
};

//---------------------------------------------------------------------------
// Vertex Shader
//---------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    input.Position  = mul(input.Position, CreateRotation(gTime));
    output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
    
    input.Normal    = mul(input.Normal, CreateRotation(gTime));
    output.Normal   = input.Normal;
    
    input.Tangent   = mul(input.Tangent, CreateRotation(gTime));
    output.Tangent  = input.Tangent;
    
    output.Color    = input.Color;
    output.Uv       = input.Uv;

    return output;
}

//---------------------------------------------------------------------------
// Pixel Shader
//---------------------------------------------------------------------------
float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    float3 viewDir = normalize(gCameraPos - input.Position.xyz);
    
    float4 diffuseColor  = gDiffuseMap.Sample(samPoint,  input.Uv);
    float3 normalColor   = gNormalMap.Sample(samPoint,   input.Uv).rgb;
    float4 specularColor = gSpecularMap.Sample(samPoint, input.Uv);
    float  gloss         = gGlossMap.Sample(samPoint,    input.Uv).r;
    
    return ShadePixel(input.Normal, input.Tangent, viewDir, diffuseColor, normalColor, specularColor, gloss);
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