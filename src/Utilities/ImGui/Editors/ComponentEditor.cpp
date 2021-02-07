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

#include "ComponentEditor.h"
#include "Utilities/Format/Format.h"
#include "Utilities/ImGui/ImGuiUtils.h"
#include "Utilities/STL/MxMap.h"
#include "Core/Runtime/DereferenceHandle.h"

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

	void Display(const char* name, int i, const ReflectionMeta& meta)
	{
		ImGui::Text("%s: %d", name, i);
	}

	void Display(const char* name, size_t i, const ReflectionMeta& meta)
	{
		Display(name, (int)i, meta);
	}

	void Display(const char* name, unsigned int i, const ReflectionMeta& meta)
	{
		Display(name, (int)i, meta);
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

	void VisitDisplay(const char* name, const rttr::variant& v, const ReflectionMeta& meta);
	rttr::variant VisitEdit(const char* name, const rttr::variant& v, const ReflectionMeta& meta);

	void DisplaySequentialContainer(const char* name, const rttr::variant& val, const ReflectionMeta& meta)
	{
		auto view = val.create_sequential_view();

		ImGui::Text("%s", name);
		ImGui::Indent(5.0f);
		int id = 0;
		for (const auto& element : view)
		{
			if (meta.Editor.EditRange.Min != 0.0f && (int)meta.Editor.EditRange.Min > id) continue;
			if (meta.Editor.EditRange.Max != 0.0f && (int)meta.Editor.EditRange.Max < id) continue;

			ImGui::PushID(id++);
			auto elementValue = element.extract_wrapped_value();
			auto elementType = elementValue.get_type();
			VisitDisplay("", elementValue, ReflectionMeta{ elementType });
			ImGui::PopID();
			ImGui::Separator();
		}
		ImGui::Unindent(5.0f);
	}

	void DisplayEnumeration(const char* name, const rttr::variant& val, const ReflectionMeta& meta)
	{
		auto t = val.get_type().get_enumeration();
		const char* text = t.value_to_name(val).cbegin();
		ImGui::Text("%s: %s", name, text);
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

	rttr::variant Edit(const char* name, int val, const ReflectionMeta& meta)
	{
		bool edited = ImGui::DragInt(name, &val, meta.Editor.EditPrecision, int(meta.Editor.EditRange.Min), int(meta.Editor.EditRange.Max));
		return edited ? rttr::variant{ val } : rttr::variant{ };
	}

	rttr::variant Edit(const char* name, uint64_t val, const ReflectionMeta& meta)
	{
		auto editVal = (int)val;
		bool edited = ImGui::DragInt(name, &editVal, meta.Editor.EditPrecision, int(meta.Editor.EditRange.Min), int(meta.Editor.EditRange.Max));
		return edited ? rttr::variant{ (uint64_t)Max(editVal, 0) } : rttr::variant{ };
	}

	rttr::variant Edit(const char* name, uint32_t val, const ReflectionMeta& meta)
	{
		auto editVal = (int)val;
		bool edited = ImGui::DragInt(name, &editVal, meta.Editor.EditPrecision, int(meta.Editor.EditRange.Min), int(meta.Editor.EditRange.Max));
		return edited ? rttr::variant{ (uint32_t)Max(editVal, 0) } : rttr::variant{ };
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

	rttr::variant EditSequentialContainer(const char* name, const rttr::variant& val, const ReflectionMeta& meta)
	{
		rttr::variant arrayResult{ };
		auto view = val.create_sequential_view();

		ImGui::Text("%s", name);
		ImGui::Indent(5.0f);
		int id = 0;
		for (size_t i = 0, size = view.get_size(); i < size; i++)
		{
			if (meta.Editor.EditRange.Min != 0.0f && (int)meta.Editor.EditRange.Min > id) continue;
			if (meta.Editor.EditRange.Max != 0.0f && (int)meta.Editor.EditRange.Max < id) continue;

			ImGui::PushID(id++);
			auto elementValue = view.get_value(i).extract_wrapped_value();
			auto elementType = elementValue.get_type();
			auto result = VisitEdit("", elementValue, ReflectionMeta{ elementType });
			if (result.is_valid())
			{
				view.set_value(i, result);
				arrayResult = val;
			}
			ImGui::PopID();
			ImGui::Separator();
		}
		ImGui::Unindent(5.0f);

		return arrayResult;
	}

	rttr::variant EditEnumeration(const char* name, const rttr::variant& v, const ReflectionMeta& meta)
	{
		auto t = v.get_type().get_enumeration();
		const char* currentEnumName = t.value_to_name(v).cbegin();

		if (ImGui::BeginCombo(name, currentEnumName))
		{
			for (const auto& enumName : t.get_names())
			{
				auto enumValue = t.name_to_value(enumName);
				if (ImGui::Selectable(enumName.cbegin(), enumValue == v))
				{
					ImGui::SetItemDefaultFocus();
					ImGui::EndCombo();
					return enumValue;
				}
			}
			ImGui::EndCombo();
		}
		return rttr::variant{ };
	}

	void VisitDisplay(const char* name, const rttr::variant& v, const ReflectionMeta& meta)
	{
		#define VISITOR_DISPLAY_ENTRY(TYPE) { rttr::type::get<TYPE>(), DisplayGeneric<TYPE> }
		using DisplayCallback = void(*)(const char*, const rttr::variant&, const ReflectionMeta&);
		static MxMap<rttr::type, DisplayCallback> visitor = {
			VISITOR_DISPLAY_ENTRY(bool),
			VISITOR_DISPLAY_ENTRY(MxString),
			VISITOR_DISPLAY_ENTRY(float),
			VISITOR_DISPLAY_ENTRY(int),
			VISITOR_DISPLAY_ENTRY(unsigned int),
			VISITOR_DISPLAY_ENTRY(size_t),
			VISITOR_DISPLAY_ENTRY(Quaternion),
			VISITOR_DISPLAY_ENTRY(Vector2),
			VISITOR_DISPLAY_ENTRY(Vector3),
			VISITOR_DISPLAY_ENTRY(Vector4),
		};

		auto t = v.get_type();
		if (visitor.find(t) != visitor.end())
		{
			visitor[t](name, v, meta);
		}
		else if (v.is_sequential_container())
		{
			DisplaySequentialContainer(name, v, meta);
		}
		else if (t.is_enumeration())
		{
			DisplayEnumeration(name, v, meta);
		}
		else
		{
			ResourceEditor(name, v);
		}
	}

	template<typename T>
	rttr::variant EditGeneric(const char* name, const rttr::variant& v, const ReflectionMeta& meta)
	{
		return Edit(name, v.template get_value<T>(), meta);
	}

	rttr::variant VisitEdit(const char* name, const rttr::variant& v, const ReflectionMeta& meta)
	{
		#define VISITOR_EDIT_ENTRY(TYPE) { rttr::type::get<TYPE>(), EditGeneric<TYPE> }
		using EditCallback = rttr::variant(*)(const char*, const rttr::variant&, const ReflectionMeta&);
		static MxMap<rttr::type, EditCallback> visitor = {
			VISITOR_EDIT_ENTRY(bool),
			VISITOR_EDIT_ENTRY(MxString),
			VISITOR_EDIT_ENTRY(float),
			VISITOR_EDIT_ENTRY(int),
			VISITOR_EDIT_ENTRY(unsigned int),
			VISITOR_EDIT_ENTRY(size_t),
			VISITOR_EDIT_ENTRY(Quaternion),
			VISITOR_EDIT_ENTRY(Vector2),
			VISITOR_EDIT_ENTRY(Vector3),
			VISITOR_EDIT_ENTRY(Vector4),
		};

		auto t = v.get_type();
		if (visitor.find(t) != visitor.end())
		{
			return visitor[t](name, v, meta);
		}
		else if(v.is_sequential_container())
		{
			return EditSequentialContainer(name, v, meta);
		}
		else if (t.is_enumeration())
		{
			return EditEnumeration(name, v, meta);
		}
		else
		{
			return ResourceEditor(name, v);
		}
	}

	rttr::variant ReflectObject(rttr::instance maybeHandle)
	{
		rttr::variant result{ };
		auto [object, type, handleId] = DereferenceHandle(maybeHandle);

		ReflectionMeta typeMeta(type);
		if (typeMeta.Editor.HandleEditor != nullptr)
		{
			result = typeMeta.Editor.HandleEditor(maybeHandle);
		}

		if (!object.is_valid() || result.is_valid())
		{
			ImGui::Text("empty");
			return result;
		}

		for (const auto& method : type.get_methods())
		{
			ReflectionMeta meta(method);

			if ((meta.Flags & MetaInfo::EDITABLE) == 0) continue;
			if (meta.Condition != nullptr && !meta.Condition(object)) continue;

			auto params = method.get_parameter_infos();

			if (meta.Editor.CustomView != nullptr)
			{
				meta.Editor.CustomView(object);
			}
			else
			{
				if (params.empty())
				{
					if (ImGui::Button(method.get_name().cbegin()))
					{
						method.invoke(object);
						if (typeMeta.CopyFunction != nullptr)
							result = typeMeta.CopyFunction(object);
					}
				}
				else
				{
					ImGui::Text("cannot add method call as it requires additional arguments");
				}
			}
		}

		for (const auto& property : type.get_properties())
		{
			ReflectionMeta meta(property);

			if ((meta.Flags & MetaInfo::EDITABLE) == 0) continue;
			if (meta.Condition != nullptr && !meta.Condition(object)) continue;

			if (meta.Editor.CustomView != nullptr)
			{
				auto editedValue = meta.Editor.CustomView(object);
				if (!property.is_readonly() && editedValue.is_valid())
					result = std::move(editedValue);
			}
			else
			{
				const char* name = property.get_name().cbegin();
				if (property.is_readonly())
				{
					VisitDisplay(name, property.get_value(object), meta);
				}
				else
				{
					auto editedValue = VisitEdit(name, property.get_value(object), meta);
					if (editedValue.is_valid())
					{
						property.set_value(object, editedValue);
						if (typeMeta.CopyFunction != nullptr)
							result = typeMeta.CopyFunction(object);
					}
				}
			}
		}
		return result;
	}

	void ComponentEditorImpl(const char* name, rttr::instance component, void(*removeCallback)(rttr::instance))
	{
		if (ImGui::TreeNode(name))
		{
			if (ImGui::Button("remove component"))
			{
				removeCallback(component);
				ImGui::TreePop();
				return;
			}
			(void)ReflectObject(component);
			ImGui::TreePop();
		}
	}

	rttr::variant ResourceEditor(const char* name, rttr::instance object)
	{
		rttr::variant result{ };
		if (strcmp(name, "") != 0)
		{
			if (ImGui::TreeNode(name))
			{
				result = ReflectObject(object);
				ImGui::TreePop();
			}
		}
		else
		{
			result = ReflectObject(object);
		}
		return result;
	}
}