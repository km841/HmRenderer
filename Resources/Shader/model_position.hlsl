#pragma pack_matrix(row_major)

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

PSInput ModelPositionVS(VSInput input)
{
	PSInput output;
	output.pos = mul(WVP, input.pos);
	output.tex = 0.5*(input.pos + 1);

	return output;
}

float4 ModelPositionPS(PSInput input) : SV_TARGET
{
	return input.tex;
}
