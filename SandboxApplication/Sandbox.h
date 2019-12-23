#include <MomoEngine.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace MomoEngine;

class SandboxApp : public MomoEngine::IApplication
{
	GLInstance CubeObject, ArcObject, GridObject;
	Shader GridShader, MeshShader, ObjectShader, CubeShader;
	IndexBuffer CubeMeshIBO, ArcMeshIBO;
	VertexBuffer CubesPositions;

	MomoEngine::TimeStep timePassed = 0.0f;
	Window::Position cursorPos{ 0.0f, 0.0f };
	glm::vec3 position = glm::vec3(0, 1.0f, 3.0f);
	float horizontalAngle = 0.0f;
	float verticalAngle = 0.0f;
	float FOV = 65.0f;
	float speed = 5.0;
	float mouseSpeed = 0.75f;
	int cubeCount = 100;
	bool drawMesh = false;

	virtual void OnCreate () override;
	virtual void OnUpdate () override;
	virtual void OnDestroy () override;

	void GenerateBuffers();
public:
	SandboxApp();
};