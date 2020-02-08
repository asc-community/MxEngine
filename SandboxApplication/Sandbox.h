#include <MxEngine.h>
#include <Library/EntryPoint.h>

using namespace MxEngine;

class SandboxApp : public Application
{
	virtual void OnCreate () override;
	virtual void OnUpdate () override;
	virtual void OnDestroy() override;
public:
	SandboxApp();
};