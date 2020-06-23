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

#include "Json.h"

using namespace MxEngine;

void nlohmann::to_json(JsonFile& j, const Vector3& v)
{
    j = JsonFile{ v.x, v.y, v.z };
}

void nlohmann::from_json(const JsonFile& j, Vector3& v)
{
    auto a = j.get<std::array<float, 3>>();
    v = MakeVector3(a[0], a[1], a[2]);
}

void nlohmann::to_json(JsonFile& j, const Vector2& v)
{
    j = JsonFile{ v.x, v.y };
}

void nlohmann::from_json(const JsonFile& j, Vector2& v)
{
    auto a = j.get<std::array<float, 2>>();
    v = MakeVector2(a[0], a[1]);
}

void nlohmann::to_json(JsonFile& j, const MxString& str)
{
    j = str.c_str();
}

void nlohmann::from_json(const JsonFile& j, MxString& str)
{
    auto s = j.get<std::string>();
    str = ToMxString(s);
}

void nlohmann::to_json(JsonFile& j, KeyCode key)
{
    j = EnumToString(key);
}

void nlohmann::from_json(const JsonFile& j, KeyCode& key)
{
    auto str = j.get<std::string>();
    #define JSONKEY(val) if(str == #val) { key = MxEngine::KeyCode::val; return; }
   
    JSONKEY(UNKNOWN);
    JSONKEY(SPACE);
    JSONKEY(APOSTROPHE);
    JSONKEY(COMMA);
    JSONKEY(MINUS);
    JSONKEY(PERIOD);
    JSONKEY(SLASH);
    JSONKEY(D0);
    JSONKEY(D1);
    JSONKEY(D2);
    JSONKEY(D3);
    JSONKEY(D4);
    JSONKEY(D5);
    JSONKEY(D6);
    JSONKEY(D7);
    JSONKEY(D8);
    JSONKEY(D9);
    JSONKEY(SEMICOLON);
    JSONKEY(EQUAL);
    JSONKEY(A);
    JSONKEY(B);
    JSONKEY(C);
    JSONKEY(D);
    JSONKEY(E);
    JSONKEY(F);
    JSONKEY(G);
    JSONKEY(H);
    JSONKEY(I);
    JSONKEY(J);
    JSONKEY(K);
    JSONKEY(L);
    JSONKEY(M);
    JSONKEY(N);
    JSONKEY(O);
    JSONKEY(P);
    JSONKEY(Q);
    JSONKEY(R);
    JSONKEY(S);
    JSONKEY(T);
    JSONKEY(U);
    JSONKEY(V);
    JSONKEY(W);
    JSONKEY(X);
    JSONKEY(Y);
    JSONKEY(Z);
    JSONKEY(LEFT_BRACKET);
    JSONKEY(BACKSLASH);
    JSONKEY(RIGHT_BRACKET);
    JSONKEY(GRAVE_ACCENT);
    JSONKEY(WORLD_1);
    JSONKEY(ESCAPE);
    JSONKEY(ENTER);
    JSONKEY(TAB);
    JSONKEY(BACKSPACE);
    JSONKEY(INSERT);
    JSONKEY(DELETE);
    JSONKEY(RIGHT);
    JSONKEY(LEFT);
    JSONKEY(DOWN);
    JSONKEY(UP);
    JSONKEY(PAGE_UP);
    JSONKEY(PAGE_DOWN);
    JSONKEY(HOME);
    JSONKEY(END);
    JSONKEY(CAPS_LOCK);
    JSONKEY(SCROLL_LOCK);
    JSONKEY(NUM_LOCK);
    JSONKEY(PRINT_SCREEN);
    JSONKEY(PAUSE);
    JSONKEY(F1);
    JSONKEY(F2);
    JSONKEY(F3);
    JSONKEY(F4);
    JSONKEY(F5);
    JSONKEY(F6);
    JSONKEY(F7);
    JSONKEY(F8);
    JSONKEY(F9);
    JSONKEY(F10);
    JSONKEY(F11);
    JSONKEY(F12);
    JSONKEY(F13);
    JSONKEY(F14);
    JSONKEY(F15);
    JSONKEY(F16);
    JSONKEY(F17);
    JSONKEY(F18);
    JSONKEY(F19);
    JSONKEY(F20);
    JSONKEY(F21);
    JSONKEY(F22);
    JSONKEY(F23);
    JSONKEY(F24);
    JSONKEY(F25);
    JSONKEY(KP_0);
    JSONKEY(KP_1);
    JSONKEY(KP_2);
    JSONKEY(KP_3);
    JSONKEY(KP_4);
    JSONKEY(KP_5);
    JSONKEY(KP_6);
    JSONKEY(KP_7);
    JSONKEY(KP_8);
    JSONKEY(KP_9);
    JSONKEY(KP_DECIMAL);
    JSONKEY(KP_DIVIDE);
    JSONKEY(KP_MULTIPLY);
    JSONKEY(KP_SUBTRACT);
    JSONKEY(KP_ADD);
    JSONKEY(KP_ENTER);
    JSONKEY(KP_EQUAL);
    JSONKEY(LEFT_SHIFT);
    JSONKEY(LEFT_CONTROL);
    JSONKEY(LEFT_ALT);
    JSONKEY(LEFT_SUPER);
    JSONKEY(RIGHT_SHIFT);
    JSONKEY(RIGHT_CONTROL);
    JSONKEY(RIGHT_ALT);
    JSONKEY(RIGHT_SUPER);
    JSONKEY(MENU);
}