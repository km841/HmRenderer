#pragma pack_matrix(row_major)

Texture2D<float4> rwOutput : register(t1);
SamplerState samplerLinear : register(s0);

cbuffer cbImmutable : register(b0)
{
    float2 g_fInvWindowSize;
    int g_iIsOnlyBone;
    int g_iIsOnlyCartilage;
}

cbuffer cbEveryFrame : register(b0)
{
    matrix WVP;
}

struct VSInput
{
    float4 pos : POSITION;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 tex : TEXCOORD0;	
};

PSInput FinalVS(VSInput input)
{
    PSInput output;
    output.pos = mul(WVP, input.pos);
    output.tex = 0.5 * (input.pos + 1);

    return output;
}

float4 FinalPS(PSInput input) : SV_TARGET
{
    return float4(1, 1, 1, 1);
}