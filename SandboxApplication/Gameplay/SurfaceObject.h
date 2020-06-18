#pragma once

#include <MxEngine.h>

using namespace MxEngine;

void InitSurface(MxObject& surface)
{
    auto transform = surface.GetComponent<Transform>();

	surface.Name = "Surface";
	surface.AddComponent<MeshSource>(
		Primitives::CreateSurface([](float x, float y)
		{
			return std::sin(10.0f * x) * std::sin(10.0f * y);
		}, 1.0f, 1.0f, 0.01f)
	);

	transform->Scale(MakeVector3(10.0f, 2.0f, 10.0f));
	transform->Translate(MakeVector3(10.0f, 3.0f, 10.0f));
}