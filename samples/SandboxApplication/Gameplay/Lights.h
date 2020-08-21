#pragma once

void InitDirLight(MxObject& object)
{
    object.Name = "Directional Light";

    auto light = object.AddComponent<DirectionalLight>();
    light->AmbientColor  = { 0.3f, 0.3f, 0.3f };
    light->DiffuseColor  = { 0.3f, 0.3f, 0.3f };
    light->SpecularColor = { 1.0f, 1.0f, 1.0f };
    light->FollowViewport();
}

void InitPointLight(MxObject& object)
{
    object.Name = "Point Light";

    auto light = object.AddComponent<PointLight>();
    light->AmbientColor  = {  1.0f, 0.3f,  0.0f };
    light->DiffuseColor  = {  1.0f, 0.3f,  0.0f };
    light->SpecularColor = {  1.0f, 0.3f,  0.0f };

    auto material = object.GetOrAddComponent<MeshRenderer>()->GetMaterial();
    material->BaseColor = light->AmbientColor + light->DiffuseColor;
    material->Emmision = 5.0f;

    object.AddComponent<MeshSource>(Primitives::CreateCube());
    object.Transform
        .SetPosition(MakeVector3(-3.0f, 2.0f, -3.0f))
        .SetScale(MakeVector3(0.3f));
}

void InitSpotLight(MxObject& object)
{
    object.Name = "Spot Light";

    auto light = object.AddComponent<SpotLight>();
    light->AmbientColor  = { 0.0f,  0.0f, 0.0f };
    light->DiffuseColor  = { 1.0f,  1.0f, 1.0f };
    light->SpecularColor = { 1.0f,  1.0f, 1.0f };
    light->Direction     = { 1.0f, -1.3f, 1.0f };
    light->UseOuterAngle(70.0f);
    light->UseInnerAngle(30.0f);

    auto material = object.GetOrAddComponent<MeshRenderer>()->GetMaterial();
    material->BaseColor = light->AmbientColor + light->DiffuseColor;
    material->Emmision = 5.0f;
    
    object.AddComponent<MeshSource>(Primitives::CreateCube());
    object.Transform
        .SetPosition(MakeVector3(-15.0f, 3.0f,  0.0f))
        .SetScale(MakeVector3(0.3f));
}