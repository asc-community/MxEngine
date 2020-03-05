#include <MxEngine.h>

using namespace MxEngine;

class TestApp : public Application
{
public:
	virtual void OnCreate () override;
	virtual void OnUpdate () override;
	virtual void OnDestroy() override;

	TestApp();
};

int main()
{
	TestApp app;
	app.Run();
	return 0;
}