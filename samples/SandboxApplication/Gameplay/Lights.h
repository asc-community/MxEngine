#pragma once

void InitDirLight(MxObject& object)
{
    object.Name = "Directional Light";

    auto light = object.AddComponent<DirectionalLight>();
    light->SetIntensity(4.0f);
    light->FollowViewport(0.0f);
}

void InitPointLight(MxObject& object)
{
    object.Name = "Point Light";

    auto light = object.AddComponent<PointLight>();
    light->SetColor(MakeVector3(1.0f, 0.3f, 0.0f));
    light->SetIntensity(500.0f);
    light->ToggleShadowCast(true);

    auto material = object.GetOrAddComponent<MeshRenderer>()->GetMaterial();
    material->BaseColor = Vector3(1.0f, 0.7f, 0.0f);
    material->Emmision = 50.0f;

    object.AddComponent<MeshSource>(Primitives::CreateCube());
    object.Transform
        .SetPosition(MakeVector3(-3.0f, 2.0f, -3.0f))
        .SetScale(MakeVector3(0.3f));
}

void InitSpotLight(MxObject& object)
{
    object.Name = "Spot Light";

    auto light = object.AddComponent<SpotLight>();
    light->Direction     = { 1.0f, -1.3f, 1.0f };
    light->SetIntensity(10.0f);
    light->UseOuterAngle(70.0f);
    light->UseInnerAngle(30.0f);
    light->ToggleShadowCast(true);

    auto material = object.GetOrAddComponent<MeshRenderer>()->GetMaterial();
    material->BaseColor = light->GetColor();
    material->Emmision = 10.0f;
    
    object.AddComponent<MeshSource>(Primitives::CreateCube());
    object.Transform
        .SetPosition(MakeVector3(-15.0f, 3.0f,  0.0f))
        .SetScale(MakeVector3(0.3f));
}