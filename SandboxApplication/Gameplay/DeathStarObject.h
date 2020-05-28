#pragma once

#include <MxEngine.h>

using namespace MxEngine;

class DeathStarObject : public MxObject
{
public:
	inline DeathStarObject()
	{
		auto context = Application::Get();

		auto meshRenderer = this->AddComponent<MeshRenderer>();

		this->SetMesh(context->GetCurrentScene().LoadMesh("objects/death_star/death_star.obj", meshRenderer.GetUnchecked()));
		this->ObjectTexture = GraphicFactory::Create<Texture>(ToMxString(FileManager::GetFilePath("objects/death_star/texture.jpg"_id)));

		this->ObjectTransform.Scale(0.00005f);
		this->ObjectTransform.RotateX(-90.0f);
		this->ObjectTransform.RotateZ(-90.0f);
		this->ObjectTransform.Translate({ -10.0f, 10.0f, 10.0f });
	}

	inline virtual void OnUpdate() override
	{
		float dt = Application::Get()->GetTimeDelta();
		this->ObjectTransform.RotateZ(2.0f * dt);
	}
};