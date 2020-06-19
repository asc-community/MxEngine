#pragma once

void InitCamera(MxObject& object)
{
	auto camera = MakeUnique<PerspectiveCamera>();
	// auto camera = MakeUnique<OrthographicCamera>();
	
	object.Name = "Camera";

	auto controller = object.AddComponent<CameraController>();
	auto skybox = object.AddComponent<Skybox>();
	skybox->Texture = AssetManager::LoadCubeMap("textures/dawn.jpg"_id);

	const auto& window = Application::Get()->GetWindow();
	camera->SetZFar(100000.0f);
	camera->SetAspectRatio((float)window.GetWidth(), (float)window.GetHeight());

	controller->SetCamera(std::move(camera));
	controller->SetMoveSpeed(5.0f);
	controller->SetRotateSpeed(0.75f);
	controller->FitScreenViewport();

	object.Transform->Translate(MakeVector3(1.0f, 3.0f, 0.0f));
}