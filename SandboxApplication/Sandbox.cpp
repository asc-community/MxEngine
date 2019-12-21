#include "Sandbox.h"
#include <MomoEngine/EntryPoint.h>

#include <algorithm>

MomoEngine::IApplication* MomoEngine::GetApplication()
{
	return new SandboxApp();
}

void SandboxApp::OnCreate()
{
	this->ResourcePath = "Resources/";
	ArcObject.Load(ResourcePath + "objects/arc170.obj");
	ArcTexture.Load(ResourcePath + "textures/arc170.jpg");
	CubeObject.Load(ResourcePath + "objects/crate.obj");
	CubeTexture.Load(ResourcePath + "textures/crate.jpg");
	CubeShader.Load(ResourcePath + "shaders/cube_vertex.glsl", ResourcePath + "shaders/cube_fragment.glsl");
	GridShader.Load(ResourcePath + "shaders/line_vertex.glsl", ResourcePath + "shaders/fragment.glsl");
	MeshShader.Load(ResourcePath + "shaders/mesh_vertex.glsl", ResourcePath + "shaders/fragment.glsl");
	ObjectShader.Load(ResourcePath + "shaders/object_vertex.glsl", ResourcePath + "shaders/object_fragment.glsl");

	GenerateBuffers();
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
	auto ModelMatrix = glm::mat4(1.0f);
	auto ProjectionMatrix = glm::perspective(glm::radians(FOV), float(Window.GetWidth()) / float(Window.GetHeight()), 0.1f, 2000.0f);
	auto ViewMatrix = glm::lookAt(
		position,
		position + direction,
		up
	);

	deltaRot += this->TimeDelta * 0.1f;
	if (deltaRot > glm::two_pi<float>()) deltaRot -= glm::two_pi<float>();
	auto RotationMatrix = glm::rotate(ModelMatrix, deltaRot, glm::vec3(0.0f, 1.0f, 0.0f));
	auto ScaleMatrix = glm::scale(ModelMatrix, glm::vec3(0.005f));

	auto MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	CubeShader.SetUniformMat4("MVP", MVP);
	CubeTexture.Bind();
	Renderer::Instance().DrawTrianglesInstanced(CubeObject.GetVertexData(), CubeObject.GetVertexCount(), CubeShader, cubeCount);

	ObjectShader.SetUniformMat4("MVP", MVP * ScaleMatrix * RotationMatrix);
	ArcTexture.Bind();
	Renderer::Instance().DrawTriangles(ArcObject.GetVertexData(), ArcObject.GetVertexCount(), ObjectShader);

	GridShader.SetUniformMat4("MVP", MVP);
	Renderer::Instance().DrawLines(GridObject.GetVertexData(), GridObject.GetVertexCount(), GridShader);

	if (drawMesh)
	{
		MeshShader.SetUniformMat4("MVP", MVP);
		Renderer::Instance().DrawLinesInstanced(CubeObject.GetVertexData(), CubeMeshIBO, MeshShader, cubeCount);
		MeshShader.SetUniformMat4("MVP", MVP * ScaleMatrix * RotationMatrix);
		Renderer::Instance().DrawLines(ArcObject.GetVertexData(), ArcMeshIBO, MeshShader);
	}
}

void SandboxApp::OnDestroy() { }

void SandboxApp::GenerateBuffers()
{
	std::vector<float> cubeInstancedBuffer;
	for (int i = 0; i < cubeCount; i++)
	{
		cubeInstancedBuffer.push_back(5.0f * sin(0.2f * i));
		cubeInstancedBuffer.push_back(0.5f * i);
		cubeInstancedBuffer.push_back(5.0f * cos(0.2f * i));
	}

	int lineCount = 1000;
	std::vector<float> gridBuffer;
	for (int i = 0; i < lineCount; i++)
	{
		auto lineWidth = lineCount - 1;
		gridBuffer.push_back(lineWidth / 2 - i);
		gridBuffer.push_back(0.0f);
		gridBuffer.push_back(-(lineWidth - 1) / 2);

		gridBuffer.push_back(lineWidth / 2 - i);
		gridBuffer.push_back(0.0f);
		gridBuffer.push_back((lineWidth - 1) / 2);

		gridBuffer.push_back(-(lineWidth - 1) / 2);
		gridBuffer.push_back(0.0f);
		gridBuffer.push_back(lineWidth / 2 - i);

		gridBuffer.push_back((lineWidth - 1) / 2);
		gridBuffer.push_back(0.0f);
		gridBuffer.push_back(lineWidth / 2 - i);
	}

	VertexBuffer CubeInstancedVBO(cubeInstancedBuffer);
	VertexBufferLayout CubeInstancedVBL;
	CubeInstancedVBL.PushFloat(3);
	CubeObject.GetVertexData().AddInstancedBuffer(CubeInstancedVBO, CubeInstancedVBL);
	CubeMeshIBO.Load(CubeObject.GeneratePolygonIndicies());
	ArcMeshIBO.Load(ArcObject.GeneratePolygonIndicies());
	GridObject.Load(gridBuffer, (int)lineCount * 4);
}