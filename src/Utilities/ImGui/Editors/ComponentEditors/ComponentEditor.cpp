// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "Core/Components/Behaviour.h"
#include "Core/Components/Transform.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Utilities/Format/Format.h"
#include "Utilities/STL/MxMap.h"
#include "GenericComponentEditor.h"

namespace MxEngine::GUI
{
	void Display(const char* name, bool b, const ReflectionMeta& meta)
	{
		ImGui::Text("%s: %s", name, BOOL_STRING(b));
	}

	void Display(const char* name, const MxString& str, const ReflectionMeta& meta)
	{
		ImGui::Text("%s: %s", name, str.c_str());
	}

	void Display(const char* name, float f, const ReflectionMeta& meta)
	{
		ImGui::Text("%s: %f", name, f);
	}

	void Display(const char* name, const Quaternion& q, const ReflectionMeta& meta)
	{
		auto angles = DegreesVec(MakeEulerAngles(q));
		ImGui::Text("%s: (%f, %f, %f)", name, angles[0], angles[1], angles[2]);
	}

	void Display(const char* name, const Vector2& v, const ReflectionMeta& meta)
	{
		ImGui::Text("%s: (%f, %f)", name, v[0], v[1]);
	}

	void Display(const char* name, const Vector3& v, const ReflectionMeta& meta)
	{
		ImGui::Text("%s: (%f, %f, %f)", name, v[0], v[1], v[2]);
	}

	void Display(const char* name, const Vector4& v, const ReflectionMeta& meta)
	{
		ImGui::Text("%s: (%f, %f, %f, %f)", name, v[0], v[1], v[2], v[3]);
	}

	void DisplaySequantialContainer(const char* name, const rttr::variant& val, const ReflectionMeta& meta)
	{
		auto view = val.create_sequential_view();

		ImGui::Text("%s", name);
		ImGui::Indent(5.0f);
		int id = 0;
		for (const auto& element : view)
		{
			ImGui::PushID(id++);
			ResourceEditorImpl(element.extract_wrapped_value());
			ImGui::PopID();
		}
		ImGui::Unindent(5.0f);
	}

	template<typename T>
	void DisplayGeneric(const char* name, const rttr::variant& v, const ReflectionMeta& meta)
	{
		Display(name, v.template get_value<T>(), meta);
	}

	rttr::variant Edit(const char* name, bool val, const ReflectionMeta& meta)
	{
		bool edited = ImGui::Checkbox(name, &val);
		return edited ? rttr::variant{ val } : rttr::variant{ };
	}

	rttr::variant Edit(const char* name, MxString val, const ReflectionMeta& meta)
	{
		static MxString text;
		bool edited = GUI::InputTextOnClick(MxFormat("{}: {}", name, val).c_str(), text, 128);
		return edited ? rttr::variant{ val } : rttr::variant{ };
	}

	rttr::variant Edit(const char* name, float val, const ReflectionMeta& meta)
	{
		bool edited = ImGui::DragFloat(name, &val, meta.Editor.EditPrecision, meta.Editor.EditRange.Min, meta.Editor.EditRange.Max);
		return edited ? rttr::variant{ val } : rttr::variant{ };
	}

	rttr::variant Edit(const char* name, Quaternion val, const ReflectionMeta& meta)
	{
		auto angles = DegreesVec(MakeEulerAngles(val));
		auto oldAngles = angles;

		ImGui::Text("%s", name);
		ImGui::Indent(5.0f);

		bool editedX = ImGui::DragFloat("rotate x", &angles.x, meta.Editor.EditPrecision, meta.Editor.EditRange.Min, meta.Editor.EditRange.Max);
		bool editedY = ImGui::DragFloat("rotate y", &angles.y, meta.Editor.EditPrecision, meta.Editor.EditRange.Min, meta.Editor.EditRange.Max);
		bool editedZ = ImGui::DragFloat("rotate z", &angles.z, meta.Editor.EditPrecision, meta.Editor.EditRange.Min, meta.Editor.EditRange.Max);

		ImGui::Unindent(5.0f);

		if (editedX) val *= MakeQuaternion(Radians(angles.x - oldAngles.x), MakeVector3(1.0f, 0.0f, 0.0f));
		if (editedY) val *= MakeQuaternion(Radians(angles.y - oldAngles.y), MakeVector3(0.0f, 1.0f, 0.0f));
		if (editedZ) val *= MakeQuaternion(Radians(angles.z - oldAngles.z), MakeVector3(0.0f, 0.0f, 1.0f));

		return (editedX || editedY || editedZ) ? rttr::variant{ val } : rttr::variant{ };
	}

	rttr::variant Edit(const char* name, Vector2 val, const ReflectionMeta& meta)
	{
		bool edited = ImGui::DragFloat2(name, &val[0], meta.Editor.EditPrecision, meta.Editor.EditRange.Min, meta.Editor.EditRange.Max);
		return edited ? rttr::variant{ val } : rttr::variant{ };
	}

	rttr::variant Edit(const char* name, Vector3 val, const ReflectionMeta& meta)
	{
		bool edited = false;
		switch (meta.Editor.InterpretAs)
		{
		case InterpretAsInfo::COLOR:
			edited = ImGui::ColorEdit3(name, &val[0]);
			break;
		default:
			MXLOG_WARNING("MxEngine::RuntimeEditor", "cannot correctly interpret " + MxString(name));
			// fallthrough
		case InterpretAsInfo::DEFAULT:
			edited = ImGui::DragFloat3(name, &val[0], meta.Editor.EditPrecision, meta.Editor.EditRange.Min, meta.Editor.EditRange.Max);
			break;
		}
		return edited ? rttr::variant{ val } : rttr::variant{ };
	}

	rttr::variant Edit(const char* name, Vector4 val, const ReflectionMeta& meta)
	{
		bool edited = false;
		switch (meta.Editor.InterpretAs)
		{
		case InterpretAsInfo::COLOR:
			edited = ImGui::ColorEdit4(name, &val[0], ImGuiColorEditFlags_AlphaBar);
			break;
		default:
			MXLOG_WARNING("MxEngine::RuntimeEditor", "cannot correctly interpret " + MxString(name));
			// fallthrough
		case InterpretAsInfo::DEFAULT:
			edited = ImGui::DragFloat4(name, &val[0], meta.Editor.EditPrecision, meta.Editor.EditRange.Min, meta.Editor.EditRange.Max);
			break;
		}
		return edited ? rttr::variant{ val } : rttr::variant{ };
	}

	rttr::variant EditSequantialContainer(const char* name, const rttr::variant& v, const ReflectionMeta& meta)
	{
		DisplaySequantialContainer(name, v, meta);
		return rttr::variant{ };
	}

	void VisitDisplay(const char* name, const rttr::variant& v, const ReflectionMeta& meta)
	{
		using DisplayCallback = void(*)(const char*, const rttr::variant&, const ReflectionMeta&);
		static MxMap<rttr::type, DisplayCallback> visitor = {
			{ rttr::type::get<bool>(),           DisplayGeneric<bool>           },
			{ rttr::type::get<MxString>(),       DisplayGeneric<MxString>       },
			{ rttr::type::get<float>(),          DisplayGeneric<float>          },
			{ rttr::type::get<Quaternion>(),     DisplayGeneric<Quaternion>     },
			{ rttr::type::get<Vector2>(),        DisplayGeneric<Vector2>        },
			{ rttr::type::get<Vector3>(),        DisplayGeneric<Vector3>        },
			{ rttr::type::get<Vector4>(),        DisplayGeneric<Vector4>        },
		};
		if (visitor.find(v.get_type()) != visitor.end())
		{
			visitor[v.get_type()](name, v, meta);
		}
		else if (v.is_sequential_container())
		{
			DisplaySequantialContainer(name, v, meta);
		}
		else
		{
			MXLOG_WARNING("MxEngine::RuntimeEditor", MxFormat("no visitor defined to display {}", name));
		}
	}

	template<typename T>
	rttr::variant EditGeneric(const char* name, const rttr::variant& v, const ReflectionMeta& meta)
	{
		return Edit(name, v.template get_value<T>(), meta);
	}

	rttr::variant VisitEdit(const char* name, const rttr::variant& v, const ReflectionMeta& meta)
	{
		using EditCallback = rttr::variant(*)(const char*, const rttr::variant&, const ReflectionMeta&);
		static MxMap<rttr::type, EditCallback> visitor = {
			{ rttr::type::get<bool>(),       EditGeneric<bool>       },
			{ rttr::type::get<MxString>(),   EditGeneric<MxString>   },
			{ rttr::type::get<float>(),      EditGeneric<float>      },
			{ rttr::type::get<Quaternion>(), EditGeneric<Quaternion> },
			{ rttr::type::get<Vector2>(),    EditGeneric<Vector2>    },
			{ rttr::type::get<Vector3>(),    EditGeneric<Vector3>    },
			{ rttr::type::get<Vector4>(),    EditGeneric<Vector4>    },
		};

		if (visitor.find(v.get_type()) != visitor.end())
		{
			return visitor[v.get_type()](name, v, meta);
		}
		else if(v.is_sequential_container())
		{
			return EditSequantialContainer(name, v, meta);
		}
		else
		{
			MXLOG_WARNING("MxEngine::RuntimeEditor", MxFormat("no visitor defined to display {}", name));
			return rttr::variant{ };
		}
	}

	class TreeNodeManager
	{
		std::string lastTreeNode;
	public:
		void NewNode(std::string name)
		{
			if (lastTreeNode == name)
			{
				if (!lastTreeNode.empty()) 
					ImGui::TreePop();
				lastTreeNode = name;
				if (!lastTreeNode.empty()) 
					ImGui::TreePush(name.c_str());
			}
		}

		void EndNodes()
		{
			if (!lastTreeNode.empty())
				ImGui::TreePop();
			lastTreeNode.clear();
		}
	};

	template<typename THandle>
	rttr::instance DereferenceGeneric(rttr::instance object)
	{
		THandle& handle = *object.try_convert<THandle>();
		return rttr::instance{ *handle };
	}

	rttr::instance DereferenceHandle(rttr::instance object)
	{
		using DereferenceCallback = rttr::instance(*)(rttr::instance);
		MxMap<rttr::type, DereferenceCallback> visitor = {
			{ rttr::type::get<MaterialHandle>(), DereferenceGeneric<MaterialHandle> },
		};
		
		auto t = object.get_type();
		if (visitor.find(t) != visitor.end())
		{
			return visitor[t](object);
		}
		else
		{
			MXLOG_WARNING("MxEngine::RuntimeEditor", MxFormat("no visitor defined to dereference handle {}", t.get_name().cbegin()));
			return rttr::instance{ };
		}
	}

	void ReflectObject(rttr::instance object)
	{
		rttr::type t = object.get_type();
		if (IsHandle(t))
		{
			ReflectObject(DereferenceHandle(object));
			return;
		}

		TreeNodeManager treeNodeManager;
		for (const auto& property : t.get_properties())
		{
			ReflectionMeta meta(property);

			if ((meta.Flags & MetaInfo::EDITABLE) == 0) continue;
			if (meta.Editor.ViewCondition != nullptr && !meta.Editor.ViewCondition(object)) continue;

			treeNodeManager.NewNode(meta.Editor.SubtreeName);

			if (meta.Editor.ExtraView != nullptr)
			{
				meta.Editor.ExtraView(object);
			}
			else
			{
				auto propertyValue = property.get_value(object);
				const char* name = property.get_name().cbegin();
				if (property.is_readonly())
				{
					VisitDisplay(name, propertyValue, meta);
				}
				else
				{
					auto editedValue = VisitEdit(name, propertyValue, meta);
					if (editedValue.is_valid())
					{
						property.set_value(object, editedValue);
					}
				}
			}
		}
		treeNodeManager.EndNodes();

		for (const auto& method : t.get_methods())
		{
			ReflectionMeta meta(method);

			if ((meta.Flags & MetaInfo::EDITABLE) == 0) continue;
			if (meta.Editor.ViewCondition != nullptr && !meta.Editor.ViewCondition(object)) continue;

			treeNodeManager.NewNode(meta.Editor.SubtreeName);

			auto params = method.get_parameter_infos();

			if (meta.Editor.ExtraView != nullptr)
			{
				meta.Editor.ExtraView(object);
			}
			else
			{
				if (params.empty())
				{
					if (ImGui::Button(method.get_name().cbegin()))
						method.invoke(object);
				}
				else
				{
					for (const auto& param : params)
					{
						// TODO
					}
					ImGui::Text("TODO: cannot add parameters for method call");
				}
			}
		}
		treeNodeManager.EndNodes();
	}

	void ComponentEditorImpl(rttr::instance component)
	{
		ReflectObject(component);
	}

	void ResourceEditorImpl(rttr::instance resource)
	{
		ReflectObject(resource);
	}

    void TransformEditor(TransformComponent& transform)
    {
		ResourceEditor("Transform", transform);
    }

	void BehaviourEditor(Behaviour& behaviour)
	{
		ComponentEditor(behaviour);
	}
}