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