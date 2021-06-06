#include <MxEngine.h>
using namespace MxEngine;

void InitArc(MxObject& object)
{
	struct ArcBehaviour
	{
		void OnUpdate(MxObject& object, float dt)
		{			
			object.LocalTransform.RotateY(-10.0f * dt);
			object.LocalTransform.TranslateX(2.0f * dt);
		}
	};

	object.Name = "Arc170";

	auto update = object.AddComponent<Behaviour>(ArcBehaviour{ });

	object.AddComponent<MeshSource>(AssetManager::LoadMesh("Resources/objects/arc170/arc170.obj"_id));
	object.AddComponent<MeshRenderer>(AssetManager::LoadMaterials("Resources/objects/arc170/arc170.obj"_id));
	object.AddComponent<MeshLOD>(); // LODs can be tweaked and generated at runtime via runtime editor
	
	object.LocalTransform.Scale(0.005f);
	object.LocalTransform.Translate(MakeVector3(5.0f, 1.0f, -5.0f));
}