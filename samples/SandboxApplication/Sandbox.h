#include <MxEngine.h>

using namespace MxEngine;

class SandboxScene;

class SandboxApp : public Application
{
	SandboxScene* scene;
public:
	virtual void OnCreate () override;
	virtual void OnUpdate () override;
	virtual void OnDestroy() override;

	SandboxApp();
};

int main()
{
	SandboxApp app;
	app.Run();
	return 0;
}