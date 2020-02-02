#include <MomoEngine.h>
#include <Library/EntryPoint.h>

using namespace MomoEngine;

class SandboxApp : public Application
{
	virtual void OnCreate () override;
	virtual void OnUpdate () override;
	virtual void OnDestroy() override;
public:
	SandboxApp();
};