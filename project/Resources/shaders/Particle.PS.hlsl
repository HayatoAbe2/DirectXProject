#include "Particle.hlsli"

struct Material
{
    float32_t4 color;
    int32_t padding;
    float32_t4x4 uvTransform;
    int useTexture;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

ConstantBuffer<Material> gMaterial : register(b0);
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    if (gMaterial.useTexture != 0)
    {
        float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
              
        output.color = gMaterial.color * textureColor * input.color;
    }
    else
    {
        output.color = gMaterial.color * input.color;
    }
    
    return output;
}