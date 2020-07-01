#pragma once

void InitArc(MxObject& object)
{
	struct ArcBehaviour
	{
		void OnUpdate(MxObject& object, float dt)
		{
			auto script = object.GetComponent<Script>();
			if (script.IsValid()) script->Execute();
			
			object.Transform->RotateY(-10.0f * dt);
			object.Transform->TranslateForward(2.0f * dt);
		}
	};

	object.Name = "Arc170";

	auto update = object.AddComponent<Behaviour>(ArcBehaviour{ });
	auto script = object.AddComponent<Script>("scripts/update.py"_id);

	object.AddComponent<MeshSource>(AssetManager::LoadMesh("objects/arc170/arc170.obj"_id));
	object.AddComponent<MeshRenderer>(AssetManager::LoadMaterials("objects/arc170/arc170.obj"_id));
	object.AddComponent<MeshLOD>(); // LODs can be tweaked and generated at runtime via runtime editor
	
	object.Transform->Scale(0.005f);
	object.Transform->Translate(MakeVector3(5.0f, 1.0f, -5.0f));
}