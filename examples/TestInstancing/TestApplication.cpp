#include "TestApplication.h"
#include <Library/Bindings/Bindings.h>
#include <Library/Primitives/Primitives.h>

class BallObject : public Sphere
{
	size_t count = 50000;
	float fadeFactor = 0.995f;
	std::vector<Vector3> impulse = std::vector<Vector3>(count, MakeVector3(0.0f));
public:
	BallObject() 
	{
		float range = 100.0f;

		this->Resize(10);

		this->ObjectTexture = Colors::MakeTexture(Colors::RED);
		this->MakeInstanced(count);

		for (size_t i = 0; i < count; i++)
		{
			auto instance = this->Instanciate();
			instance->Model.SetPosition({
				Random::Get(-range, range),
				Random::Get(5.0f, 2.0f * range + 5.0f),
				Random::Get(-range, range)
			});
			instance->Model.SetScale(Random::Get(0.5f, 10.0f));
		}
	}

	virtual void OnUpdate() override
	{
		auto ctx = Application::Get();
		auto& camera = ctx->GetCurrentScene().Viewport;
		Vector3 cameraPos = camera.GetPosition();
		auto& instances = this->GetInstances();
		for (size_t i = 0; i < count; i++)
		{
			const auto& model = instances[i].Model;
			if (Length(cameraPos - model.GetPosition()) < 5.0f * Length(model.GetScale()))
			{
				impulse[i] += 10.0f * camera.GetMoveSpeed() / model.GetScale() * Normalize(model.GetPosition() - cameraPos);
			}
		}

		for (size_t i = 0; i < count; i++)
		{
			auto& model = instances[i].Model;
			model.Translate(ctx->GetTimeDelta() * impulse[i]);
			if (model.GetPosition().y < model.GetScale().y)
				impulse[i].y = std::abs(impulse[i].y);
			impulse[i] *= fadeFactor;
		}
	}
};

void SnakePath3D::OnCreate()
{
	// add objects here
	this->GetCurrentScene().SetDirectory("Resources/");
	this->GetCurrentScene().AddObject("Grid", MakeUnique<Grid>());
	this->GetCurrentScene().AddObject("Ball", MakeUnique<BallObject>());

	auto& scene = this->GetCurrentScene();
	
	scene.GlobalLight
		.UseAmbientColor ({ 0.3f, 0.3f, 0.3f })
		.UseDiffuseColor ({ 0.8f, 0.8f, 0.8f })
		.UseSpecularColor({ 1.0f, 1.0f, 1.0f })
		.Direction = { 0.7f, 1.0f, 0.7f };
	
	auto camera = MakeUnique<PerspectiveCamera>();
	camera->SetZFar(1000.0f);
	camera->SetAspectRatio((float)this->GetWindow().GetWidth(), (float)this->GetWindow().GetHeight());
	
	auto& controller = scene.Viewport;
	controller.SetCamera(std::move(camera));
	controller.TranslateY(1.0f);
	controller.SetMoveSpeed(5.0f);
	controller.SetRotateSpeed(0.75f);
	controller.SetMoveSpeed(20.0f);

	ConsoleBinding("Console").Bind(KeyCode::GRAVE_ACCENT);
	AppCloseBinding("AppClose").Bind(KeyCode::ESCAPE);
	InputControlBinding("CameraControl", scene.Viewport)
		.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT)
		.BindRotation();
}

void SnakePath3D::OnUpdate()
{

}

void SnakePath3D::OnDestroy()
{

}

SnakePath3D::SnakePath3D()
{
	this->CreateContext();
}