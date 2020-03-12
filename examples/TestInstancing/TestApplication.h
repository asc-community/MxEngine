#include <MxEngine.h>

using namespace MxEngine;

class SnakePath3D : public Application
{
public:
	virtual void OnCreate () override;
	virtual void OnUpdate () override;
	virtual void OnDestroy() override;

	SnakePath3D();
};

int main()
{
	int a[3];
	ArrayView<int> A(a);
	SnakePath3D app;
	app.Run();
	return 0;
}