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
    float radius;
    float decay;
};

struct SpotLight
{
    float32_t4 color;
    float32_t3 position;
    float32_t intensity;
    float32_t3 direction;
    float32_t distance;
    float32_t decay;
    float32_t cosAngle;
    float32_t cosFalloffStart;
};

struct Light
{
    DirectionalLight directionalLight;
    PointLight pointLights[32];
    SpotLight spotLights[16];
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
        float32_t3 diffuseDirectionalLight = gLight.directionalLight.color.rgb * cos * gLight.directionalLight.intensity;
            
        float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        float32_t3 halfVector = normalize(-gLight.directionalLight.direction + toEye);
        float NDotH = dot(normalize(input.normal), halfVector);
        float specularPow = pow(saturate(NDotH), gMaterial.shininess);
        float32_t3 specularDirectionalLight = gLight.directionalLight.color.rgb * gLight.directionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
        lighting = diffuseDirectionalLight + specularDirectionalLight;
        
        // pointLight
        for (int i = 0; i < 32; i++)
        {
            if (gLight.pointLights[i].intensity == 0)
            {
                continue;
            }
            
            float32_t distance = length(gLight.pointLights[i].position - input.worldPosition);
            float32_t factor = pow(saturate(-distance / gLight.pointLights[i].radius + 1.0), gLight.pointLights[i].decay);
            
            float32_t3 pointLightDirection = normalize(input.worldPosition - gLight.pointLights[i].position);
            NdotL = dot(normalize(input.normal), -pointLightDirection);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            float32_t3 diffusePointLight = gLight.pointLights[i].color.rgb * cos * gLight.pointLights[i].intensity * factor;
        
            toEye = normalize(gCamera.worldPosition - input.worldPosition);
            halfVector = normalize(-pointLightDirection + toEye);
            NDotH = dot(normalize(input.normal), halfVector);
            specularPow = pow(saturate(NDotH), gMaterial.shininess);
            float32_t3 specularPointLight = gLight.pointLights[i].color.rgb * gLight.pointLights[i].intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f) * factor;
            
            lighting += diffusePointLight + specularPointLight;
        }
        
        // spotLight
        for (int i = 0; i < 16; i++)
        {
            if (gLight.spotLights[i].intensity == 0)
            {
                continue;
            }
            
            float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - gLight.spotLights[i].position);
            
            // 距離、角度減衰
            float32_t distance = length(gLight.spotLights[i].position - input.worldPosition);
            float32_t attenuationFactor = pow(saturate(-distance / gLight.spotLights[i].distance + 1.0), gLight.spotLights[i].decay);
            float32_t cosAngle = dot(spotLightDirectionOnSurface, gLight.spotLights[i].direction);
            float32_t falloffFactor = saturate((cosAngle - gLight.spotLights[i].cosAngle) / (gLight.spotLights[i].cosFalloffStart - gLight.spotLights[i].cosAngle));
            float32_t3 factor = attenuationFactor * falloffFactor;
            
            // diffuse
            NdotL = dot(normalize(input.normal), -spotLightDirectionOnSurface);
            cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            float32_t3 diffuseSpotLight = gLight.spotLights[i].color.rgb * cos * gLight.spotLights[i].intensity * factor;
        
            // specular
            toEye = normalize(gCamera.worldPosition - input.worldPosition);
            halfVector = normalize(-spotLightDirectionOnSurface + toEye);
            NDotH = dot(normalize(input.normal), halfVector);
            specularPow = pow(saturate(NDotH), gMaterial.shininess);
            float32_t3 specularSpotLight = gLight.spotLights[i].color.rgb * gLight.spotLights[i].intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f) * factor;
            
            lighting += diffuseSpotLight + specularSpotLight;
        }
        
        output.color.rgb *= lighting;

    }
    
    return output;
}