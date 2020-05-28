#pragma once

#include <MxEngine.h>

using namespace MxEngine;

class DestroyerObject : public MxObject
{
public:
	inline DestroyerObject()
	{
		auto context = Application::Get();

		auto meshRenderer = this->AddComponent<MeshRenderer>();

		this->SetMesh(context->GetCurrentScene().LoadMesh("objects/destroyer/destroyer.obj", meshRenderer.GetUnchecked()));
		this->ObjectTexture = GraphicFactory::Create<Texture>(ToMxString(FileManager::GetFilePath("objects/destroyer/Aluminm5.jpg"_id)));

		this->Translate(3.0f, 1.0f, 2.0f);
		this->Translate(-5, 0.0f, 5.0f);
		this->ObjectTransform.Scale(0.0005f);
	}
};