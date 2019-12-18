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

using namespace MomoEngine;

int main()
{
	Window window(1280, 720);
	window
		.UseProfile(3, 3, Profile::CORE)
		.UseCursorMode(CursorMode::DISABLED)
		.UseSampling(4)
		.UseDoubleBuffering(false)
		.UseTitle("OpenGL Project")
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
	std::vector<GLfloat> lineBuffer;
	for (int i = 0; i < lineCount; i++)
	{
		auto lineWidth = lineCount - 1;
		lineBuffer.push_back(lineWidth / 2 - i);
		lineBuffer.push_back(0.0f);
		lineBuffer.push_back(-(lineWidth - 1) / 2);

		lineBuffer.push_back(lineWidth / 2 - i);
		lineBuffer.push_back(0.0f);
		lineBuffer.push_back((lineWidth - 1) / 2);

		lineBuffer.push_back(-(lineWidth - 1) / 2);
		lineBuffer.push_back(0.0f);
		lineBuffer.push_back(lineWidth / 2 - i);

		lineBuffer.push_back((lineWidth - 1) / 2);
		lineBuffer.push_back(0.0f);
		lineBuffer.push_back(lineWidth / 2 - i);
	}
	std::vector<GLuint> lineIndicies;
	for (int i = 0; i < 4 * (lineCount - 1); i++)
	{
		lineIndicies.push_back(i);
	}
	GLObject Cube(ResourcePath + "objects/crate.obj");
	Texture CubeTexture(ResourcePath + "textures/crate.jpg");
	VertexArray CubeVAO;
	VertexBufferLayout CubeVBL;
	CubeVBL.Push<float>(3);
	CubeVBL.Push<float>(2);
	CubeVAO.AddBuffer(Cube.GetBuffer(), CubeVBL);
	VertexBuffer CubeInstancedVBO(cubeInstancedBuffer);
	VertexBufferLayout CubeInstancedVBL;
	CubeInstancedVBL.Push<float>(3);
	CubeVAO.AddInstancedBuffer(CubeInstancedVBO, CubeInstancedVBL);
	IndexBuffer CubeMeshIBO(Cube.GeneratePolygonIndicies());

	GLObject ArcObject(ResourcePath + "objects/arc170.obj");
	Texture ArcTexture(ResourcePath + "textures/arc170.jpg");
	VertexArray ArcVAO;
	VertexBufferLayout ArcVBL;
	ArcVBL.Push<float>(3);
	ArcVBL.Push<float>(2);
	ArcVBL.Push<float>(3);
	ArcVAO.AddBuffer(ArcObject.GetBuffer(), ArcVBL);
	IndexBuffer ArcMeshIBO(ArcObject.GeneratePolygonIndicies());

	VertexBuffer LineVBO(lineBuffer);
	VertexArray lineVAO;
	VertexBufferLayout lineVBL;
	lineVBL.Push<float>(3);
	lineVAO.AddBuffer(LineVBO, lineVBL);
	IndexBuffer lineIBO(lineIndicies);

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
	Shader lineShader  (ResourcePath + "shaders/line_vertex.glsl",   ResourcePath + "shaders/fragment.glsl");
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
			title = "OpenGL Project " + std::to_string(counterFPS) + " fps";
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
		renderer.DrawTrianglesInstanced(CubeVAO, Cube.GetVertexCount(), cubeShader, cubeCount);

		objectShader.SetUniformMat4("MVP", MVP * ScaleMatrix * RotationMatrix);
		ArcTexture.Bind();
		renderer.DrawTriangles(ArcVAO, ArcObject.GetVertexCount(), objectShader);
		
		lineShader.SetUniformMat4("MVP", MVP);
		renderer.DrawLines(lineVAO, lineIBO, lineShader);

		if (drawMesh)
		{
			meshShader.SetUniformMat4("MVP", MVP);
			renderer.DrawLinesInstanced(CubeVAO, CubeMeshIBO, meshShader, cubeCount);
			meshShader.SetUniformMat4("MVP", MVP * ScaleMatrix * RotationMatrix);
			renderer.DrawLines(ArcVAO, ArcMeshIBO, meshShader);
		}

		renderer.Flush();
		window.PullEvents();
	}
	return 0;
}