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

        this->AddObject("Grid", MakeUnique<Grid>(2000));
        this->AddObject("Cube", MakeUnique<CubeObject>());
        this->AddObject("Arc170", MakeUnique<Arc170Object>());
        this->AddObject("Sphere", MakeUnique<SphereObject>());
        // this->AddObject("Destroyer", MakeUnique<DestroyerObject>());
        // this->AddObject("DeathStar", MakeUnique<DeathStarObject>());
<<<<<<< Updated upstream
=======
		
		auto& surface = (Surface&)this->AddObject("Surface", MakeUnique<Surface>());
		surface.SetSurface([](float x, float y) 
			{ 
				return std::sin(10.0f * x) * std::sin(10.0f * y);
			}, 1.0f, 1.0f, 0.01f);
		surface.Scale(10.0f, 2.0f, 10.0f);
		surface.Translate(0.0f, 2.0f, 0.0f);
>>>>>>> Stashed changes

        this->LoadScript("init", "scripts/init.py");
        this->LoadScript("update", "scripts/update.py");
        this->LoadScript("load", "scripts/load.py");

        Application::Get()->ExecuteScript(*this->GetResource<Script>("init"));

		this->PointLights.SetCount(1);
		this->SpotLights.SetCount(1);

		this->GlobalLight
			.UseAmbientColor({ 0.0f, 0.0f, 0.0f })
			.UseDiffuseColor({ 0.3f, 0.3f, 0.3f })
			.UseSpecularColor({ 1.0f, 1.0f, 1.0f })
			;

		this->PointLights[0]
			.UseAmbientColor({ 1.0f, 0.3f, 0.0f })
			.UseDiffuseColor({ 1.0f, 0.3f, 0.0f })
			.UseSpecularColor({ 1.0f, 0.3f, 0.0f })
			.UsePosition({ -3.0f, 2.0f, -3.0f });
		
		this->SpotLights[0]
			.UseAmbientColor({ 0.2f, 0.2f, 0.2f })
			.UseDiffuseColor({ 0.5f, 0.5f, 0.5f })
			.UseSpecularColor({ 0.5f, 0.5f, 0.5f })
			.UsePosition({ -15.0f, 3.0f, 0.0f })
			.UseDirection({ -1.0f, 1.3f, -1.0f })
			.UseOuterAngle(35.0f)
			.UseInnerAngle(15.0f);

		auto camera = MakeUnique<PerspectiveCamera>();
		// auto camera = MakeUnique<OrthographicCamera>();
		// Application::Get()->GetRenderer().ToggleReversedDepth(false);

<<<<<<< Updated upstream
=======
		this->PointLights.SetCount(0);

>>>>>>> Stashed changes
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
		LightBinding(this->PointLights).BindAll();
		LightBinding(this->SpotLights).BindAll();
		InputControlBinding("CameraControl", this->Viewport)
			.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT)
			.BindRotation();

		Application::Get()->ExecuteScript(*this->GetResource<Script>("load"));
	}

    virtual void OnUpdate() override
    {
		Application::Get()->ExecuteScript(*this->GetResource<Script>("update"));
    }
};