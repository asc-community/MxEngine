
#include <nlohmann/json.hpp>
#include "Utilities/FileSystem/File.h"
#include "Utilities/Math/Math.h"

namespace MxEngine
{
    using JsonFile = nlohmann::json;

    JsonFile LoadJson(File& file);

    void SaveJson(File& file, const JsonFile& json);
}

namespace glm
{
    void to_json(MxEngine::JsonFile& j, const MxEngine::Vector3& v);
    void from_json(const MxEngine::JsonFile& j, MxEngine::Vector3& v);
    void to_json(MxEngine::JsonFile& j, const MxEngine::Vector2& v);
    void from_json(const MxEngine::JsonFile& j, MxEngine::Vector2& v);
}

namespace eastl
{
    void to_json(MxEngine::JsonFile& j, const MxEngine::MxString& s);
    void from_json(const MxEngine::JsonFile& j, MxEngine::MxString& s);
}