#pragma pack_matrix(row_major)

Texture3D<float> txVolume : register(t0);
Texture2D<float4> txPositionFront : register(t1);
Texture2D<float4> txPositionBack : register(t2);
Texture1D<float4> txTransfunc : register(t3);

SamplerState samplerLinear : register(s0);

RWTexture2D<float4> rwOutput : register(u0); // 최종 결과를 저장할 텍스처

static const uint g_iMaxIterations = 128;
static const uint g_iVolumeSize = 256;
static const float g_fStepSize = sqrt(3.f) / g_iMaxIterations;

cbuffer cbEveryFrame : register(b0)
{
    float2 g_fInvWindowSize;
    int g_iIsOnlyBone;
    int g_iIsOnlyCartilage;
}

[numthreads(8, 8, 1)]
void RayMarchingCS(uint3 threadID : SV_DispatchThreadID)
{
    // 현재 픽셀의 위치
    float2 tex = threadID.xy * g_fInvWindowSize;
    
    // 앞면, 뒷면 위치 가져오기
    float3 pos_front = txPositionFront.Load(int3(threadID.xy, 0)).xyz;
    float3 pos_back = txPositionBack.Load(int3(threadID.xy, 0)).xyz;
    
    float3 dir = normalize(pos_back - pos_front);
    float3 step = g_fStepSize * dir;

    float3 v = pos_front;
    float2 result = float2(0, 0);

    for (uint i = 0; i < g_iMaxIterations; ++i)
    {
        int3 voxelPos = int3(v * g_iVolumeSize); // 볼륨 좌표 변환
        if (any(voxelPos < 0 || voxelPos >= g_iVolumeSize))  // 경계 검사
            break;

        float src = txVolume.Load(int4(voxelPos, 0)).r;

        float2 transfunc = float2(1.0, 1.0);
        if (g_iIsOnlyBone == true)
            transfunc = float2(src, src); // 뼈만 보이도록
        else if (g_iIsOnlyCartilage == true)
            transfunc = float2(1.0 - src, 1.0 - src); // 연골만 보이도록

        result += (1.0 - result.y) * src * transfunc;
        v += step;
    }

    rwOutput[threadID.xy] = float4(result.r, result.r, result.r, result.g);
}
