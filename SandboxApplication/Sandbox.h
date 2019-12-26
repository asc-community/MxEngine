#include <MomoEngine.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <MomoEngine/EntryPoint.h>

using namespace MomoEngine;

class SandboxApp : public MomoEngine::IApplication
{
	GLInstance CubeObject, ArcObject, GridObject, DeathStarObject, DestroyerObject;
	VertexBuffer CubesPositions;

	MomoEngine::TimeStep timePassed = 0.0f;
	Window::Position cursorPos{ 0.0f, 0.0f };
	float speed = 5.0;
	float mouseSpeed = 0.75f;
	int cubeCount = 100;
	bool drawMesh = false;

	virtual void OnCreate () override;
	virtual void OnUpdate () override;
	virtual void OnDestroy() override;
public:
	SandboxApp();
};