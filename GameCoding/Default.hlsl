
struct VS_INPUT
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    //float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    //float4 color : COLOR;
};

cbuffer CameraData : register(b0)
{
    row_major matrix matView;
    row_major matrix matProjection;
}

cbuffer TransformData : register(b1)
{
    row_major matrix matWorld;
}

cbuffer AnimationData : register(b2)
{
    float2 spriteOffset;
    float2 spriteSize;
    float2 textureSize;
    float useAnimation;
}

// Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 position = mul(input.position, matWorld); // W
    position = mul(position, matView); // V
    position = mul(position, matProjection); // P
    
    output.position = position;
    output.uv = input.uv;
    
    if (useAnimation == 1.0f)
    {
        output.uv *= spriteSize / textureSize;
        output.uv += spriteOffset / textureSize;
    }
    
    return output;
}

// --------------

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

// Pixel Shader
float4 PS(VS_OUTPUT input) : SV_Target
{    
    float4 color = texture0.Sample(sampler0, input.uv);
    
    return color;
}