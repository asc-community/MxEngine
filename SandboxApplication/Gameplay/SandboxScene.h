#pragma once

#include <MxEngine.h>
#include <Library/Primitives/Primitives.h>
#include <Library/Bindings/Bindings.h>
#include "UserObjects.h"

using namespace MxEngine;

class SandboxScene : public Scene
{
    virtual void OnCreate() override
    {
        this->SetDirectory("Resources/");

        this->AddObject("Cube", MakeUnique<CubeObject>());
        this->AddObject("Sphere", MakeUnique<SphereObject>());
        this->AddObject("Arc170", MakeUnique<Arc170Object>());
        // this->AddObject("Destroyer", MakeUnique<DestroyerObject>());
        // this->AddObject("DeathStar", MakeUnique<DeathStarObject>());
		
		auto& surface = (Surface&)this->AddObject("Surface", MakeUnique<Surface>());
		surface.SetSurface([](float x, float y) 
			{ 
				return std::sin(10.0f * x) * std::sin(10.0f * y);
			}, 1.0f, 1.0f, 0.01f);
		surface.Scale(10.0f, 2.0f, 10.0f);
		surface.Translate(10.0f, 3.0f, 10.0f);

		auto& grid = this->AddObject("Grid", MakeUnique<Grid>(2000));
		grid.Scale(3.0f, 3.0f, 3.0f);
		grid.ObjectTexture = this->LoadTexture("Bricks", "textures/brick.jpg");
		auto& material = grid.GetMesh()->GetRenderObjects()[0].GetMaterial();
		material.map_normal = Graphics::Instance()->CreateTexture(GetDirectory() / "textures/brick_normal.jpg");
		
        this->LoadScript("init", "scripts/init.py");
        this->LoadScript("update", "scripts/update.py");
        this->LoadScript("load", "scripts/load.py");

        Application::Get()->ExecuteScript(*this->GetResource<Script>("init"));

		this->SceneSkybox = MakeUnique<Skybox>();
		this->SceneSkybox->SkyboxTexture = this->LoadCubeMap("Skybox", "textures/dawn.jpg");

		this->PointLights.SetCount(1);
		this->SpotLights.SetCount(1);

		
		this->GlobalLight.AmbientColor  = { 0.3f, 0.3f, 0.3f };
		this->GlobalLight.DiffuseColor  = { 0.3f, 0.3f, 0.3f };
		this->GlobalLight.SpecularColor = { 1.0f, 1.0f, 1.0f };

		this->PointLights[0].AmbientColor  = { 1.0f, 0.3f, 0.0f };
		this->PointLights[0].DiffuseColor  = { 1.0f, 0.3f, 0.0f };
		this->PointLights[0].SpecularColor = { 1.0f, 0.3f, 0.0f };
		this->PointLights[0].UsePosition({ -3.0f, 2.0f, -3.0f });
		
		this->SpotLights[0].AmbientColor  = { 1.0f, 1.0f, 1.0f };
		this->SpotLights[0].DiffuseColor  = { 1.0f, 1.0f, 1.0f };
		this->SpotLights[0].SpecularColor = { 1.0f, 1.0f, 1.0f };
		this->SpotLights[0].UsePosition({ -15.0f, 3.0f, 0.0f });
		this->SpotLights[0].UseDirection({ -1.0f, 1.3f, -1.0f });
		this->SpotLights[0].UseOuterAngle(35.0f);
		this->SpotLights[0].UseInnerAngle(15.0f);

		auto camera = MakeUnique<PerspectiveCamera>();
		// auto camera = MakeUnique<OrthographicCamera>();
		// Application::Get()->GetRenderer().ToggleReversedDepth(false);

		// this->PointLights.SetCount(0);

		const auto& window = Application::Get()->GetWindow();
		camera->SetZFar(100000.0f);
		camera->SetAspectRatio((float)window.GetWidth(), (float)window.GetHeight());

		this->Viewport.SetCamera(std::move(camera));
		this->Viewport.Translate(1.0f, 3.0f, 0.0f);
		this->Viewport.SetMoveSpeed(5.0f);
		this->Viewport.SetRotateSpeed(0.75f);

    }

	virtual void OnLoad() override
	{
		InputControlBinding("CameraControl", this->Viewport)
			.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT)
			.BindRotation();

		Application::Get()->ExecuteScript(*this->GetResource<Script>("load"));
		LightBinding(this->PointLights).BindAll();
		LightBinding(this->SpotLights).BindAll();
	}

	virtual void OnUnload() override
	{

	}

    virtual void OnUpdate() override
    {
		Application::Get()->ExecuteScript(*this->GetResource<Script>("update"));
    }
};