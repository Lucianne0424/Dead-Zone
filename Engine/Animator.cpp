#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Resources.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "StructuredBuffer.h"

Animator::Animator() : Component(COMPONENT_TYPE::ANIMATOR)
{
	_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeAnimation");
	_boneFinalMatrix = make_shared<StructuredBuffer>();
}

Animator::~Animator()
{
}

void Animator::FinalUpdate()
{
	_updateTime += DELTA_TIME;

	const AnimClipInfo& animClip = _animClips->at(_clipIndex);
	if (_updateTime >= animClip.duration)
		_updateTime = 0.f;

	if (_blendAnimation)
	{
		_blendUpdateTime -= DELTA_TIME;
		if (_blendUpdateTime < 0.f)
		{
			_blendAnimation = false;
			_blendUpdateTime = 0.f;
		}
	}

	const int32 ratio = static_cast<int32>(animClip.frameCount / animClip.duration);
	_frame = static_cast<int32>(_updateTime * ratio);
	_frame = min(_frame, animClip.frameCount - 1);
	_nextFrame = min(_frame + 1, animClip.frameCount - 1);
	_frameRatio = static_cast<float>(_frame - _frame);
}

void Animator::SetAnimClip(const vector<AnimClipInfo>* animClips)
{
	_animClips = animClips;
}

void Animator::PushData()
{
	uint32 boneCount = static_cast<uint32>(_bones->size());
	if (_boneFinalMatrix->GetElementCount() < boneCount)
		_boneFinalMatrix->Init(sizeof(Matrix), boneCount);

	// Compute Shader
	shared_ptr<Mesh> mesh = GetGameObject()->GetMeshRenderer()->GetMesh();
	mesh->GetBoneFrameDataBuffer(_blendClipIndex)->PushComputeSRVData(SRV_REGISTER::t6);
	mesh->GetBoneFrameDataBuffer(_clipIndex)->PushComputeSRVData(SRV_REGISTER::t8);
	mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

	_boneFinalMatrix->PushComputeUAVData(UAV_REGISTER::u0);

	_computeMaterial->SetInt(0, boneCount);
	_computeMaterial->SetInt(1, _frame);
	_computeMaterial->SetInt(2, _nextFrame);
	_computeMaterial->SetFloat(0, _frameRatio);

	_computeMaterial->SetInt(3, _blendFrame);
	_computeMaterial->SetFloat(1, 1 - _blendUpdateTime / 0.25f);
	if (_blendAnimation)
		_computeMaterial->SetFloat(2, 1.f);
	else
		_computeMaterial->SetFloat(2, -1.f);

	uint32 groupCount = (boneCount / 256) + 1;
	_computeMaterial->Dispatch(groupCount, 1, 1);

	// Graphics Shader
	_boneFinalMatrix->PushGraphicsData(SRV_REGISTER::t7);
}

void Animator::Play(uint32 idx)
{
	assert(idx < _animClips->size());
	_blendClipIndex = _clipIndex;
	_clipIndex = idx;
	_updateTime = 0.f;

	_blendUpdateTime = 0.25f;
	_blendAnimation = true;
	_blendFrame = _frame;
}