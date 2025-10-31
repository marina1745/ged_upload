//--------------------------------------------------------------------------------------
// Shading constants
//--------------------------------------------------------------------------------------

static const float3 light = float3(1.000000, 0.948336, 0.880797); // Sun color
static const float3 ambient = float3(0.025525, 0.045511, 0.088005); // Sky color
static const float mesh_specularity = 0.2;
static const float shadow_bias = 0.002;

//--------------------------------------------------------------------------------------
// Shader resources
//--------------------------------------------------------------------------------------

Texture2D g_DiffuseTex; // Material albedo for diffuse lighting
Texture2D g_NormalTex;
Texture2D g_SpecularTex;
Texture2D g_GlowTex;
Buffer<float> g_HeightMap;

Texture2D g_ShadowMap;

//--------------------------------------------------------------------------------------
// Constant buffers
//--------------------------------------------------------------------------------------

cbuffer cbConstant
{
    float4 g_LightDir; // Object space
    int g_TerrainRes;
};

cbuffer cbChangesEveryFrame
{
    matrix g_World;
    matrix g_WorldViewProjection;
    matrix g_LightWorldViewProjection;
    matrix g_WorldNormals;
    float4 g_cameraPosWorld;
    float g_Time;
};

cbuffer cbUserChanges
{
};

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------

struct PosTex
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

struct TerrainPSIn
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
    float3 PosLight : LIGHTPOS;
};

struct T3dVertexVSIn
{
    float3 Pos : POSITION; //Position in object space     
    float2 Tex : TEXCOORD; //Texture coordinate     
    float3 Nor : NORMAL; //Normal in object space     
    float3 Tan : TANGENT; //Tangent in object space (not used in Ass. 5) 
};
	
struct T3dVertexPSIn
{
    float4 Pos : SV_POSITION; //Position in clip space     
    float2 Tex : TEXCOORD; //Texture coordinate     
    float3 PosWorld : POSITION; //Position in world space
    float3 PosLight : LIGHTPOS;
    float3 NorWorld : NORMAL; //Normal in world space     
    float3 TanWorld : TANGENT; //Tangent in world space (not used in Ass. 5) 
};

//--------------------------------------------------------------------------------------
// Samplers
//--------------------------------------------------------------------------------------

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samLinearClamp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

SamplerComparisonState ShadowSampler
{
    Filter = COMPARISON_ANISOTROPIC;
    AddressU = MIRROR;
    AddressV = MIRROR;
    ComparisonFunc = LESS;
};

//--------------------------------------------------------------------------------------
// Rasterizer states
//--------------------------------------------------------------------------------------

RasterizerState rsDefault
{
};

RasterizerState rsCullFront
{
    CullMode = Front;
};

RasterizerState rsCullBack
{
    CullMode = Back;
};

RasterizerState rsCullNone
{
    CullMode = None;
};

RasterizerState rsLineAA
{
    CullMode = None;
    AntialiasedLineEnable = true;
};


//--------------------------------------------------------------------------------------
// DepthStates
//--------------------------------------------------------------------------------------
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};

//--------------------------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------------------------

inline float4 dehom(float4 co)
{
    return co / co.w;
}

inline float shadowing(float3 lightPos)
{
    float3 shadow_coord = lightPos * float3(0.5, -0.5, 1) + float3(0.5, 0.5, 0);
    return g_ShadowMap.SampleCmpLevelZero(ShadowSampler, shadow_coord.xy, shadow_coord.z - shadow_bias).r;
}

//--------------------------------------------------------------------------------------
// Shaders
//--------------------------------------------------------------------------------------

float4 TerrainVSPrimitive(uint VertexID : SV_VertexID) : SV_Position
{
    float4 output;
    
    float2 tmp;
    tmp.x = VertexID % g_TerrainRes;
    tmp.y = (int) (VertexID / g_TerrainRes);
	
    tmp /= g_TerrainRes - 1;
	
    output.x = tmp.x - 0.5f;
    output.y = g_HeightMap[VertexID];
    output.z = tmp.y - 0.5f;
    output.w = 1;
    
    output = mul(output, g_WorldViewProjection);
	
    return output;
}

TerrainPSIn TerrainVS(uint VertexID : SV_VertexID)
{
    TerrainPSIn output = (TerrainPSIn) 0;
	
    output.Tex.x = VertexID % g_TerrainRes;
    output.Tex.y = (int) (VertexID / g_TerrainRes);
	
    output.Tex /= g_TerrainRes - 1;
	
    output.Pos.x = output.Tex.x - 0.5f;
    output.Pos.y = g_HeightMap[VertexID];
    output.Pos.z = output.Tex.y - 0.5f;
    output.Pos.w = 1;
    
    output.PosLight = dehom(mul(output.Pos, g_LightWorldViewProjection)).xyz;
    
    output.Pos = mul(output.Pos, g_WorldViewProjection);
	
    return output;
}

float4 TerrainPS(TerrainPSIn i) : SV_Target0
{
    
    float3 normal;
    normal.xz = g_NormalTex.Sample(samAnisotropic, i.Tex).rg * 1.98f - 0.99f;
	// Use max() to fix some shading bugs due to normalmap compression
    normal.y = sqrt(max(1.0f - normal.x * normal.x - normal.z * normal.z, 0.01));
    normal = normalize(mul(float4(normal, 0.0f), g_WorldNormals).xyz);
    
    float shadow = shadowing(i.PosLight);
    float3 diff = g_DiffuseTex.Sample(samAnisotropic, i.Tex).rgb;
    float3 diff_light = saturate(dot(normal, g_LightDir.xyz)) * light * shadow + ambient;

    float tmp = normal.y;
    return float4(diff * diff_light, 1.0f);
}

float4 MeshVSPrimitive(T3dVertexVSIn input) : SV_Position
{
    return mul(float4(input.Pos, 1), g_WorldViewProjection);
}

T3dVertexPSIn MeshVS(T3dVertexVSIn input)
{
    T3dVertexPSIn output;
	
    output.Pos = mul(float4(input.Pos, 1), g_WorldViewProjection);
    output.Tex = input.Tex;
    output.PosWorld = dehom(mul(float4(input.Pos, 1), g_World)).xyz;
    output.PosLight = dehom(mul(float4(input.Pos, 1), g_LightWorldViewProjection)).xyz;
    output.NorWorld = mul(float4(input.Nor, 0), g_WorldNormals).xyz;
    output.TanWorld = mul(float4(input.Tan, 0), g_World).xyz;
	
    return output;
}

float4 MeshPS(T3dVertexPSIn input) : SV_Target0
{
    float3 n = normalize(input.NorWorld);
    float3 v = normalize(g_cameraPosWorld.xyz - input.PosWorld);
    float shadow = shadowing(input.PosLight);
	
    float4 diff = g_DiffuseTex.Sample(samAnisotropic, input.Tex);
    float4 spec = g_SpecularTex.Sample(samAnisotropic, input.Tex);
    float4 glow = g_GlowTex.Sample(samAnisotropic, input.Tex);
	
    float3 diff_light = saturate(dot(n, g_LightDir.xyz)) * light * shadow + ambient;
	// Reflections not only reflect lightsources
    float3 spec_light = pow(saturate(dot(reflect(g_LightDir.xyz * -1, n), v)), 128) * light * shadow + ambient;
	
    float3 diffuse = diff.rgb * diff_light;
    float3 specular = spec.rgb * spec_light;
	
    return float4(diffuse * (1.0f - mesh_specularity) + specular * mesh_specularity + glow.rgb, 1);

}

PosTex FullscreenVS(uint VertexID : SV_VertexID)
{
    PosTex output = (PosTex) 0;
      
    switch (VertexID)
    {
        case 0:
            output.Pos = float4(-1, 1, 0, 1);
            output.Tex = float2(0, 0);
            return output;
        case 1:
            output.Pos = float4(1, 1, 0, 1);
            output.Tex = float2(1, 0);
            return output;
        case 2:
            output.Pos = float4(-1, -1, 0, 1);
            output.Tex = float2(0, 1);
            return output;
        case 3:
            output.Pos = float4(1, -1, 0, 1);
            output.Tex = float2(1, 1);
            return output;
    }
    
    return output;
}

float4 DebugDepthPS(PosTex input) : SV_Target0
{   
    return float4(g_ShadowMap.Sample(samAnisotropic, input.Tex).rrr * 1, 1);
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique11 Render
{
    pass P0_Terrain
    {
        SetVertexShader(CompileShader(vs_4_0, TerrainVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, TerrainPS()));
        
        SetRasterizerState(rsCullNone);
        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass P1_Mesh
    {
        SetVertexShader(CompileShader(vs_4_0, MeshVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, MeshPS()));
        
        SetRasterizerState(rsCullBack);
        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass P0_Terrain_Shadow
    {
        SetVertexShader(CompileShader(vs_4_0, TerrainVSPrimitive()));
        SetGeometryShader(NULL);
        SetPixelShader(NULL);
        
        SetRasterizerState(rsCullNone);
        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass P1_Mesh_Shadow
    {
        SetVertexShader(CompileShader(vs_4_0, MeshVSPrimitive()));
        SetGeometryShader(NULL);
        SetPixelShader(NULL);
        
        SetRasterizerState(rsCullBack);
        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass P2_Debug
    {
        SetVertexShader(CompileShader(vs_4_0, FullscreenVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, DebugDepthPS()));
        
        SetRasterizerState(rsCullNone);
        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}
