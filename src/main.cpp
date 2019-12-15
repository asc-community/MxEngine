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

#include "Renderer.h"
#include "Window.h"

int main()
{
	Window window(1280, 720);
	window
		.UseProfile(3, 3, Profile::CORE)
		.UseCursorMode(CursorMode::DISABLED)
		.UseSampling(4)
		.UseDoubleBuffering(false)
		.UseTitle("OpenGL Project")
		.Create();

	auto renderer = Renderer()
		.UseDepthBuffer()
		.UseCulling()
		.UseSampling()
		.UseClearColor(0.0f, 0.0f, 0.0f);

	std::vector<GLfloat> bufferData =
	{
		-1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, -1.0f,	1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f,	1.0f, 1.0f, 0.0f,
	};

	std::vector<GLuint> indicies =
	{
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	int blockCount = 1000;
	std::vector<GLfloat> instancedBuffer;
	for (int i = 0; i < blockCount; i++)
	{
		instancedBuffer.push_back(10.0f * sin(0.2f * i));
		instancedBuffer.push_back(0.5f * i);
		instancedBuffer.push_back(10.0f * cos(0.2f * i));
	}

	VertexBuffer VBO(bufferData.data(), bufferData.size() * sizeof(GLfloat));
	VertexBuffer InstancedVBO(instancedBuffer.data(), instancedBuffer.size() * sizeof(GLfloat));

	VertexArray VAO;

	VertexBufferLayout VBL;
	VBL.Push<float>(3);
	VBL.Push<float>(3);
	VAO.AddBuffer(VBO, VBL);

	VertexBufferLayout InstancedVBL;
	InstancedVBL.Push<float>(3);
	VAO.AddInstancedBuffer(InstancedVBO, InstancedVBL);

	IndexBuffer IBO(indicies.data(), indicies.size() * sizeof(GLuint));

	Shader shader("vertex.glsl", "fragment.glsl");
	shader.Bind();

	std::string title;
	float start = window.GetTime();
	float end = start;
	
	glm::vec3 position = glm::vec3(0, 0.5f, 3.0f);
	float horizontalAngle = glm::pi<float>();
	float verticalAngle = glm::half_pi<float>();
	float initialFoV = 60.0f;
	float speed = 5.0;
	float mouseSpeed = 0.75f;
	Window::CursorPos pos { 0.0f, 0.0f };

	Shader meshShader("mesh_vertex.glsl", "fragment.glsl");

	float deltaRot = 0.0f;

	/* Loop until the user closes the window */
	while (window.IsOpen())
	{
		start = end;
		end = window.GetTime();
		float dt = end - start;
		title = "OpenGL Project " + std::to_string(int(1.0f / dt)) + " fps";
		window.UseTitle(title);

		renderer.Clear();
		
		auto oldPos = pos;
		pos = window.GetCursorPos();

		horizontalAngle -= mouseSpeed * dt * (pos.x - oldPos.x);
		verticalAngle   -= mouseSpeed * dt * (pos.y - oldPos.y);
		verticalAngle = std::max(-glm::half_pi<float>(), verticalAngle);
		verticalAngle = std::min( glm::half_pi<float>(), verticalAngle);

		glm::vec3 direction(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);

		glm::vec3 right = glm::vec3(
			sin(horizontalAngle - glm::half_pi<float>()),
			0,
			cos(horizontalAngle - glm::half_pi<float>())
		);

		glm::vec3 up = glm::cross(right, direction);

		if (window.IsKeyPressed(KeyCode::W))
		{
			position += direction * dt * speed;
		}
		if (window.IsKeyPressed(KeyCode::S))
		{
			position -= direction * dt * speed;
		}
		if (window.IsKeyPressed(KeyCode::D))
		{
			position += right * dt * speed;
		}
		if (window.IsKeyPressed(KeyCode::A))
		{
			position -= right * dt * speed;
		}
		if (window.IsKeyPressed(KeyCode::SPACE))
		{
			position += up * dt * speed;
		}
		if (window.IsKeyPressed(KeyCode::LEFT_SHIFT))
		{
			position -= up * dt * speed;
		}
		if (window.IsKeyPressed(KeyCode::ESCAPE))
		{
			return 0;
		}
		auto ModelMatrix = glm::mat4(1.0f);
		auto ProjectionMatrix = glm::perspective(glm::radians(initialFoV), float(window.GetWidth()) / float(window.GetHeight()), 0.1f, 250.0f);
		auto ViewMatrix = glm::lookAt(
			position,
			position + direction,
			up
		);

		deltaRot += dt * 0.1f;
		if (deltaRot > glm::two_pi<float>()) deltaRot -= glm::two_pi<float>();
		auto RotationMatrix = glm::rotate(ModelMatrix, deltaRot, glm::vec3(0.0f, 1.0f, 0.0f));

		auto MVP = ProjectionMatrix * ViewMatrix * ModelMatrix * RotationMatrix;

		shader.Bind();
		shader.SetUniformMat4("MVP", MVP);
		renderer.DrawTrianglesInstanced(VAO, IBO, shader, blockCount);
		//meshShader.Bind();
		//meshShader.SetUniformMat4("MVP", MVP);
		//renderer.DrawLines(VAO, IBO, meshShader);

		renderer.Flush();
		window.PullEvents();
	}
	return 0;
}