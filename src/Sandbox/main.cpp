#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <iomanip>

#include "../Core/Renderer/Renderer.h"
#include "../Core/Window/Window.h"
#include "../Core/Interfaces/IApplication.h"

using namespace MomoEngine;

int main()
{
	Window window(1280, 720);
	window
		.UseProfile(3, 3, Profile::CORE)
		.UseCursorMode(CursorMode::DISABLED)
		.UseSampling(4)
		.UseDoubleBuffering(false)
		.UseTitle("MomoEngine Project")
		.UsePosition(600, 300)
		.Create();

	auto renderer = Renderer()
		.UseDepthBuffer()
		.UseCulling()
		.UseSampling()
		.UseTextureMagFilter(MagFilter::NEAREST)
		.UseTextureMinFilter(MinFilter::LINEAR_MIPMAP_LINEAR)
		.UseClearColor(0.0f, 0.0f, 0.0f);

	const std::string ResourcePath = "../src/Resources/";

	int cubeCount = 50;
	std::vector<GLfloat> cubeInstancedBuffer;
	for (int i = 0; i < cubeCount; i++)
	{
		cubeInstancedBuffer.push_back(5.0f * sin(0.2f * i));
		cubeInstancedBuffer.push_back(0.5f * i);
		cubeInstancedBuffer.push_back(5.0f * cos(0.2f * i));
	}

	int lineCount = 1000;
	std::vector<GLfloat> gridBuffer;
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
	GLObject CubeObject(ResourcePath + "objects/crate.obj");
	Texture CubeTexture(ResourcePath + "textures/crate.jpg");
	VertexBuffer CubeInstancedVBO(cubeInstancedBuffer);
	VertexBufferLayout CubeInstancedVBL;
	CubeInstancedVBL.Push<float>(3);
	CubeObject.GetVertexData().AddInstancedBuffer(CubeInstancedVBO, CubeInstancedVBL);
	IndexBuffer CubeMeshIBO(CubeObject.GeneratePolygonIndicies());

	GLObject ArcObject(ResourcePath + "objects/arc170.obj");
	Texture ArcTexture(ResourcePath + "textures/arc170.jpg");
	IndexBuffer ArcMeshIBO(ArcObject.GeneratePolygonIndicies());

	GLObject GridObject(gridBuffer, lineCount * 4);

	std::string title;
	float start = window.GetTime();
	float lastUdpateTime = start;
	float end = start;
	
	glm::vec3 position = glm::vec3(0, 1.0f, 3.0f);
	float horizontalAngle = 0.0f;
	float verticalAngle = 0.0f;
	float initialFoV = 65.0f;
	float speed = 5.0;
	float mouseSpeed = 0.75f;
	Window::Position pos { 0.0f, 0.0f };

	Shader cubeShader  (ResourcePath + "shaders/cube_vertex.glsl",   ResourcePath + "shaders/cube_fragment.glsl");
	Shader GridShader  (ResourcePath + "shaders/line_vertex.glsl",   ResourcePath + "shaders/fragment.glsl");
	Shader meshShader  (ResourcePath + "shaders/mesh_vertex.glsl",   ResourcePath + "shaders/fragment.glsl");
	Shader objectShader(ResourcePath + "shaders/object_vertex.glsl", ResourcePath + "shaders/object_fragment.glsl");

	bool drawMesh = false;
	float deltaRot = 0.0f;

	/* Loop until the user closes the window */
	long long counterFPS = 0;
	while (window.IsOpen())
	{
		counterFPS++;
		if (end - lastUdpateTime >= 1.0f)
		{
			title = "MomoEngine Project " + std::to_string(counterFPS) + " fps";
			window.UseTitle(title);
			counterFPS = 0;
			lastUdpateTime = end;

			std::cout << std::setw(10) << std::to_string(position.x) << ' '
					  << std::setw(10) << std::to_string(position.y) << ' '
				      << std::setw(10) << std::to_string(position.z) << std::endl;
		}
		start = end;
		end = window.GetTime();
		float dt = end - start; 

		renderer.Clear();
		
		auto oldPos = pos;
		pos = window.GetCursorPos();

		horizontalAngle -= mouseSpeed * dt * (pos.x - oldPos.x);
		verticalAngle   -= mouseSpeed * dt * (pos.y - oldPos.y);
		verticalAngle = std::max(-glm::half_pi<float>() + 0.01f, verticalAngle);
		verticalAngle = std::min( glm::half_pi<float>() - 0.01f, verticalAngle);

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

		if (window.IsKeyHolded(KeyCode::W))
		{
			position += forward * dt * speed;
		}
		if (window.IsKeyHolded(KeyCode::S))
		{
			position -= forward * dt * speed;
		}
		if (window.IsKeyHolded(KeyCode::D))
		{
			position += right * dt * speed;
		}
		if (window.IsKeyHolded(KeyCode::A))
		{
			position -= right * dt * speed;
		}
		if (window.IsKeyHolded(KeyCode::SPACE))
		{
			position += up * dt * speed;
		}
		if (window.IsKeyHolded(KeyCode::LEFT_SHIFT))
		{
			position -= up * dt * speed;
		}
		if (window.IsKeyPressed(KeyCode::ESCAPE))
		{
			return 0;
		}
		if (window.IsKeyHolded(KeyCode::UP))
		{
			speed *= 1.001f;
		}
		if (window.IsKeyHolded(KeyCode::DOWN))
		{
			speed *= 1 / 1.001f;
		}
		if (window.IsKeyPressed(KeyCode::C))
		{
			initialFoV = 90 - initialFoV;
		}
		if (window.IsKeyPressed(KeyCode::M))
		{
			drawMesh = !drawMesh;
		}
		auto ModelMatrix = glm::mat4(1.0f);
		auto ProjectionMatrix = glm::perspective(glm::radians(initialFoV), float(window.GetWidth()) / float(window.GetHeight()), 0.1f, 2000.0f);
		auto ViewMatrix = glm::lookAt(
			position,
			position + direction,
			up
		);

		deltaRot += dt * 0.1f;
		if (deltaRot > glm::two_pi<float>()) deltaRot -= glm::two_pi<float>();
		auto RotationMatrix = glm::rotate(ModelMatrix, deltaRot, glm::vec3(0.0f, 1.0f, 0.0f));
		auto ScaleMatrix = glm::scale(ModelMatrix, glm::vec3(0.005f));

		auto MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		cubeShader.SetUniformMat4("MVP", MVP);
		CubeTexture.Bind();
		renderer.DrawTrianglesInstanced(CubeObject.GetVertexData(), CubeObject.GetVertexCount(), cubeShader, cubeCount);

		objectShader.SetUniformMat4("MVP", MVP * ScaleMatrix * RotationMatrix);
		ArcTexture.Bind();
		renderer.DrawTriangles(ArcObject.GetVertexData(), ArcObject.GetVertexCount(), objectShader);
		
		GridShader.SetUniformMat4("MVP", MVP);
		renderer.DrawLines(GridObject.GetVertexData(), GridObject.GetVertexCount(), GridShader);

		if (drawMesh)
		{
			meshShader.SetUniformMat4("MVP", MVP);
			renderer.DrawLinesInstanced(CubeObject.GetVertexData(), CubeMeshIBO, meshShader, cubeCount);
			meshShader.SetUniformMat4("MVP", MVP * ScaleMatrix * RotationMatrix);
			renderer.DrawLines(ArcObject.GetVertexData(), ArcMeshIBO, meshShader);
		}
		renderer.Finish();
		window.PullEvents();
	}
	return 0;
}