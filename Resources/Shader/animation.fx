#ifndef _ANIMATION_FX_
#define _ANIMATION_FX_

#include "params.fx"
#include "utils.fx"
#include "math.fx"

struct AnimFrameParams
{
    float4 scale;
    float4 rotation;
    float4 translation;
};

StructuredBuffer<AnimFrameParams> g_blend_bone_frame : register(t6);
StructuredBuffer<AnimFrameParams> g_bone_frame : register(t8);
StructuredBuffer<matrix> g_offset : register(t9);
RWStructuredBuffer<matrix> g_final : register(u0);

// ComputeAnimation
// g_int_0 : BoneCount
// g_int_1 : CurrentFrame
// g_int_2 : NextFrame
// g_float_0 : Ratio
[numthreads(256, 1, 1)]
void CS_Main(int3 threadIdx : SV_DispatchThreadID)
{
    if (g_int_0 <= threadIdx.x)
        return;

    int boneCount = g_int_0;
    int currentFrame = g_int_1;
    int nextFrame = g_int_2;
    float ratio = g_float_0;
    
    int blendFrame = g_int_3;
    float blendRatio = g_float_1;
    float checkBlend = g_float_2;

    uint idx = (boneCount * currentFrame) + threadIdx.x;
    uint nextIdx = (boneCount * nextFrame) + threadIdx.x;
    
    uint blendIdx = (boneCount * blendFrame) + threadIdx.x;

    float4 quaternionZero = float4(0.f, 0.f, 0.f, 1.f);

    float4 scale = lerp(g_bone_frame[idx].scale, g_bone_frame[nextIdx].scale, ratio);
    float4 rotation = QuaternionSlerp(g_bone_frame[idx].rotation, g_bone_frame[nextIdx].rotation, ratio);
    float4 translation = lerp(g_bone_frame[idx].translation, g_bone_frame[nextIdx].translation, ratio);

    if (checkBlend > 0.f)
    {
        float4 blendScale = g_blend_bone_frame[blendIdx].scale;
        float4 blendRotation = g_blend_bone_frame[blendIdx].rotation;
        float4 blendTranslation = g_blend_bone_frame[blendIdx].translation;
    
        float4 finalScale = lerp(blendScale, scale, blendRatio);
        float4 finalRotation = QuaternionSlerp(blendRotation, rotation, blendRatio);
        float4 finalTranslation = lerp(blendTranslation, translation, blendRatio);
        
        matrix matBone = MatrixAffineTransformation(finalScale, quaternionZero, finalRotation, finalTranslation);
        g_final[threadIdx.x] = mul(g_offset[threadIdx.x], matBone);
    }
    else
    {
        matrix matBone = MatrixAffineTransformation(scale, quaternionZero, rotation, translation);
        g_final[threadIdx.x] = mul(g_offset[threadIdx.x], matBone);
    }
}

#endif