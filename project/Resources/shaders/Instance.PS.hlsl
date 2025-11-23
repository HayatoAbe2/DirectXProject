#include "Instance.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
    int useTexture;
};

struct Camera
{
    float32_t3 worldPosition;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

struct PointLight
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
};

struct Light
{
    DirectionalLight directionalLight;
    PointLight pointLights[32];
    uint pointLightCount;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);
ConstantBuffer<Light> gLight : register(b2);
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // マテリアル
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    if (gMaterial.useTexture != 0)
    {
        float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
              
        output.color = gMaterial.color * textureColor;
    }
    else
    {
        output.color = gMaterial.color;
    }
    
    // ライティング
    float32_t3 lighting;
    if (gMaterial.enableLighting != 0)
    {
        // directionalLight
        float NdotL = dot(normalize(input.normal), -gLight.directionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float diffuseDirectionalLight = gLight.directionalLight.color.rgb * cos * gLight.directionalLight.intensity;
            
        float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        float32_t3 halfVector = normalize(-gLight.directionalLight.direction + toEye);
        float NDotH = dot(normalize(input.normal), halfVector);
        float specularPow = pow(saturate(NDotH), gMaterial.shininess);
        float32_t3 specularDirectionalLight = gLight.directionalLight.color.rgb * gLight.directionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
        lighting = diffuseDirectionalLight + specularDirectionalLight;
        
        // pointLight
        for (int i = 0; i < gLight.pointLightCount; i++)
        {
            float32_t3 pointLightDirection = normalize(input.worldPosition - gLight.pointLights[i].position);
            NdotL = dot(normalize(input.normal), -pointLightDirection);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            float diffusePointLight = gLight.pointLights[i].color.rgb * cos * gLight.pointLights[i].intensity;
        
            toEye = normalize(gCamera.worldPosition - input.worldPosition);
            halfVector = normalize(-pointLightDirection + toEye);
            NDotH = dot(normalize(input.normal), halfVector);
            specularPow = pow(saturate(NDotH), gMaterial.shininess);
            float32_t3 specularPointLight = gLight.pointLights[i].color.rgb * gLight.pointLights[i].intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
            
            lighting += diffusePointLight + specularPointLight;
        }
        
        output.color.rgb *= lighting;
    }
    
    return output;
}