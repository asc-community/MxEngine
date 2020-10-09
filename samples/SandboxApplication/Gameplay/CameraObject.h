#pragma once

void InitCamera(MxObject& object)
{	
	object.Name = "Camera";

	auto listener = object.AddComponent<AudioListener>();
	auto controller = object.AddComponent<CameraController>();
	auto effects = object.AddComponent<CameraEffects>();
	auto skybox = object.AddComponent<Skybox>();
	auto input = object.AddComponent<InputController>();

	effects->SetVignetteRadius(0.15f);
	effects->SetBloomIterations(6);
	effects->SetWhitePoint(0.75f);
	effects->SetMinLuminance(0.3f);
	effects->ToggleToneMapping(true);
	effects->SetSSRSteps(10);
	effects->SetSSRSkyboxMultiplier(0.3f);

	skybox->Texture = AssetManager::LoadCubeMap("textures/dawn.jpg"_id);

	controller->Camera.SetZFar(100000.0f);
	controller->Camera.SetAspectRatio(WindowManager::GetWidth(), WindowManager::GetHeight());

	controller->SetMoveSpeed(5.0f);
	controller->ListenWindowResizeEvent();
	input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
	input->BindRotation();

	object.Transform.Translate(MakeVector3(1.0f, 3.0f, 0.0f));
}