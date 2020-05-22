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

	Vector3 vec;
	VectorComponent(float x, float y, float z)
		: vec(x, y, z) { }

	~VectorComponent() { };
};

struct ColorComponent
{
	constexpr static size_t ComponentId = STRING_ID("ColorComponent");

	Vector4 color;
	
	ColorComponent(float r, float g, float b, float a)
		: color(r, g, b, a) { }

	~ColorComponent() { }
};

int main()
{
	UUIDGenerator::Init();
	ComponentFactory::Init();

	{
		ComponentManager entity1;
		entity1.AddComponent<VectorComponent>(1.2f, 3.4f, 2.3f);
		entity1.AddComponent<ColorComponent>(0.5f, 0.3f, 0.4f, 0.1f);

		auto vectorComponent = entity1.GetComponent<VectorComponent>();

		std::cout << Format("({}, {}, {})", vectorComponent->vec.x, vectorComponent->vec.y, vectorComponent->vec.z) << std::endl;

		auto color = entity1.GetComponent<ColorComponent>()->color;

		std::cout << Format("({}, {}, {}. {})", color.r, color.g, color.b, color.a) << std::endl;

		entity1.RemoveComponent<VectorComponent>();
		if (!vectorComponent.IsValid() && !entity1.HasComponent<VectorComponent>())
		{
			std::cout << "vector component was removed\n";
		}

		ComponentManager entity2;
		entity2.AddComponent<ColorComponent>(0.1f, 0.1f, 0.2f, 0.4f);

		auto view = ComponentFactory::GetView<ColorComponent>();
		for (auto& component : view)
		{
			auto& color = component.color;
			std::cout << Format("({}, {}, {}. {})", color.r, color.g, color.b, color.a) << std::endl;
		}
	}

	SandboxApp app;
	app.Run();
	return 0;
}