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
#include "Core/Runtime/Reflection.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Utilities/Format/Format.h"
#include "Utilities/STL/MxMap.h"

namespace MxEngine::GUI
{
	#define REMOVE_COMPONENT_BUTTON(comp) \
	if(ImGui::Button("remove component")) {\
		MxObject::template GetByComponent(comp).template RemoveComponent<std::remove_reference_t<decltype(comp)>>(); return; }

	template<typename... Args>
	void DisplayImpl(const rttr::property& property, const char* format, Args&&... args)
	{
		const char* name = property.get_name().cbegin();
		ImGui::Text(format, name, std::forward<Args>(args)...);
	}

	void DisplayBool(rttr::instance parent, const rttr::property& property)
	{
		auto val = property.get_value(parent).get_value<bool>();
		DisplayImpl(property, "%s: %s", BOOL_STRING(val));
	}

	void DisplayString(rttr::instance parent, const rttr::property& property)
	{
		auto val = property.get_value(parent).get_value<MxString>();
		DisplayImpl(property, "%s: %s", val.c_str());
	}

	void DisplayFloat(rttr::instance parent, const rttr::property& property)
	{
		auto val = property.get_value(parent).get_value<float>();
		DisplayImpl(property, "%s: %f", val);
	}

	void DisplayQuaternion(rttr::instance parent, const rttr::property& property)
	{
		auto val = property.get_value(parent).get_value<Quaternion>();
		auto angles = DegreesVec(MakeEulerAngles(val));
		DisplayImpl(property, "%s: (%f, %f, %f)", angles[0], angles[1], angles[2]);
	}

	void DisplayVector2(rttr::instance parent, const rttr::property& property)
	{
		auto val = property.get_value(parent).get_value<Vector2>();
		DisplayImpl(property, "%s: (%f, %f)", val[0], val[1]);
	}

	void DisplayVector3(rttr::instance parent, const rttr::property& property)
	{
		auto val = property.get_value(parent).get_value<Vector3>();
		DisplayImpl(property, "%s: (%f, %f, %f)", val[0], val[1], val[2]);
	}

	void DisplayVector4(rttr::instance parent, const rttr::property& property)
	{
		auto val = property.get_value(parent).get_value<Vector2>();
		DisplayImpl(property, "%s: (%f, %f, %f, %f)", val[0], val[1], val[2], val[3]);
	}

	void Display(rttr::instance parent, const rttr::property& property)
	{
		using DisplayCallback = void(*)(rttr::instance parent, const rttr::property&);
		static MxMap<rttr::type, DisplayCallback> visitor = {
			{ rttr::type::get<bool>(),       DisplayBool       },
			{ rttr::type::get<MxString>(),   DisplayString     },
			{ rttr::type::get<float>(),      DisplayFloat      },
			{ rttr::type::get<Quaternion>(), DisplayQuaternion },
			{ rttr::type::get<Vector2>(),    DisplayVector2    },
			{ rttr::type::get<Vector3>(),    DisplayVector3    },
			{ rttr::type::get<Vector4>(),    DisplayVector4    },
		};
		if (visitor.find(property.get_type()) != visitor.end())
		{
			visitor[property.get_type()](std::move(parent), property);
		}
		else
		{
			MXLOG_WARNING("MxEngine::RuntimeEditor", "no visitor defined to display type: " + MxString(parent.get_type().get_name().cbegin()));
		}
	}

	void EditBool(rttr::instance parent, const rttr::property& property)
	{
		const char* name = property.get_name().cbegin();
		auto val = property.get_value(parent).get_value<bool>();

		bool edited = ImGui::Checkbox(name, &val);
		if (edited)
		{
			property.set_value(parent, val);
		}
	}

	void EditString(rttr::instance parent, const rttr::property& property)
	{
		const char* name = property.get_name().cbegin();
		auto val = property.get_value(parent).get_value<MxString>();

		static MxString text;
		bool edited = GUI::InputTextOnClick(MxFormat("{}: {}", name, val).c_str(), text, 128);
		if (edited)
		{
			property.set_value(parent, text);
		}
	}

	void EditFloat(rttr::instance parent, const rttr::property& property)
	{
		const char* name = property.get_name().cbegin();
		auto val = property.get_value(parent).get_value<float>();

		auto precisionMeta = property.get_metadata(EditorInfo::EDIT_PRECISION);
		auto rangeMeta = property.get_metadata(EditorInfo::EDIT_RANGE);

		auto precision = precisionMeta.is_valid() ? precisionMeta.get_value<float>() : 1.0f;
		auto range = rangeMeta.is_valid() ? rangeMeta.get_value<Range>() : Range{ 0.0f, 0.0f };

		bool edited = ImGui::DragFloat(name, &val, precision, range.Min, range.Max);
		if (edited)
		{
			property.set_value(parent, val);
		}
	}

	void EditQuaternion(rttr::instance parent, const rttr::property& property)
	{
		auto val = property.get_value(parent).get_value<Quaternion>();

		auto precisionMeta = property.get_metadata(EditorInfo::EDIT_PRECISION);
		auto rangeMeta = property.get_metadata(EditorInfo::EDIT_RANGE);

		auto precision = precisionMeta.is_valid() ? precisionMeta.get_value<float>() : 1.0f;
		auto range = rangeMeta.is_valid() ? rangeMeta.get_value<Range>() : Range{ 0.0f, 0.0f };

		auto angles = DegreesVec(MakeEulerAngles(val));
		auto oldAngles = angles;

		bool editedX = ImGui::DragFloat("rotate x", &angles.x, precision, range.Min, range.Max);
		bool editedY = ImGui::DragFloat("rotate y", &angles.y, precision, range.Min, range.Max);
		bool editedZ = ImGui::DragFloat("rotate z", &angles.z, precision, range.Min, range.Max);

		if (editedX) val *= MakeQuaternion(Radians(angles.x - oldAngles.x), MakeVector3(1.0f, 0.0f, 0.0f));
		if (editedY) val *= MakeQuaternion(Radians(angles.y - oldAngles.y), MakeVector3(0.0f, 1.0f, 0.0f));
		if (editedZ) val *= MakeQuaternion(Radians(angles.z - oldAngles.z), MakeVector3(0.0f, 0.0f, 1.0f));

		if (editedX || editedY || editedZ)
		{
			property.set_value(parent, val);
		}
	}

	void EditVector2(rttr::instance parent, const rttr::property& property)
	{
		const char* name = property.get_name().cbegin();
		auto val = property.get_value(parent).get_value<Vector2>();

		auto precisionMeta = property.get_metadata(EditorInfo::EDIT_PRECISION);
		auto rangeMeta = property.get_metadata(EditorInfo::EDIT_RANGE);

		auto precision = precisionMeta.is_valid() ? precisionMeta.get_value<float>() : 1.0f;
		auto range = rangeMeta.is_valid() ? rangeMeta.get_value<Range>() : Range{ 0.0f, 0.0f };

		bool edited = ImGui::DragFloat2(name, &val[0], precision, range.Min, range.Max);
		if (edited)
		{
			property.set_value(parent, val);
		}
	}

	void EditVector3(rttr::instance parent, const rttr::property& property)
	{
		const char* name = property.get_name().cbegin();
		auto val = property.get_value(parent).get_value<Vector3>();

		auto precisionMeta = property.get_metadata(EditorInfo::EDIT_PRECISION);
		auto rangeMeta = property.get_metadata(EditorInfo::EDIT_RANGE);
		auto interpretAsMeta = property.get_metadata(EditorInfo::INTERPRET_AS);

		auto precision = precisionMeta.is_valid() ? precisionMeta.get_value<float>() : 1.0f;
		auto range = rangeMeta.is_valid() ? rangeMeta.get_value<Range>() : Range{ 0.0f, 0.0f };
		auto interpretAs = interpretAsMeta.is_valid() ? interpretAsMeta.get_value<InterpretAsInfo>() : InterpretAsInfo::DEFAULT;

		bool edited = false;
		switch (interpretAs)
		{
		case InterpretAsInfo::COLOR:
			edited = ImGui::ColorEdit3(name, &val[0]);
			break;
		default:
			MXLOG_WARNING("MxEngine::RuntimeEditor", "cannot correctly interpret " + MxString(name));
			// fallthrough
		case InterpretAsInfo::DEFAULT:
			edited = ImGui::DragFloat3(name, &val[0], precision, range.Min, range.Max);
			break;
		}

		if (edited)
		{
			property.set_value(parent, val);
		}
	}

	void EditVector4(rttr::instance parent, const rttr::property& property)
	{
		const char* name = property.get_name().cbegin();
		auto val = property.get_value(parent).get_value<Vector3>();

		auto precisionMeta = property.get_metadata(EditorInfo::EDIT_PRECISION);
		auto rangeMeta = property.get_metadata(EditorInfo::EDIT_RANGE);
		auto interpretAsMeta = property.get_metadata(EditorInfo::INTERPRET_AS);

		auto precision = precisionMeta.is_valid() ? precisionMeta.get_value<float>() : 1.0f;
		auto range = rangeMeta.is_valid() ? rangeMeta.get_value<Range>() : Range{ 0.0f, 0.0f };
		auto interpretAs = interpretAsMeta.is_valid() ? interpretAsMeta.get_value<InterpretAsInfo>() : InterpretAsInfo::DEFAULT;

		bool edited = false;
		switch (interpretAs)
		{
		case InterpretAsInfo::COLOR:
			edited = ImGui::ColorEdit4(name, &val[0]);
			break;
		default:
			MXLOG_WARNING("MxEngine::RuntimeEditor", "cannot correctly interpret " + MxString(name));
			// fallthrough
		case InterpretAsInfo::DEFAULT:
			edited = ImGui::DragFloat4(name, &val[0], precision, range.Min, range.Max);
			break;
		}

		if (edited)
		{
			property.set_value(parent, val);
		}
	}

	void Edit(rttr::instance parent, const rttr::property& property)
	{
		using EditCallback = void(*)(rttr::instance parent, const rttr::property&);
		static MxMap<rttr::type, EditCallback> visitor = {
			{ rttr::type::get<bool>(),       EditBool       },
			{ rttr::type::get<MxString>(),   EditString     },
			{ rttr::type::get<float>(),      EditFloat      },
			{ rttr::type::get<Quaternion>(), EditQuaternion },
			{ rttr::type::get<Vector2>(),    EditVector2    },
			{ rttr::type::get<Vector3>(),    EditVector3    },
			{ rttr::type::get<Vector4>(),    EditVector4    },
		};
		if (visitor.find(property.get_type()) != visitor.end())
		{
			visitor[property.get_type()](std::move(parent), property);
		}
		else
		{
			MXLOG_WARNING("MxEngine::RuntimeEditor", "no visitor defined to display type: " + MxString(parent.get_type().get_name().cbegin()));
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

	template<typename T>
	void ReflectObject(T& object)
	{
		rttr::type t = rttr::type::get<T>();

		TreeNodeManager treeNodeManager;
		for (const auto& property : t.get_properties())
		{
			auto flags = property.get_metadata(MetaInfo::FLAGS).to_uint32();
			if ((flags & MetaInfo::EDITABLE) == 0) continue;

			auto subtree = property.get_metadata(EditorInfo::SUBTREE_NAME);
			treeNodeManager.NewNode(subtree.is_valid() ? subtree.get_value<std::string>() : "");

			if (property.is_readonly())
			{
				Display(rttr::instance{ object }, property);
			}
			else
			{
				Edit(rttr::instance{ object }, property);
			}
		}
		treeNodeManager.EndNodes();

		for (const auto& method : t.get_methods())
		{
			auto flags = method.get_metadata(MetaInfo::FLAGS).to_uint32();
			if ((flags & MetaInfo::EDITABLE) == 0) continue;

			auto subtree = method.get_metadata(EditorInfo::SUBTREE_NAME);
			treeNodeManager.NewNode(subtree.is_valid() ? subtree.get_value<std::string>() : "");

			auto params = method.get_parameter_infos();

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
		treeNodeManager.EndNodes();
	}

	template<typename T>
	void ComponentEditor(T& component)
	{
		rttr::type type = rttr::type::get<T>();
		const char* name = type.get_name().cbegin();

		TREE_NODE_PUSH(name);
		REMOVE_COMPONENT_BUTTON(component);

		ReflectObject(component);
	}

	template<typename T>
	void ResourceEditor(T& resource)
	{
		rttr::type type = rttr::type::get<T>();
		const char* name = type.get_name().cbegin();

		TREE_NODE_PUSH(name);

		ReflectObject(resource);
	}

    void TransformEditor(TransformComponent& transform)
    {
		ResourceEditor(transform);
    }

	void BehaviourEditor(Behaviour& behaviour)
	{
		ComponentEditor(behaviour);
	}
}