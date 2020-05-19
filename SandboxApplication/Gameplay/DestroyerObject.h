#pragma once

#include <MxEngine.h>

using namespace MxEngine;

class DestroyerObject : public MxObject
{
public:
	inline DestroyerObject()
	{
		auto context = Application::Get();
		this->SetMesh(context->GetCurrentScene().LoadMesh("objects/destroyer/destroyer.obj"));
		this->ObjectTexture = context->GetCurrentScene().LoadTexture("objects/destroyer/Aluminm5.jpg");

		this->Translate(3.0f, 1.0f, 2.0f);
		this->Translate(-5, 0.0f, 5.0f);
		this->ObjectTransform.Scale(0.0005f);
	}
};