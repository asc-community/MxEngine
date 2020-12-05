
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
    void to_json(MxEngine::JsonFile& j, const MxEngine::Vector4& v);
    void from_json(const MxEngine::JsonFile& j, MxEngine::Vector4& v);
    void to_json(MxEngine::JsonFile& j, const MxEngine::Vector3& v);
    void from_json(const MxEngine::JsonFile& j, MxEngine::Vector3& v);
    void to_json(MxEngine::JsonFile& j, const MxEngine::Vector2& v);
    void from_json(const MxEngine::JsonFile& j, MxEngine::Vector2& v);
    void to_json(MxEngine::JsonFile& j, const MxEngine::Quaternion& q);
    void from_json(const MxEngine::JsonFile& j, MxEngine::Quaternion& q);
}

namespace eastl
{
    void to_json(MxEngine::JsonFile& j, const MxEngine::MxString& s);
    void from_json(const MxEngine::JsonFile& j, MxEngine::MxString& s);
}

namespace std::filesystem
{
    void to_json(MxEngine::JsonFile& j, const MxEngine::FilePath& p);
    void from_json(const MxEngine::JsonFile& j, MxEngine::FilePath& p);
}