struct PSOutput
{
    float4 color : SV_TARGET;
};

PSOutput main()
{
    PSOutput output;
    output.color = float4(1, 1, 1, 1); // 白固定
    return output;
}
