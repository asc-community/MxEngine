#include <MomoEngine.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <MomoEngine/EntryPoint.h>

using namespace MomoEngine;

class SandboxApp : public MomoEngine::IApplication
{
	virtual void OnCreate () override;
	virtual void OnUpdate () override;
	virtual void OnDestroy() override;
public:
	SandboxApp();
};