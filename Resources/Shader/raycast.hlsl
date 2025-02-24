#pragma pack_matrix(row_major)

Texture3D<float> txVolume : register(t0);
Texture2D<float4> txPositionFront : register(t1);
Texture2D<float4> txPositionBack  : register(t2);
Texture1D<float4> txTransfunc : register(t3);

SamplerState samplerLinear : register(s0);

static const uint g_iMaxIterations = 128;

static const float g_fStepSize = sqrt(3.f)/g_iMaxIterations;

cbuffer cbEveryFrame : register(b0)
{
	matrix WVP;
}

cbuffer cbImmutable : register(b0)
{
	float2 g_fInvWindowSize;
    int g_iIsOnlyBone;
    int g_iIsOnlyCartilage;
}

struct VSInput
{
	float4 pos : POSITION;
};

struct PSInput
{
	float4 pos : SV_POSITION;
};


PSInput RayCastVS(VSInput input)
{
	PSInput output;
	input.pos.w = 1.0f;
	output.pos = mul(WVP, input.pos);
	return output;
}

float4 RayCastPS(PSInput input) : SV_TARGET
{
	float2 tex = input.pos.xy * g_fInvWindowSize;
	float3 pos_front = txPositionFront.Sample(samplerLinear, tex);
	float3 pos_back = txPositionBack.Sample(samplerLinear, tex);
	float3 dir = normalize(pos_back - pos_front);
	float3 step = g_fStepSize * dir;

	float3 v = pos_front;

	float2 result = float2(0, 0);
 
	for (uint i = 0; i < g_iMaxIterations; ++i)
	{
		float2 src = txVolume.Sample(samplerLinear, v).rr;
        float4 tfColor = txTransfunc.Sample(samplerLinear, src.g);
		
        float2 transfunc = float2(0, 0);
		
        if (g_iIsOnlyBone == true)
            transfunc = tfColor.aa;
        else if (g_iIsOnlyCartilage == true)
            transfunc = (float2(1.0, 1.0) - tfColor.aa);
		else 
            transfunc = float2(1.0, 1.0);
		
        result += (1.0 - result.y) * src.y * src * transfunc;
		v += step;
	}
 
    return float4(result.r, result.r, result.r, result.g);
}
