Texture2DArray g_Sprites[4];

cbuffer cbChangesEveryFrame
{
    matrix g_ViewProjection;
    float3 g_CameraRight;
    float3 g_CameraUp;
}

// IO structs

struct SpriteVertex
{
    float3 pos : POSITION;
    float radius : RADIUS;
    int tex_index : TEX_INDEX;
    float time : TIME;
    float alpha : ALPHA;
};

struct SpriteFragment
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    int tex_index : TEX_INDEX;
    float time : TIME;
    float alpha : ALPHA;
};

// Other Stuff

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

RasterizerState rsCullNone
{
    CullMode = None;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

BlendState BlendMix
{
    BlendEnable[0] = TRUE;
    SrcBlend[0] = SRC_ALPHA;
    SrcBlendAlpha[0] = ONE;
    DestBlend[0] = INV_SRC_ALPHA;
    DestBlendAlpha[0] = INV_SRC_ALPHA;
};

// Shaders

SpriteVertex SpriteVS(in SpriteVertex vertex)
{
    return vertex;
}

[maxvertexcount(4)]
void SpriteGS(point SpriteVertex vertex[1], inout TriangleStream<SpriteFragment> stream)
{
    SpriteFragment sf;
    sf.tex_index = vertex[0].tex_index;
    sf.time = vertex[0].time;
    sf.alpha = vertex[0].alpha;
    
    // Upper left
    sf.pos = mul(float4(vertex[0].pos + (-g_CameraRight + g_CameraUp) * vertex[0].radius, 1), g_ViewProjection);
    sf.uv = float2(0, 0);
    stream.Append(sf);
    
    // Upper right
    sf.pos = mul(float4(vertex[0].pos + (g_CameraRight + g_CameraUp) * vertex[0].radius, 1), g_ViewProjection);
    sf.uv = float2(1,0);
    stream.Append(sf);
    
    // Lower left
    sf.pos = mul(float4(vertex[0].pos + (-g_CameraRight - g_CameraUp) * vertex[0].radius, 1), g_ViewProjection);
    sf.uv = float2(0, 1);
    stream.Append(sf);
    
    // Lower right
    sf.pos = mul(float4(vertex[0].pos + (g_CameraRight - g_CameraUp) * vertex[0].radius, 1), g_ViewProjection);
    sf.uv = float2(1, 1);
    stream.Append(sf);
}

#define TEXTURE_LOOKUP(n) \
    case n: \
        g_Sprites[n].GetDimensions(dims.x, dims.y, dims.z);\
        output = g_Sprites[n].Sample(samAnisotropic, float3(sf.uv, int(dims.z * sf.time))); \
        break;

float4 SpritePS(SpriteFragment sf) : SV_Target0
{   
    float4 output = float4(1,1,0,1);
    int3 dims = int3(0, 0, 0);
    
    switch (sf.tex_index)
    {
        TEXTURE_LOOKUP(0);
        TEXTURE_LOOKUP(1);
        TEXTURE_LOOKUP(2);
        TEXTURE_LOOKUP(3);
    }
    
    output.a *= sf.alpha;
    return output;
}

// Technique

technique11 Render
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, SpriteVS()));
        SetGeometryShader(CompileShader(gs_4_0, SpriteGS()));
        SetPixelShader(CompileShader(ps_4_0, SpritePS()));
        
        SetRasterizerState(rsCullNone);
        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(BlendMix, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}