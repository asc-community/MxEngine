#include <MxEngine.h>
using namespace MxEngine;

void InitCamera(MxObject& object)
{	
	object.Name = "Camera";

	auto listener = object.AddComponent<AudioListener>();
	auto controller = object.AddComponent<CameraController>();
	auto skybox = object.AddComponent<Skybox>();
	auto input = object.AddComponent<InputController>();
	auto effects = object.AddComponent<CameraEffects>();
	auto toneMapping = object.AddComponent<CameraToneMapping>();
	auto ssr = object.AddComponent<CameraSSR>();
	auto ssao = object.AddComponent<CameraSSAO>();

	effects->SetBloomWeight(5.0f);

	ssao->SetIntensity(2.0f);

	toneMapping->SetWhitePoint(0.75f);
	toneMapping->SetMinLuminance(0.3f);

	skybox->CubeMap = AssetManager::LoadCubeMap("Resources/textures/dawn.jpg"_id);
	skybox->Irradiance = AssetManager::LoadCubeMap("Resources/textures/dawn_irradiance.jpg"_id);
	skybox->SetIntensity(5.0f);

	controller->Camera.SetZFar(100000.0f);
	controller->Camera.SetAspectRatio(WindowManager::GetWidth(), WindowManager::GetHeight());

	controller->ListenWindowResizeEvent();
	input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
	input->SetMoveSpeed(5.0f);
	input->BindRotation();

	object.LocalTransform.Translate(MakeVector3(1.0f, 3.0f, 0.0f));
}