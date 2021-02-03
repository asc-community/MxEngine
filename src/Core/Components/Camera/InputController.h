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

#pragma once

#include "Utilities/ECS/Component.h"
#include "Core/Events/KeyEvent.h"
#include "Utilities/Math/Math.h"

namespace MxEngine
{
	class InputController
	{
		MAKE_COMPONENT(InputController);

		Vector3 motion{ 0.0f };
		std::array<KeyCode, 6> keybindings = {
			KeyCode::UNKNOWN, KeyCode::UNKNOWN, KeyCode::UNKNOWN, KeyCode::UNKNOWN, KeyCode::UNKNOWN, KeyCode::UNKNOWN,
		};
		bool bindHorizontalRotation = false, bindVerticalRotation = false, bindMovement = false;
		
		void BindMovementCallback();
		void BindRotationCallback();
	public:
		InputController() = default;
		~InputController();

		void BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right);
		void BindMovement(KeyCode forward, KeyCode left, KeyCode back, KeyCode right, KeyCode up, KeyCode down);
		void BindMovementWASD();
		void BindMovementWASDSpaceShift();
		void BindRotation();
		void BindHorizontalRotation();
		void BindVerticalRotation();
		void UnbindAll();

		const Vector3& GetMotionVector() const;
		KeyCode GetForwardKeyBinding() const;
		KeyCode GetBackKeyBinding() const;
		KeyCode GetLeftKeyBinding() const;
		KeyCode GetRightKeyBinding() const;
		KeyCode GetUpKeyBinding() const;
		KeyCode GetDownKeyBinding() const;
		bool IsVerticalRotationBound() const;
		bool IsHorizontalRotationBound() const;

		void SetForwardKeyBinding(KeyCode key);
		void SetBackKeyBinding(KeyCode key);
		void SetLeftKeyBinding(KeyCode key);
		void SetRightKeyBinding(KeyCode key);
		void SetUpKeyBinding(KeyCode key);
		void SetDownKeyBinding(KeyCode key);
		void ToggleVerticalRotationBound(bool value);
		void ToggleHorizontalRotationBound(bool value);
	};
}