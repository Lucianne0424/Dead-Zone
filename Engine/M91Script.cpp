#include "pch.h"
#include "M91Script.h"
#include "Input.h"
#include "Animator.h"

#include "Scene.h"
#include "SceneManager.h"
#include "Engine.h"
#include "Camera.h"
#include "Transform.h"

void M91Script::Update()
{
	//if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
	//{
	//	int32 count = GetAnimator()->GetAnimCount();
	//	int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

	//	int32 index = (currentIndex + 1) % count;
	//	GetAnimator()->Play(index);
	//}

	//if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
	//{
	//	int32 count = GetAnimator()->GetAnimCount();
	//	int32 currentIndex = GetAnimator()->GetCurrentClipIndex();

	//	int32 index = (currentIndex - 1 + count) % count;
	//	GetAnimator()->Play(index);
	//}

	shared_ptr<Camera> camera = GET_SINGLE(SceneManager)->GetActiveScene()->GetMainCamera();

	Vec3 cameraPos = camera->GetTransform()->GetLocalPosition();
	Vec3 cameraDir = camera->GetTransform()->GetLook();

	cameraPos.x += 30.f;
	cameraPos.y -= 20.f;
	cameraPos.z += 200.f;

	cameraDir.y += 170.f;

	GetTransform()->SetLocalPosition(cameraPos);
	GetTransform()->SetLocalRotation(cameraDir);
}