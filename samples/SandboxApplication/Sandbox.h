#include <MxEngine.h>

using namespace MxEngine;

class SandboxScene;

/*
in this sample I test most of the engine functional
you can just launch it and take a look 
how most of the engine features work
*/
class SandboxApp : public Application
{
	SandboxScene* scene;
public:
	virtual void OnCreate() override;
	virtual void OnUpdate() override;
	virtual void OnDestroy() override;
	virtual void OnRender() override;

	SandboxApp();
};