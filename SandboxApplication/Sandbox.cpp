#include "Sandbox.h"
#include <Library/Bindings/Bindings.h>

#include "Objects/UserObjects.h"

void SandboxApp::OnCreate()
{
	this->AddObject("Cube",   MakeUnique<CubeObject>());
	this->AddObject("Arc170", MakeUnique<Arc170Object>());
	this->AddObject("Grid",   MakeUnique<GridObject>());
	// this->AddObject("Destroyer", MakeUnique<DestroyerObject>());
	// this->AddObject("DeathStar", MakeUnique<DeathStarObject>());

	auto& Renderer = this->GetRenderer();

	for (size_t i = 0; i < Renderer.PointLights.GetCount(); i++)
	{
		this->AddObject("PointLight #" + std::to_string(i),
			MakeUnique<LightObject<PointLight>>(Renderer.PointLights[i]));
	}
	for (size_t i = 0; i < Renderer.SpotLights.GetCount(); i++)
	{
		this->AddObject("SpotLight #" + std::to_string(i),
			MakeUnique<LightObject<SpotLight>>(Renderer.SpotLights[i]));
	}

	Renderer.DefaultTexture = CreateTexture("textures/default.jpg");
	Renderer.ObjectShader = CreateShader("shaders/object_vertex.glsl", "shaders/object_fragment.glsl");
	Renderer.MeshShader = CreateShader("shaders/mesh_vertex.glsl", "shaders/mesh_fragment.glsl");
	
	Renderer.GlobalLight
		.UseAmbientColor ({ 0.0f, 0.0f, 0.0f })
		.UseDiffuseColor ({ 0.3f, 0.3f, 0.3f })
		.UseSpecularColor({ 1.0f, 1.0f, 1.0f });

	Renderer.PointLights[0]
		.UseAmbientColor ({ 1.0f, 0.3f, 0.0f })
		.UseDiffuseColor ({ 1.0f, 0.3f, 0.0f })
		.UseSpecularColor({ 1.0f, 0.3f, 0.0f })
		.UsePosition({ -3.0f, 2.0f, -3.0f });

	Renderer.SpotLights[0]
		.UseAmbientColor ({ 1.0f, 1.0f, 1.0f })
		.UseDiffuseColor ({ 1.0f, 1.0f, 1.0f })
		.UseSpecularColor({ 1.0f, 1.0f, 1.0f })
		.UsePosition({ -15.0f, 1.5f, 0.0f })
		.UseDirection({ -1.0f, 1.3f, -1.0f })
		.UseOuterAngle(35.0f)
		.UseInnerAngle(15.0f);
	
	auto camera = MakeUnique<PerspectiveCamera>();
	//auto camera = MakeUnique<OrthographicCamera>();
	
	camera->SetZFar(100000.0f);
	camera->SetAspectRatio((float)this->GetWindow().GetWidth(), (float)this->GetWindow().GetHeight());
	
	auto& controller = Renderer.ViewPort;
	controller.SetCamera(std::move(camera));
	controller.Translate(1.0f, 3.0f, 0.0f);
	controller.SetMoveSpeed(5.0f);
	controller.SetRotateSpeed(0.75f);

	ConsoleBinding("Console").Bind(KeyCode::GRAVE_ACCENT);
	AppCloseBinding("AppClose").Bind(KeyCode::ESCAPE);
	InputBinding("CameraControl", this->GetRenderer().ViewPort)
		.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT)
		.BindRotation();

	this->GetEventDispatcher().AddEventListener<FpsUpdateEvent>("CountFPS",
		[&window = this->GetWindow()](FpsUpdateEvent& e)
		{
			window.UseTitle("Sandbox App " + std::to_string(e.FPS) + " FPS");
		});
}

void SandboxApp::OnUpdate()
{
	// fps tracking
	#define GRAPH_RECORD_SIZE 128
	static float fpsData[GRAPH_RECORD_SIZE];
	static int curPointer = 0;
	fpsData[curPointer] = (float)this->CounterFPS;

	if(this->GetConsole().IsToggled())
	{
		auto& camera = this->GetRenderer().ViewPort;
		auto& globalLight = this->GetRenderer().GlobalLight;
		auto pos = camera.GetPosition();
		float speed = camera.GetMoveSpeed();
		auto zoom = camera.GetZoom();
		static bool mesh = false;
		static ImVec2 lastWindowSize(0.0f, 0.0f);
		static ImVec2 lastWindowPos(0.0f, 0.0f);

		ImGui::SetNextWindowPos({ 0.0f, this->GetConsole().GetSize().y });
		ImGui::SetNextWindowSize({ this->GetConsole().GetSize().x, 0.0f });
		ImGui::Begin("Game Editor");

		ImGui::Checkbox("display mesh", &mesh);
		ImGui::Text("position: (%f, %f, %f)", pos.x, pos.y, pos.z);

		this->ToggleMeshDrawing(mesh);

		if (ImGui::InputFloat("set speed", &speed))
		{
			camera.SetMoveSpeed(speed);
		}
		if (ImGui::InputFloat3("set position", &pos[0]))
		{
			camera.SetPosition(pos);
		}

		ImGui::Text("zoom / fov: %f", zoom);
		if (ImGui::DragFloat("zoom", &zoom, 0.1f, 0.1f, 20.0f))
		{
			camera.SetZoom(zoom);
		}

		ImGui::SetNextWindowPos({ this->GetConsole().GetSize().x, 0.0f });
		ImGui::SetNextWindowSize({ this->GetWindow().GetWidth() - this->GetConsole().GetSize().x - 150.0f, 0.0f });
		ImGui::Begin("scene editor");
		if (ImGui::CollapsingHeader("objects", ImGuiTreeNodeFlags_None))
		{
			ImGui::Indent(5.0f);
			ImGui::BeginGroup();
			for (const auto& pair : this->GetObjectList())
			{
				if (ImGui::CollapsingHeader(pair.first.c_str(), ImGuiTreeNodeFlags_None))
				{
					auto& object = *pair.second;

					// toggle object visibility
					bool isDrawn = object.IsDrawable();
					static bool dirVecs = false;
					if (ImGui::Checkbox("drawn", &isDrawn))
						isDrawn ? object.Show() : object.Hide();

					// toggle dir vec input (see below)
					ImGui::SameLine(); ImGui::Checkbox("dir. vecs", &dirVecs);

					// current texture path
					ImGui::Text((std::string("texture: ") + (object.Texture ? object.Texture->GetPath() : std::string("none"))).c_str());

					// object translation
					const auto& oldTranslation = object.GetTranslation();
					Vector3 newTranslation = oldTranslation;
					if (ImGui::InputFloat3("translation", &newTranslation[0]))
						pair.second->Translate(newTranslation - oldTranslation);

					// object rotation (euler)
					const auto& oldRotation = object.GetEulerRotation();
					Vector3 newRotation = oldRotation;
					if (ImGui::InputFloat3("rotation", &newRotation[0]))
					{
						auto diffRotation = newRotation - oldRotation;
						pair.second->RotateX(diffRotation.x).RotateY(diffRotation.y).RotateZ(diffRotation.z);
					}

					// object scale
					const auto& oldScale = object.GetScale();
					Vector3 newScale = oldScale;
					if (ImGui::InputFloat3("scale", &newScale[0]))
					{
						newScale = Clamp(newScale, MakeVector3(1e-6f), MakeVector3(1e6f));
						pair.second->Scale(newScale / oldScale);
					}

					// object texture (loads from file)
					static std::string texturePath(64, '\0');
					ImGui::InputText("texture path", texturePath.data(), texturePath.size());
					ImGui::SameLine();
					if (ImGui::Button("update"))
						object.Texture = this->CreateTexture(texturePath);

					if (dirVecs)
					{
						Vector3 forward = object.GetForwardVector();
						Vector3 up = object.GetUpVector();
						Vector3 right = object.GetRightVector();
						if (ImGui::InputFloat3("forward vec", &forward[0]))
							object.SetForwardVector(forward);
						if (ImGui::InputFloat3("up vec", &up[0]))
							object.SetUpVector(up);
						if (ImGui::InputFloat3("right vec", &right[0]))
							object.SetRightVector(right);
					}
				}
			}
			ImGui::EndGroup();
			ImGui::Unindent(5.0f);
		}
		if (ImGui::CollapsingHeader("profiler", ImGuiTreeNodeFlags_None))
		{
			INVOKE_ONCE(this->GetEventDispatcher().AddEventListener<FpsUpdateEvent>("FpsGraph", [](FpsUpdateEvent& e)
				{
					curPointer = (curPointer + 1) % GRAPH_RECORD_SIZE;
					if (curPointer == 0) memset(fpsData, 0, GRAPH_RECORD_SIZE * sizeof(float));
				}));
			ImGui::Indent(5.0f);
			ImGui::PlotLines("", fpsData, GRAPH_RECORD_SIZE, 0, "FPS profiler", FLT_MAX, FLT_MAX, { ImGui::GetWindowWidth() - 15, 150 });
			ImGui::Unindent(5.0f);
		}
		if (ImGui::CollapsingHeader("light editor", ImGuiTreeNodeFlags_None))
		{
			auto ambientGlobal  = globalLight.GetAmbientColor();
			auto diffuseGlobal  = globalLight.GetDiffuseColor();
			auto specularGlobal = globalLight.GetSpecularColor();

			ImGui::Indent(5.0f);
			ImGui::BeginGroup();
			if (ImGui::CollapsingHeader("global light", ImGuiTreeNodeFlags_None))
			{
				ImGui::InputFloat3("direction", &globalLight.Direction[0]);

				if (ImGui::InputFloat3("ambient color", &ambientGlobal[0]))
					globalLight.UseAmbientColor(ambientGlobal);
				if (ImGui::InputFloat3("diffuse color", &diffuseGlobal[0]))
					globalLight.UseDiffuseColor(diffuseGlobal);
				if (ImGui::InputFloat3("specular color", &specularGlobal[0]))
					globalLight.UseSpecularColor(specularGlobal);
			}

			for (size_t i = 0; i < this->GetRenderer().PointLights.GetCount(); i++)
			{
				static std::string strLight = "point light #?";
				strLight.back() = char('0' + i);
				if (ImGui::CollapsingHeader(strLight.c_str(), ImGuiTreeNodeFlags_None))
				{
					auto& pointLight = this->GetRenderer().PointLights[i];
					auto ambientPoint = pointLight.GetAmbientColor();
					auto diffusePoint = pointLight.GetDiffuseColor();
					auto specularPoint = pointLight.GetSpecularColor();
					auto factors = pointLight.GetFactors();

					ImGui::InputFloat3("position", &pointLight.Position[0]);
					
					if(ImGui::InputFloat3("K factors", &factors[0]))
						pointLight.UseFactors(factors);
					if (ImGui::InputFloat3("ambient color", &ambientPoint[0]))
						pointLight.UseAmbientColor(ambientPoint);
					if (ImGui::InputFloat3("diffuse color", &diffusePoint[0]))
						pointLight.UseDiffuseColor(diffusePoint);
					if (ImGui::InputFloat3("specular color", &specularPoint[0]))
						pointLight.UseSpecularColor(specularPoint);
				}
			}

			for (size_t i = 0; i < this->GetRenderer().SpotLights.GetCount(); i++)
			{
				static std::string strLight = "spot light #?";
				strLight.back() = char('0' + i);
				if (ImGui::CollapsingHeader(strLight.c_str(), ImGuiTreeNodeFlags_None))
				{
					auto& spotLight = this->GetRenderer().SpotLights[i];
					auto innerAngle = spotLight.GetInnerAngle();
					auto outerAngle = spotLight.GetOuterAngle();
					auto ambientPoint = spotLight.GetAmbientColor();
					auto diffusePoint = spotLight.GetDiffuseColor();
					auto specularPoint = spotLight.GetSpecularColor();

					ImGui::InputFloat3("position", &spotLight.Position[0]);
					ImGui::InputFloat3("direction", &spotLight.Direction[0]);

					if (ImGui::DragFloat("outer angle", &outerAngle, 1.0f, 0.0f, 90.0f))
						spotLight.UseOuterAngle(outerAngle);
					if (ImGui::DragFloat("inner angle", &innerAngle, 1.0f, 0.0f, 90.0f))
						spotLight.UseInnerAngle(innerAngle);

					if (ImGui::InputFloat3("ambient color", &ambientPoint[0]))
						spotLight.UseAmbientColor(ambientPoint);
					if (ImGui::InputFloat3("diffuse color", &diffusePoint[0]))
						spotLight.UseDiffuseColor(diffusePoint);
					if (ImGui::InputFloat3("specular color", &specularPoint[0]))
						spotLight.UseSpecularColor(specularPoint);
				}
			}
			ImGui::EndGroup();
			ImGui::Unindent(5.0f);
		}
	}
}

void SandboxApp::OnDestroy()
{

}

SandboxApp::SandboxApp()
{
	this->ResourcePath = "D:/repos/MxEngine/SandboxApplication/Resources/";
	this->CreateContext();
}