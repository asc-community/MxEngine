#include "Sandbox.h"

#include <algorithm>
#include <array>

MomoEngine::IApplication* MomoEngine::GetApplication()
{
	return new SandboxApp();
}

SandboxApp::SandboxApp()
{
	//CreateDefaultContext();
	this->ResourcePath = "Resources/";

	this->Window
		.UseProfile(3, 3, Profile::CORE)
		.UseCursorMode(CursorMode::DISABLED)
		.UseSampling(4)
		.UseDoubleBuffering(false)
		.UseTitle("MomoEngine Project")
		.UsePosition(600, 300)
		.Create();

	this->GetRenderer()
		.UseAnisotropicFiltering(this->GetRenderer().GetLargestAnisotropicFactor())
		.UseDepthBuffer()
		.UseCulling()
		.UseSampling()
		.UseTextureMagFilter(MagFilter::NEAREST)
		.UseTextureMinFilter(MinFilter::LINEAR_MIPMAP_LINEAR)
		.UseTextureWrap(WrapType::REPEAT, WrapType::REPEAT)
		.UseBlending(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA)
		.UseClearColor(0.0f, 0.0f, 0.0f);
}

void SandboxApp::OnCreate()
{
	this->GetRenderer().MeshShader = MakeRef<Shader>(
		ResourcePath + "shaders/mesh_vertex.glsl", 
		ResourcePath + "shaders/mesh_fragment.glsl");

	this->GetRenderer().ObjectShader = MakeRef<Shader>(
		ResourcePath + "shaders/object_vertex.glsl", 
		ResourcePath + "shaders/object_fragment.glsl");

	DestroyerObject = MakeRef<GLObject>(ResourcePath + "objects/destroyer/destroyer.obj");
	DeathStarObject = MakeRef<GLObject>(ResourcePath + "objects/death_star/death_star.obj");
	ArcObject       = MakeRef<GLObject>(ResourcePath + "objects/arc170/arc170.obj");
	CubeObject      = MakeRef<GLObject>(ResourcePath + "objects/crate/crate.obj");
	GridObject      = MakeRef<GLObject>(ResourcePath + "objects/grid/grid.obj");

	GridObject.Shader = MakeRef<Shader>(ResourcePath + "shaders/grid_vertex.glsl", ResourcePath + "shaders/grid_fragment.glsl");
	CubeObject.Shader = MakeRef<Shader>(ResourcePath + "shaders/cube_vertex.glsl", ResourcePath + "shaders/object_fragment.glsl");

	CubeObject.GetGLObject()->AddInstanceBuffer([](int idx, int coord)
		{
			return std::array<float, 3>({ 5.0f * std::sin(0.2f * idx), 0.5f * idx, 5.0f * std::cos(0.2f * idx) })[coord];
		}, 3, cubeCount);

	ArcObject
		.Scale(0.005f)
		.Translate(10.0f, 0.0f, -10.0f);
	DeathStarObject
		.Scale(0.00005f)
		.RotateX(glm::radians(-90.0f))
		.RotateZ(glm::radians(-90.0f))
		.Translate(-10.0f, 10.0f, -10.0f);

	DestroyerObject.Translate(22000.0f, 0.0f, 20000.0f);
	
	Camera& camera = this->GetRenderer().Camera;
	camera.SetZFar(100000.0f);
	camera.SetFOV(65.0f);
	camera.SetAspectRatio(Window.GetWidth(), Window.GetHeight());
	camera.Translate(1.0f, 3.0f, 0.0f);
}

void SandboxApp::OnUpdate()
{
	timePassed += this->TimeDelta;
	if (timePassed > 1.0f)
	{
		this->Window.UseTitle("Sandbox App " + std::to_string(this->CounterFPS) + " FPS");
		timePassed = 0.0f;
	}
	auto curPos = Window.GetCursorPos();
	Camera& camera = Renderer::Instance().Camera;
	camera.Rotate(
		mouseSpeed * this->TimeDelta * (cursorPos.x - curPos.x),
		mouseSpeed * this->TimeDelta * (cursorPos.y - curPos.y)
	);
	cursorPos = curPos;
	// event handling 
	{
		if (Window.IsKeyHolded(KeyCode::W))
		{
			camera.TranslateForward(this->TimeDelta * speed);
		}
		if (Window.IsKeyHolded(KeyCode::S))
		{
			camera.TranslateForward(-this->TimeDelta * speed);
		}
		if (Window.IsKeyHolded(KeyCode::D))
		{
			camera.TranslateRight(this->TimeDelta * speed);
		}
		if (Window.IsKeyHolded(KeyCode::A))
		{
			camera.TranslateRight(-this->TimeDelta * speed);
		}
		if (Window.IsKeyHolded(KeyCode::SPACE))
		{
			camera.TranslateUp(this->TimeDelta * speed);
		}
		if (Window.IsKeyHolded(KeyCode::LEFT_SHIFT))
		{
			camera.TranslateUp(-this->TimeDelta * speed);
		}
		if (Window.IsKeyPressed(KeyCode::ESCAPE))
		{
			this->CloseApplication();
		}
		if (Window.IsKeyHolded(KeyCode::UP))
		{
			speed += this->TimeDelta * speed;
		}
		if (Window.IsKeyHolded(KeyCode::DOWN))
		{
			speed -= this->TimeDelta * speed;
		}
		if (Window.IsKeyPressed(KeyCode::C))
		{
			camera.SetFOV(90.0f - camera.GetFOV());
		}
		if (Window.IsKeyPressed(KeyCode::M))
		{
			Logger::Instance().Debug("Sandbox App", "mesh toggled " + std::string(drawMesh ? "on" : "off"));
			drawMesh = !drawMesh;
		}
	}
	float deltaRot = 0.1f * TimeDelta;
	ArcObject.RotateY(deltaRot);

	this->GetRenderer().DrawInstanced(CubeObject, cubeCount);
	this->GetRenderer().Draw(ArcObject);
	this->GetRenderer().Draw(DeathStarObject);
	this->GetRenderer().Draw(DestroyerObject);

	if (drawMesh)
	{
		this->GetRenderer().DrawObjectMesh(ArcObject);
		this->GetRenderer().DrawObjectMeshInstanced(CubeObject, cubeCount);
		this->GetRenderer().DrawObjectMesh(DeathStarObject);
		this->GetRenderer().DrawObjectMesh(DestroyerObject);
	}

	Renderer::Instance().Draw(GridObject);
}

void SandboxApp::OnDestroy() { }