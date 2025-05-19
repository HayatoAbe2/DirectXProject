#include "object3d.hlsli"

struct Material{
    float32_t4 color;
    int32_t enableLighting;
    int useTexture;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    
    if (gMaterial.useTexture != 0){
        float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
        output.color = gMaterial.color * textureColor;
    }
    else{
        output.color = gMaterial.color;
    }
    
    // Lighting計算
    if (gMaterial.enableLighting != 0)
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color *= gDirectionalLight.color * cos * gDirectionalLight.intensity;
    }
    
    return output;
}