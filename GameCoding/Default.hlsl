
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

// Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = input.position;
    output.uv = input.uv;
    //output.color = input.color;
    
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