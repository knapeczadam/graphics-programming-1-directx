//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
float4x4  gWorldViewProj  : WorldViewProjection;

Texture2D gDiffuseMap     : DiffuseMap;
Texture2D gNormalMap      : NormalMap;
Texture2D gSpecularMap    : SpecularMap;
Texture2D gGlossMap       : GlossMap;

float     gTime           : Time;
float3    gCameraPos      : CameraPos;
bool      gUseNormalMap   : UseNormalMap;
int       gShadingMode    : ShadingMode;

float3    gAmbientColor   : AmbientColor;
float3    gLightDir       : LightDir;
float     gLightIntensity : LightIntensity;
float     gKD             : KD;
float     gShininess      : Shininess;

#define PI 3.1415926535897932384626433832795
#define DEG_TO_RAD 0.01745329251994329576923690768489
#define ROTATION_ANGLE -45.0f 

//---------------------------------------------------------------------------
// Sampler States: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_sampler_desc
//---------------------------------------------------------------------------
SamplerState samPoint
{
    Filter   = MIN_MAG_MIP_POINT; // MIN_MAG_MIP_POINT = 0, MIN_MAG_POINT_MIP_LINEAR = 0x1, MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4, etc.,  MIN_MAG_MIP_LINEAR by default
    AddressU = WRAP; // WRAP = 1, MIRROR = 2, CLAMP = 3, BORDER = 4, MIRROR_ONCE = 5, CLAMP by default
    AddressV = Wrap; // WRAP = 1, MIRROR = 2, CLAMP = 3, BORDER = 4, MIRROR_ONCE = 5, CLAMP by default
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
// Blending States: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_render_target_blend_desc
//---------------------------------------------------------------------------
BlendState gAlphaBlendState
{
    BlendEnable[0]           = TRUE;          // FALSE, FALSE by default
    SrcBlend                 = SRC_ALPHA;     // ZERO = 1, ONE = 2, SRC_COLOR = 3, INV_SRC_COLOR = 4, SRC_ALPHA = 5, INV_SRC_ALPHA = 6, DEST_ALPHA = 7, INV_DEST_ALPHA = 8, DEST_COLOR = 9, INV_DEST_COLOR = 10, SRC_ALPHA_SAT = 11, BLEND_FACTOR = 14, INV_BLEND_FACTOR = 15, SRC1_COLOR = 16, INV_SRC1_COLOR = 17, SRC1_ALPHA = 18, INV_SRC1_ALPHA = 19, ONE by default
    DestBlend                = INV_SRC_ALPHA; // ZERO = 1, ONE = 2, SRC_COLOR = 3, INV_SRC_COLOR = 4, SRC_ALPHA = 5, INV_SRC_ALPHA = 6, DEST_ALPHA = 7, INV_DEST_ALPHA = 8, DEST_COLOR = 9, INV_DEST_COLOR = 10, SRC_ALPHA_SAT = 11, BLEND_FACTOR = 14, INV_BLEND_FACTOR = 15, SRC1_COLOR = 16, INV_SRC1_COLOR = 17, SRC1_ALPHA = 18, INV_SRC1_ALPHA = 19, ZERO by default
    BlendOp                  = ADD;           // ADD = 1, SUBTRACT = 2, REV_SUBTRACT = 3, MIN = 4, MAX = 5, ADD by default
    SrcBlendAlpha            = ZERO;          // ZERO = 1, ONE = 2, SRC_COLOR = 3, INV_SRC_COLOR = 4, SRC_ALPHA = 5, INV_SRC_ALPHA = 6, DEST_ALPHA = 7, INV_DEST_ALPHA = 8, DEST_COLOR = 9, INV_DEST_COLOR = 10, SRC_ALPHA_SAT = 11, BLEND_FACTOR = 14, INV_BLEND_FACTOR = 15, SRC1_COLOR = 16, INV_SRC1_COLOR = 17, SRC1_ALPHA = 18, INV_SRC1_ALPHA = 19, ONE by default
    DestBlendAlpha           = ZERO;          // ZERO = 1, ONE = 2, SRC_COLOR = 3, INV_SRC_COLOR = 4, SRC_ALPHA = 5, INV_SRC_ALPHA = 6, DEST_ALPHA = 7, INV_DEST_ALPHA = 8, DEST_COLOR = 9, INV_DEST_COLOR = 10, SRC_ALPHA_SAT = 11, BLEND_FACTOR = 14, INV_BLEND_FACTOR = 15, SRC1_COLOR = 16, INV_SRC1_COLOR = 17, SRC1_ALPHA = 18, INV_SRC1_ALPHA = 19, ZERO by default
    BlendOpAlpha             = ADD;           // ADD = 1, SUBTRACT = 2, REV_SUBTRACT = 3, MIN = 4, MAX = 5, ADD by default
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState gNoBlendState
{
    BlendEnable[0]           = FALSE;
    SrcBlend                 = ONE;
    DestBlend                = ZERO;
    BlendOp                  = ADD;
    SrcBlendAlpha            = ONE;
    DestBlendAlpha           = ZERO;
    BlendOpAlpha             = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

//---------------------------------------------------------------------------
// Rasterizer States: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_rasterizer_desc
//---------------------------------------------------------------------------
RasterizerState gRasterizerState
{
    FillMode              = SOLID; // WIREFRAME = 2, SOLID = 3, SOLID by default
    CullMode              = NONE;  // NONE = 1, FRONT = 2, BACK = 3, BACK by default
    FrontCounterClockwise = FALSE; // TRUE, FALSE by default
};

//---------------------------------------------------------------------------
// Depth/Stencil States: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_depth_stencil_desc
//---------------------------------------------------------------------------
DepthStencilState gDepthStencilState
{
    DepthEnable    = TRUE;  // FALSE, TRUE by default
    DepthWriteMask = ZERO;  // ZERO = 0, ALL = 1, ALL by default
    DepthFunc      = LESS;  // NEVER = 1, LESS = 2, EQUAL = 3, LEQUAL = 4, GREATER = 5, NOTEQUAL = 6, GEQUAL = 7, ALWAYS = 8, LESS by default
    StencilEnable  = FALSE; // TRUE, FALSE by default
};

DepthStencilState gNoDepthStencilState
{
    DepthEnable    = TRUE;
    DepthWriteMask = ALL;
    DepthFunc      = LESS;
    StencilEnable  = FALSE;
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
    
    // Light direction
    float3 lightDir = normalize(gLightDir);
    
    // Radiance (directional light)
    float3 radiance = float3(1.0f, 1.0f, 1.0f) * gLightIntensity;
    
    // Observed area
    float3 observedArea = saturate(dot(normal, -lightDir));
    
    // Diffuse lighting
    float4 diffuse = diffuseColor * gKD / PI;
    
    // Phong specular lighting
    float3 reflectedLight = reflect(-lightDir, normal);
    float cosAlpha = saturate(dot(reflectedLight, -viewDir));
    float phong =  specularColor * pow(cosAlpha, gloss * gShininess);
    
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
        color = float4(radiance, 1.0f) * (diffuse + phong + float4(gAmbientColor, 1.0f)) * float4(observedArea, 1.0f);
    }
    return color;
}

//---------------------------------------------------------------------------
// Input/Output Structs: https://learn.microsoft.com/en-us/windows/win32/direct3dgetstarted/work-with-shaders-and-shader-resources
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
    float4 Position : SV_POSITION; // SV_POSITION is equivalent to SV_POSITION0
    float3 Color    : COLOR;
    float2 Uv       : TEXCOORD; // TEXCOORD is equivalent to TEXCOORD0
    float3 Normal   : NORMAL;
    float3 Tangent  : TANGENT;
};

//---------------------------------------------------------------------------
// Vertex Shader: https://learn.microsoft.com/en-us/windows/win32/direct3d11/vertex-shader-stage
//---------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    input.Position  = mul(input.Position, CreateRotation(ROTATION_ANGLE * DEG_TO_RAD * gTime));
    output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
    
    input.Normal    = mul(input.Normal, CreateRotation(ROTATION_ANGLE * DEG_TO_RAD * gTime));
    output.Normal   = input.Normal;
    
    input.Tangent   = mul(input.Tangent, CreateRotation(ROTATION_ANGLE * DEG_TO_RAD * gTime));
    output.Tangent  = input.Tangent;
    
    output.Color    = input.Color;
    output.Uv       = input.Uv;

    return output;
}

VS_OUTPUT VS_FireFX(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    input.Position  = mul(input.Position, CreateRotation(ROTATION_ANGLE * DEG_TO_RAD * gTime));
    output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
    
    output.Uv       = input.Uv;

    return output;
}

//---------------------------------------------------------------------------
// Pixel Shader: https://learn.microsoft.com/en-us/windows/win32/direct3d11/pixel-shader-stage#the-pixel-shader
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
    float3 viewDir = normalize(gCameraPos - input.Position.xyz);
    
    float4 diffuseColor  = gDiffuseMap.Sample(samLinear,  input.Uv);
    float3 normalColor   = gNormalMap.Sample(samLinear,   input.Uv).rgb;
    float4 specularColor = gSpecularMap.Sample(samLinear, input.Uv);
    float  gloss         = gGlossMap.Sample(samLinear,    input.Uv).r;
    
    return ShadePixel(input.Normal, input.Tangent, viewDir, diffuseColor, normalColor, specularColor, gloss);
}

float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
    float3 viewDir = normalize(gCameraPos - input.Position.xyz);
    
    float4 diffuseColor  = gDiffuseMap.Sample(samAnisotropic,  input.Uv);
    float3 normalColor   = gNormalMap.Sample(samAnisotropic,   input.Uv).rgb;
    float4 specularColor = gSpecularMap.Sample(samAnisotropic, input.Uv);
    float  gloss         = gGlossMap.Sample(samAnisotropic,    input.Uv).r;
    
    return ShadePixel(input.Normal, input.Tangent, viewDir, diffuseColor, normalColor, specularColor, gloss);
}

float4 PS_FireFX(VS_OUTPUT input) : SV_TARGET
{
    float4 color = gDiffuseMap.Sample(samPoint, input.Uv);
    return color;
}

//---------------------------------------------------------------------------
// Techniques: https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d11-effect-technique-syntax
//---------------------------------------------------------------------------
technique11 DefaultTechnique
{
    //-----------------------------------------------------------------------
    // Passes for the vehicle
    //-----------------------------------------------------------------------
    pass P0 // Point sampling
    {
        SetDepthStencilState( gNoDepthStencilState, 0 );
        SetBlendState( gNoBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Point() ) );
    }
    
    pass P1 // Linear sampling
    {
        SetDepthStencilState( gNoDepthStencilState, 0 );
        SetBlendState( gNoBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Linear() ) );
    }
    
    pass P2 // Anisotropic sampling
    {
        SetDepthStencilState( gNoDepthStencilState, 0 );
        SetBlendState( gNoBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Anisotropic() ) );
    }
    
    //-----------------------------------------------------------------------
    // Passes for the fire
    //-----------------------------------------------------------------------
    pass P3 // With alpha blending
    {
        // SetRasterizerState( gRasterizerState ); // Rasterizer State is controlled by Mesh::SetRasterizerState()
        SetDepthStencilState( gDepthStencilState, 0 );
        SetBlendState( gAlphaBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF ); 
        
        SetVertexShader( CompileShader( vs_5_0, VS_FireFX() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_FireFX() ) );
    }
    
    pass P4 // Without alpha blending
    {
        // SetRasterizerState( gRasterizerState ); // Rasterizer State is controlled by Mesh::SetRasterizerState()
        SetDepthStencilState( gDepthStencilState, 0 );
        SetBlendState( gNoBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        
        SetVertexShader( CompileShader( vs_5_0, VS_FireFX() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_FireFX() ) );
    }
}