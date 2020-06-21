#pragma once

#include <MxEngine.h>
#include "UserObjects.h"
#include <Library/Primitives/Primitives.h>
#include <Library/Bindings/Bindings.h>

using namespace MxEngine;

class SandboxScene
{
public:
    void OnCreate()
    {
		auto cube = MxObject::Create();
		InitCube(*cube);
		cube.MakeStatic();
		
		auto sphere = MxObject::Create();
		InitSphere(*sphere);
		sphere.MakeStatic();
		
		auto grid = MxObject::Create();
		InitGrid(*grid);
		grid.MakeStatic();
		
		auto surface = MxObject::Create();
		InitSurface(*surface);
		surface.MakeStatic();

        //auto arc = MxObject::Create();
		//InitArc(*arc);
		//arc.MakeStatic();

		// auto& deathStar = this->AddObject("DeathStar", MakeUnique<MxObject>());
		// InitDeathStar(deathStar);

		// auto& destroyer = this->AddObject("Destroyer", MakeUnique<MxObject>());
		// InitDestroyer(destroyer);

		// auto object = MxObject::Create();
		// object->AddComponent<MeshSource>(AssetManager::LoadMesh("D:/repos/glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf"));

		Script initScript(FileManager::GetFilePath("scripts/init.py"_id));
        Application::Get()->ExecuteScript(initScript);
		
		auto dirLight = MxObject::Create();
		InitDirLight(*dirLight);
		dirLight.MakeStatic();

		auto pointLight = MxObject::Create();
		InitPointLight(*pointLight);
		pointLight.MakeStatic();
		
		auto spotLight = MxObject::Create();
		InitSpotLight(*spotLight);
		spotLight.MakeStatic();

		auto camera = MxObject::Create();
		InitCamera(*camera);
		camera.MakeStatic();

		// TODO: add debug draw component 
		// TODO: move LOD compute logic to MeshLOD component

		RenderManager::SetViewport(camera->GetComponent<CameraController>());

		InputControlBinding(RenderManager::GetViewport())
			.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT)
			.BindRotation();
    }

	void OnUpdate()
	{
	}
};