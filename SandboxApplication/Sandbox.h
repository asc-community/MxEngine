#include <MxEngine.h>

using namespace MxEngine;

class SandboxApp : public Application
{
public:
	virtual void OnCreate () override;
	virtual void OnUpdate () override;
	virtual void OnDestroy() override;

	SandboxApp();
};

#include "Utilities/ECS/Component.h"

struct VectorComponent
{
	constexpr static size_t ComponentId = STRING_ID("VectorComponent");

	Vector3 v;
	VectorComponent(float x)
		: v(x)
	{
	}

	~VectorComponent() { };
};

struct ColorComponent
{
	constexpr static size_t ComponentId = STRING_ID("ColorComponent");

	Vector4 c;
	
	ColorComponent(float r, float g, float b, float a)
		: c(r, g, b, a)
	{
	}

	~ColorComponent() { }
};

int main()
{
	UUIDGenerator::Init();
	{
		ComponentManager manager;
		manager.AddComponent<VectorComponent>(1.2f);
		manager.AddComponent<ColorComponent>(0.5f, 0.3f, 0.4f, 0.1f);

		auto vector = manager.GetComponent<VectorComponent>();

		std::cout << Format("({}, {}, {})", vector->v.x, vector->v.y, vector->v.z) << std::endl;

		auto color = manager.GetComponent<ColorComponent>();

		std::cout << Format("({}, {}, {}. {})", color->c.r, color->c.g, color->c.b, color->c.a) << std::endl;

		manager.RemoveComponent<VectorComponent>();
		if (!vector.IsValid() && !manager.HasComponent<VectorComponent>())
		{
			std::cout << "vector component was removed\n";
		}
	}

	SandboxApp app;
	app.Run();
	return 0;
}