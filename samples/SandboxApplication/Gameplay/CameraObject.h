#pragma once

void InitCamera(MxObject& object)
{	
	object.Name = "Camera";

	auto listener = object.AddComponent<AudioListener>();
	auto controller = object.AddComponent<CameraController>();
	auto skybox = object.AddComponent<Skybox>();
	auto input = object.AddComponent<InputController>();
	//auto effects = object.AddComponent<CameraEffects>();
	//auto toneMapping = object.AddComponent<CameraToneMapping>();
	//auto ssr = object.AddComponent<CameraSSR>();

	//toneMapping->SetWhitePoint(0.75f);
	//toneMapping->SetMinLuminance(0.3f);

	skybox->Texture = AssetManager::LoadCubeMap("textures/dawn.jpg"_id);

	controller->Camera.SetZFar(100000.0f);
	controller->Camera.SetAspectRatio(WindowManager::GetWidth(), WindowManager::GetHeight());

	controller->SetMoveSpeed(5.0f);
	controller->ListenWindowResizeEvent();
	input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
	input->BindRotation();

	object.Transform.Translate(MakeVector3(1.0f, 3.0f, 0.0f));
}