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

#include "Core/Components/Physics/RigidBody.h"
#include "Core/Components/Physics/BoxCollider.h"
#include "Core/Components/Physics/SphereCollider.h"
#include "Core/Components/Physics/CapsuleCollider.h"
#include "Core/Components/Physics/CylinderCollider.h"

#include "Utilities/ImGui/ImGuiUtils.h"

namespace MxEngine::GUI
{
	#define REMOVE_COMPONENT_BUTTON(comp) \
	if(ImGui::Button("remove component")) {\
		MxObject::GetByComponent(comp).RemoveComponent<std::remove_reference_t<decltype(comp)>>(); return; }

	void RigidBodyEditor(RigidBody& rigidBody)
	{
		TREE_NODE_PUSH("RigidBody");
		REMOVE_COMPONENT_BUTTON(rigidBody);

		auto inertia = rigidBody.GetInertia();
		auto totalForce = rigidBody.GetTotalForce();
		auto totalTorque = rigidBody.GetTotalTorque();
		auto collisionGroup = rigidBody.GetCollisionGroup();
		auto collisionMask = rigidBody.GetCollisionMask();

		ImGui::Text("is kinematic: %s, is static: %s, is dynamic: %s", 
			BOOL_STRING(rigidBody.IsKinematic()), BOOL_STRING(rigidBody.IsStatic()), BOOL_STRING(rigidBody.IsDynamic()));

		ImGui::Text("collision group: %s, collision mask: %s", 
			EnumToString((CollisionGroup::Group)collisionGroup), 
			EnumToString((CollisionMask::Mask)collisionMask)
		);
		ImGui::Text("total force  applied: (%f, %f, %f)", totalForce.x, totalForce.y, totalForce.z);
		ImGui::Text("total torque applied: (%f, %f, %f)", totalTorque.x, totalTorque.y, totalTorque.z);
		ImGui::Text("body inertia:         (%f, %f, %f)", inertia.x, inertia.y, inertia.z);

		if (ImGui::Button("make dynamic"))
			rigidBody.MakeDynamic();
		ImGui::SameLine();
		if (ImGui::Button("make kinematic"))
			rigidBody.MakeKinematic();
		ImGui::SameLine();
		if (ImGui::Button("make static"))
			rigidBody.MakeStatic();

		if (ImGui::Button("clear forces"))
			rigidBody.ClearForces(); //-V111

		ImGui::SameLine();

		bool isRaycastable = rigidBody.IsRayCastable();
		if (ImGui::Checkbox("raycast enabled", &isRaycastable))
			rigidBody.ToggleRayCasting(isRaycastable);
			
		auto mass = rigidBody.GetMass();
		if (ImGui::DragFloat("mass", &mass, 0.01f))
			rigidBody.SetMass(mass);

		auto nativeBody = rigidBody.GetNativeHandle();
		ImGui::Text("native handle: 0x%p", nativeBody->GetNativeHandle()); //-V111

		if (ImGui::TreeNode("applied forces"))
		{
			auto gravity = rigidBody.GetGravity();
			if (ImGui::DragFloat3("gravity", &gravity[0], 0.01f))
				rigidBody.SetGravity(gravity);

			static auto centralForce = MakeVector3(0.0f);
			if (ImGui::DragFloat3("central force", &centralForce[0]))
				rigidBody.ApplyCentralForce(centralForce);

			static auto centralImpulse = MakeVector3(0.0f);
			if (ImGui::DragFloat3("central impulse", &centralImpulse[0]))
				rigidBody.ApplyCentralImpulse(centralImpulse);

			static auto centralPushImpulse = MakeVector3(0.0f);
			if (ImGui::DragFloat3("central push impulse", &centralPushImpulse[0]))
				rigidBody.ApplyCentralPushImpulse(centralPushImpulse);

			static auto torque = MakeVector3(0.0f);
			if (ImGui::DragFloat3("torque impulse", &torque[0]))
				rigidBody.ApplyTorque(torque);

			static auto turnTorque = MakeVector3(0.0f);
			if (ImGui::DragFloat3("turn torque impulse", &turnTorque[0]))
				rigidBody.ApplyTorqueTurnImpulse(turnTorque);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("velocity"))
		{
			auto linearVelocity = rigidBody.GetLinearVelocity();
			auto angularVelocity = rigidBody.GetAngularVelocity();
			auto pushVelocity = rigidBody.GetPushVelocity();
			auto turnVelocity = rigidBody.GetTurnVelocity();

			if (ImGui::DragFloat3("linear velocity", &linearVelocity[0], 0.01f))
				rigidBody.SetLinearVelocity(linearVelocity);
			if (ImGui::DragFloat3("angular velocity", &angularVelocity[0], 0.01f))
				rigidBody.SetAngularVelocity(angularVelocity);
			if (ImGui::DragFloat3("push velocity", &pushVelocity[0], 0.01f))
				rigidBody.SetPushVelocity(pushVelocity);
			if (ImGui::DragFloat3("turn velocity", &turnVelocity[0], 0.01f))
				rigidBody.SetTurnVelocity(turnVelocity);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("air resistance"))
		{
			auto linearAirResistance = rigidBody.GetLinearAirResistance();
			auto angularAirResistance = rigidBody.GetAngularAirResistance();

			if (ImGui::DragFloat("linear air resistance", &linearAirResistance, 0.01f))
				rigidBody.SetLinearAirResistance(linearAirResistance);
			if (ImGui::DragFloat("angular air resistance", &angularAirResistance, 0.01f))
				rigidBody.SetAngularAirResistance(angularAirResistance);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("force factor"))
		{
			auto linearForceFactor = rigidBody.GetLinearForceFactor();
			auto angularForceFactor = rigidBody.GetAngularForceFactor();

			if (ImGui::DragFloat3("linear force factor", &linearForceFactor[0], 0.01f))
				rigidBody.SetLinearForceFactor(linearForceFactor);
			if (ImGui::DragFloat3("angular force factor", &angularForceFactor[0], 0.01f))
				rigidBody.SetAngularForceFactor(angularForceFactor);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("friction"))
		{
			auto friction = rigidBody.GetFriction();
			auto rollingFriction = rigidBody.GetRollingFriction();
			auto spinningFriction = rigidBody.GetSpinningFriction();
			auto bounceFactor = rigidBody.GetBounceFactor();
			auto anisotropicFriction = rigidBody.GetAnisotropicFriction();

			if (ImGui::DragFloat("friction", &friction, 0.01f))
				rigidBody.SetFriction(friction);
			if (ImGui::DragFloat("rolling friction", &rollingFriction, 0.01f))
				rigidBody.SetRollingFriction(rollingFriction);
			if (ImGui::DragFloat("spinning friction", &spinningFriction, 0.01f))
				rigidBody.SetSpinningFriction(spinningFriction);
			if (ImGui::DragFloat("bounce factor", &bounceFactor, 0.01f))
				rigidBody.SetBounceFactor(bounceFactor);
			if (ImGui::DragFloat3("anisotropic friction", &anisotropicFriction[0], 0.01f))
				rigidBody.SetAnisotropicFriction(anisotropicFriction);
			ImGui::TreePop();
		}
	}

	void BoxColliderEditor(BoxCollider& boxCollider)
	{
		TREE_NODE_PUSH("BoxCollider");
		REMOVE_COMPONENT_BUTTON(boxCollider);

		auto boundingBox = boxCollider.GetNativeHandle()->GetBoundingBoxUnchanged();
		DrawBoxEditor("bounding box", boundingBox);
		boxCollider.SetBoundingBox(boundingBox);
	}

	void SphereColliderEditor(SphereCollider& sphereCollider) //-V111
	{
		TREE_NODE_PUSH("SphereCollider");
		REMOVE_COMPONENT_BUTTON(sphereCollider);

		auto boundingSphere = sphereCollider.GetNativeHandle()->GetBoundingSphereUnchanged();
		DrawSphereEditor("bounding sphere", boundingSphere);
		sphereCollider.SetBoundingSphere(boundingSphere);
	}

	void CylinderColliderEditor(CylinderCollider& cylinderCollider) //-V111
	{
		TREE_NODE_PUSH("CylinderCollider");
		REMOVE_COMPONENT_BUTTON(cylinderCollider);

		auto boundingCylinder = cylinderCollider.GetNativeHandle()->GetBoundingCylinderUnchanged();
		DrawCylinderEditor("bounding cylinder", boundingCylinder);
		cylinderCollider.SetBoundingCylinder(boundingCylinder);
	}

	void CapsuleColliderEditor(CapsuleCollider& capsuleCollider) //-V111
	{
		TREE_NODE_PUSH("CapsuleCollider");
		REMOVE_COMPONENT_BUTTON(capsuleCollider);

		auto boundingCapsule = capsuleCollider.GetNativeHandle()->GetBoundingCapsuleUnchanged();
		DrawCapsuleEditor("bounding capsule", boundingCapsule);
		capsuleCollider.SetBoundingCapsule(boundingCapsule);
	}
}