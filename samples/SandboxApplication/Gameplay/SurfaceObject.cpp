#include <MxEngine.h>
using namespace MxEngine;

void InitSurface(MxObject& surface)
{
	surface.Name = "Surface";
	surface.AddComponent<MeshSource>(
		Primitives::CreateSurface([](float x, float y)
		{
			return std::sin(10.0f * x) * std::sin(10.0f * y);
		}, 1.0f, 1.0f, 0.01f, "Surface")
	);
	surface.AddComponent<MeshRenderer>();

	surface.LocalTransform
		.Scale(MakeVector3(10.0f, 2.0f, 10.0f))
		.Translate(MakeVector3(10.0f, 5.0f, 10.0f));
}