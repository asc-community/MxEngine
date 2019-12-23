#include "Sandbox.h"
#include <MomoEngine/EntryPoint.h>

#include <algorithm>

MomoEngine::IApplication* MomoEngine::GetApplication()
{
	return new SandboxApp();
}

SandboxApp::SandboxApp()
{

}

void SandboxApp::OnCreate()
{
	this->ResourcePath = "Resources/";
	ArcObject          = MakeRef<GLObject>(ResourcePath + "objects/arc170.obj");
	ArcObject.Texture  = MakeRef<Texture>(ResourcePath + "textures/arc170.jpg");
	CubeObject         = MakeRef<GLObject>(ResourcePath + "objects/crate.obj");
	CubeObject.Texture = MakeRef<Texture>(ResourcePath + "textures/crate.jpg");
	GridObject         = MakeRef<GLObject>(ResourcePath + "objects/grid.obj");
	GridObject.Texture = MakeRef<Texture>(ResourcePath + "textures/grid.png");
	CubeShader.Load(ResourcePath + "shaders/cube_vertex.glsl", ResourcePath + "shaders/object_fragment.glsl");
	GridShader.Load(ResourcePath + "shaders/grid_vertex.glsl", ResourcePath + "shaders/grid_fragment.glsl");
	MeshShader.Load(ResourcePath + "shaders/mesh_vertex.glsl", ResourcePath + "shaders/mesh_fragment.glsl");
	ObjectShader.Load(ResourcePath + "shaders/object_vertex.glsl", ResourcePath + "shaders/object_fragment.glsl");

	GenerateBuffers();

	ArcObject.Scale(0.005f);
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

	horizontalAngle -= mouseSpeed * this->TimeDelta * (curPos.x - cursorPos.x);
	verticalAngle   -= mouseSpeed * this->TimeDelta * (curPos.y - cursorPos.y);
	verticalAngle = std::max(-glm::half_pi<float>() + 0.01f, verticalAngle);
	verticalAngle = std::min(glm::half_pi<float>() - 0.01f, verticalAngle);

	cursorPos = curPos;

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	glm::vec3 forward(
		sin(horizontalAngle),
		0.0f,
		cos(horizontalAngle)
	);

	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - glm::half_pi<float>()),
		0.0f,
		cos(horizontalAngle - glm::half_pi<float>())
	);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	if (Window.IsKeyHolded(KeyCode::W))
	{
		position += forward * this->TimeDelta * speed;
	}
	if (Window.IsKeyHolded(KeyCode::S))
	{
		position -= forward * this->TimeDelta * speed;
	}
	if (Window.IsKeyHolded(KeyCode::D))
	{
		position += right * this->TimeDelta * speed;
	}
	if (Window.IsKeyHolded(KeyCode::A))
	{
		position -= right * this->TimeDelta * speed;
	}
	if (Window.IsKeyHolded(KeyCode::SPACE))
	{
		position += up * this->TimeDelta * speed;
	}
	if (Window.IsKeyHolded(KeyCode::LEFT_SHIFT))
	{
		position -= up * this->TimeDelta * speed;
	}
	if (Window.IsKeyPressed(KeyCode::ESCAPE))
	{
		this->CloseApplication();
	}
	if (Window.IsKeyHolded(KeyCode::UP))
	{
		speed *= 1.001f;
	}
	if (Window.IsKeyHolded(KeyCode::DOWN))
	{
		speed *= 1 / 1.001f;
	}
	if (Window.IsKeyPressed(KeyCode::C))
	{
		FOV = 90.0f - FOV;
	}
	if (Window.IsKeyPressed(KeyCode::M))
	{
		Logger::Instance().Debug("Sandbox App", "mesh toggled " + std::string(drawMesh ? "on" : "off"));
		drawMesh = !drawMesh;
	}
	auto ProjectionMatrix = glm::perspective(glm::radians(FOV), float(Window.GetWidth()) / float(Window.GetHeight()), 0.1f, 2000.0f);
	auto ViewMatrix = glm::lookAt(
		position,
		position + direction,
		up
	);

	float deltaRot = 0.1f * TimeDelta;
	ArcObject.RotateY(deltaRot);
	
	auto ViewProjectMatrix = ProjectionMatrix * ViewMatrix;

	CubeShader.SetUniformMat4("MVP", ViewProjectMatrix * CubeObject.Model);
	CubeObject.Texture->Bind();	
	Renderer::Instance().DrawTrianglesInstanced(CubeObject.GetGLObject()->GetVertexData(), CubeObject.GetGLObject()->GetVertexCount(), CubeShader, cubeCount);

	ObjectShader.SetUniformMat4("MVP", ViewProjectMatrix * ArcObject.Model);
	ArcObject.Texture->Bind();
	Renderer::Instance().DrawTriangles(ArcObject.GetGLObject()->GetVertexData(), ArcObject.GetGLObject()->GetVertexCount(), ObjectShader);

	if (drawMesh)
	{
		MeshShader.SetUniformMat4("MVP", ViewProjectMatrix * CubeObject.Model);
		Renderer::Instance().DrawLinesInstanced(CubeObject.GetGLObject()->GetVertexData(), CubeMeshIBO, MeshShader, cubeCount);
		MeshShader.SetUniformMat4("MVP", ViewProjectMatrix * ArcObject.Model);
		Renderer::Instance().DrawLines(ArcObject.GetGLObject()->GetVertexData(), ArcMeshIBO, MeshShader);
	}

	GridShader.SetUniformMat4("MVP", ViewProjectMatrix * GridObject.Model);
	GridObject.Texture->Bind();
	Renderer::Instance().DrawTriangles(GridObject.GetGLObject()->GetVertexData(), GridObject.GetGLObject()->GetVertexCount(), GridShader);
}

void SandboxApp::OnDestroy() { }

void SandboxApp::GenerateBuffers()
{
	std::vector<float> cubesPos;
	for (int i = 0; i < cubeCount; i++)
	{
		cubesPos.push_back(5.0f * sin(0.2f * i));
		cubesPos.push_back(0.5f * i);
		cubesPos.push_back(5.0f * cos(0.2f * i));
	}

	CubesPositions.Load(cubesPos, UsageType::STATIC_DRAW);
	VertexBufferLayout CubeInstancedVBL;
	CubeInstancedVBL.PushFloat(3);
	CubeObject.GetGLObject()->GetVertexData().AddInstancedBuffer(CubesPositions, CubeInstancedVBL);
	CubeMeshIBO.Load(CubeObject.GetGLObject()->GeneratePolygonIndicies());
	ArcMeshIBO.Load(ArcObject.GetGLObject()->GeneratePolygonIndicies());
}