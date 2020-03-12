#include "TestApplication.h"
#include <Library/Bindings/Bindings.h>
#include <Library/Primitives/Primitives.h>

class BallObject : public Instanced<Sphere>
{
	size_t count = 100000;
	float fadeFactor = 0.995f;
	std::vector<Vector3> positions = std::vector<Vector3>(count);
	std::vector<float> scale = std::vector<float>(count);
	std::vector<Vector3> impulse = std::vector<Vector3>(count, MakeVector3(0.0f));
public:
	BallObject() 
	{
		float range = 100.0f;

		this->Resize(10);

		this->ObjectTexture = Colors::MakeTexture(Colors::RED);
		this->MakeInstanced(count, UsageType::DYNAMIC_DRAW);

		for (size_t i = 0; i < count; i++)
		{
			positions[i].x = Random::Get(-range, range);
			positions[i].y = Random::Get(  5.0f, 2.0f * range + 5.0f);
			positions[i].z = Random::Get(-range, range);
			scale[i] = Random::Get(0.5f, 10.0f);
		}
	}

	virtual void OnUpdate() override
	{
		auto ctx = Application::Get();
		auto& camera = ctx->GetRenderer().ViewPort;
		Vector3 cameraPos = camera.GetPosition();
		for (size_t i = 0; i < count; i++)
		{
			if (Length(cameraPos - positions[i]) < 5.0f * Length(MakeVector3(scale[i])))
			{
				impulse[i] += 10.0f * camera.GetMoveSpeed() / scale[i] * Normalize(positions[i] - cameraPos);
			}
		}

		for (size_t i = 0; i < count; i++)
		{
			positions[i] += ctx->GetTimeDelta() * impulse[i];
			if (positions[i].y < scale[i])
				impulse[i].y = std::abs(impulse[i].y);
			impulse[i] *= fadeFactor;
		}

		this->GenerateInstanceModels([this](int idx)
			{
				Matrix4x4 T = MxEngine::Translate(Matrix4x4(1.0f), positions[idx]);
				return MxEngine::Scale(T, scale[idx]);
			});

		this->BufferInstances();
	}
};

void SnakePath3D::OnCreate()
{
	// add objects here
	this->AddObject("Grid", MakeUnique<Grid>());
	this->AddObject("Ball", MakeUnique<BallObject>());
	this->InvalidateObjects();

	auto& Renderer = this->GetRenderer();

	Renderer.DefaultTexture = Colors::MakeTexture(Colors::WHITE);
	Renderer.ObjectShader = this->CreateShader("shaders/object_vertex.glsl", "shaders/object_fragment.glsl");
	Renderer.MeshShader = this->CreateShader("shaders/mesh_vertex.glsl", "shaders/mesh_fragment.glsl");
	
	Renderer.GlobalLight
		.UseAmbientColor ({ 0.3f, 0.3f, 0.3f })
		.UseDiffuseColor ({ 0.8f, 0.8f, 0.8f })
		.UseSpecularColor({ 1.0f, 1.0f, 1.0f })
		.Direction = { 0.7f, 1.0f, 0.7f };
	
	auto camera = MakeUnique<PerspectiveCamera>();
	camera->SetZFar(1000.0f);
	camera->SetAspectRatio((float)this->GetWindow().GetWidth(), (float)this->GetWindow().GetHeight());
	
	auto& controller = Renderer.ViewPort;
	controller.SetCamera(std::move(camera));
	controller.TranslateY(1.0f);
	controller.SetMoveSpeed(5.0f);
	controller.SetRotateSpeed(0.75f);
	controller.SetMoveSpeed(20.0f);

	ConsoleBinding("Console").Bind(KeyCode::GRAVE_ACCENT);
	AppCloseBinding("AppClose").Bind(KeyCode::ESCAPE);
	InputBinding("CameraControl", Renderer.ViewPort)
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
	this->ResourcePath = "Resources/";
	this->CreateContext();
}