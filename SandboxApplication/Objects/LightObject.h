#pragma once

#include <MxEngine.h>

using namespace MxEngine;

// TODO: make LightObject MxEngine library binding
template<typename LightType>
class LightObject : public MxObject
{
	LightType& light;
public:
	inline LightObject(LightType& light) : light(light)
	{
		auto context = Context::Instance();
		this->Load(context->LoadObjectBase("objects/cube/cube.obj"));
		this->Scale(0.33f);
		auto& material = this->GetObjectBase()->GetRenderObjects().front().GetMaterial();
		material.f_Ka = 0.0f;
		material.f_Kd = 0.0f;
	}

	inline virtual void OnUpdate() override
	{
		this->Translate(light.Position - this->GetTranslation());

		auto& material = this->GetObjectBase()->GetRenderObjects().front().GetMaterial();
		material.Ke = 0.3f * light.GetAmbientColor() + 0.7f * light.GetDiffuseColor();
	}
};