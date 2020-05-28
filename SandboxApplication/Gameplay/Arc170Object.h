#pragma once

#include <MxEngine.h>
#include <Core/Components/Update.h>

using namespace MxEngine;

void InitArc(MxObject& object)
{
	object.AddComponent<Update>([](auto& self, float dt)
	{
		auto& object = MxObject::GetByComponent(self);
		object.ObjectTransform.RotateY(-10.0f * dt);
		object.TranslateForward(2.0f * dt);
	});
}

class Arc170Object : public MxObject
{
public:
	inline Arc170Object()
	{
		auto context = Application::Get();

		auto meshRenderer = this->AddComponent<MeshRenderer>();

		this->SetMesh(context->GetCurrentScene().LoadMesh("objects/arc170/arc170.obj", meshRenderer.GetUnchecked()));
		this->ObjectTexture = GraphicFactory::Create<Texture>(ToMxString(FileManager::GetFilePath("objects/arc170/arc170.jpg"_id)));

		this->ObjectTransform.Scale(0.005f);
		this->Translate(10.0f, 1.0f, -10.0f);
	}
};