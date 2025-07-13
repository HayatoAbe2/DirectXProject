cbuffer Material : register(b0)
{
    float4 color;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

PSOutput main()
{
    PSOutput output;
    output.color = color; // 白固定
    return output;
}
