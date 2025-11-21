#include "object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
    int useTexture;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
    int32_t lightingType;
};

struct Camera
{
    float32_t3 worldPosition;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
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
        
        if (textureColor.a < 0.1f)
        {
            discard;
        }
        
        output.color = gMaterial.color * textureColor;
    }
    else
    {
        output.color = gMaterial.color;
    }
    
    if (gMaterial.enableLighting != 0)
    {
        if (gDirectionalLight.lightingType == 0)
        {
            float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
            output.color.rgb *= gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
            
            float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
            float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
            float RdotE = dot(reflectLight, toEye);
            float specularPow = pow(saturate(RdotE), gMaterial.shininess);
            float32_t3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
            output.color.rgb += specular;
        }
        else
        {
            float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            output.color.rgb *= gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
            
            float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
            float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
            float NDotH = dot(normalize(input.normal), halfVector);
            float specularPow = pow(saturate(NDotH), gMaterial.shininess);
            float32_t3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
            output.color.rgb += specular;
        }
        if (output.color.a == 0.0f)
        {
            discard;
        }
    }
    
    return output;
}