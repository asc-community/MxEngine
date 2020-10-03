#pragma once

#include <MxEngine.h>
#include "UserObjects.h"

using namespace MxEngine;

class SandboxScene
{
public:
    void OnCreate()
    {
		auto camera = MxObject::Create();
		InitCamera(*camera);
		Rendering::SetViewport(camera->GetComponent<CameraController>());

		auto cube = MxObject::Create();
		InitCube(*cube);
		
		auto sphere = MxObject::Create();
		InitSphere(*sphere);
		
		auto grid = MxObject::Create();
		InitGrid(*grid);
		
		auto surface = MxObject::Create();
		InitSurface(*surface);

        // auto arc = MxObject::Create();
		// InitArc(*arc);

		// auto& deathStar = this->AddObject("DeathStar", MakeUnique<MxObject>());
		// InitDeathStar(deathStar);

		// auto& destroyer = this->AddObject("Destroyer", MakeUnique<MxObject>());
		// InitDestroyer(destroyer);

		// auto object = MxObject::Create();
		// object->AddComponent<MeshSource>(AssetManager::LoadMesh("D:/repos/glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf"));
		
		auto dirLight = MxObject::Create();
		InitDirLight(*dirLight);

		auto pointLight = MxObject::Create();
		InitPointLight(*pointLight);
		
		auto spotLight = MxObject::Create();
		InitSpotLight(*spotLight);

		// auto sound = MxObject::Create();
		// InitSound(*sound);
    }

	void OnUpdate()
	{
		static float thickness = 0.5f;
		static int steps = 10;
		static float maxDistance = 0.5f;
		auto shader = Rendering::GetController().GetEnvironment().Shaders["SSR"_id];

		if (Runtime::IsEditorActive())
		{
			ImGui::Begin("SSR");

			ImGui::DragFloat("thickness", &thickness, 0.01f);
			ImGui::DragFloat("maxDistance", &maxDistance, 0.01f);
			ImGui::DragInt("steps", &steps, 0.1f);

			ImGui::End();
		}

		shader->SetUniformFloat("thickness", thickness);
		shader->SetUniformFloat("maxDistance", maxDistance);
		shader->SetUniformInt("steps", steps);
	}
};