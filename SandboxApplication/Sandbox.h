#include <MxEngine.h>

using namespace MxEngine;

class SandboxApp : public Application
{
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