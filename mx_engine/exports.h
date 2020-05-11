// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

#include <MxEngine.h>
#include <Library/Scripting/Python/PythonEngine.h>
#include <Library/Primitives/Primitives.h>
#include <Library/Bindings/Bindings.h>

#define BOOST_PYTHON_STATIC_LIB
#include <boost/python.hpp>
#include <sstream>

namespace py = boost::python;

using namespace MxEngine;

template<typename T>
T& LightContainerIndexGetWrapper(Scene::LightContainer<T>& v, int index)
{
    if (index >= 0 && index < v.GetCount()) {
        return v[index];
    }
    else {
        ::PyErr_SetString(::PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
        return (*(T*)nullptr); // execution wont go here because of exception
    }
}

template <typename T, typename U = typename T::value_type>
void VectorIndexSetWrapper(T& v, int index, U value)
{
    if (index >= 0 && index < v.length()) {
        v[index] = value;
    }
    else {
        ::PyErr_SetString(::PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
    }
}

template <typename T>
T StdVectorGetWrapper(std::vector<T>& v, int index)
{
    if (index >= 0 && index < v.size()) {
        return v[index];
    }
    else {
        ::PyErr_SetString(::PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
        return v[0]; // execution wont go here because of exception
    }
}

template <typename T>
T& StdVectorGetRefWrapper(std::vector<T>& v, int index)
{
    if (index >= 0 && index < v.size()) {
        return v[index];
    }
    else {
        ::PyErr_SetString(::PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
        return v[0]; // execution wont go here because of exception
    }
}

template <typename T>
void StdVectorSetWrapper(std::vector<T>& v, int index, T& value)
{
    if (index >= 0 && index < v.size()) {
        v[index] = value;
    }
    else {
        ::PyErr_SetString(::PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
    }
}

template <typename T>
auto VectorIndexGetWrapper(T& v, int index)
{
    if (index >= 0 && index < v.length()) {
        return v[index];
    }
    else {
        ::PyErr_SetString(::PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
        return v[0]; // execution wont go here because of exception
    }
}

template<typename Matrix>
void MatrixIndexSetWrapper(Matrix& m, int index, const Vector4& value)
{
    if (index >= 0 && index < m.length()) {
        m[index] = value;
    }
    else {
        ::PyErr_SetString(::PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
    }
}

template<typename Matrix>
auto& MatrixIndexGetWrapper(Matrix& m, int index)
{
    if (index >= 0 && index < m.length()) {
        return m[index];
    }
    else {
        ::PyErr_SetString(::PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
        return m[0]; // execution wont go here because of exception
    }
}

template<typename Vector>
std::string VectorToString(const Vector& v)
{
    std::stringstream out;
    out << '[';
    for (size_t i = 0; i < v.length(); i++)
    {
        if (i != 0) out << ", ";
        out << v[(Vector::length_type)i];
    }
    out << ']';
    return out.str();
}

template<typename Matrix>
std::string MatrixToString(const Matrix& m)
{
    std::stringstream out;
    out << '[';
    for (int i = 0; i < m.length(); i++)
    {
        if (i != 0) out << ", ";
        out << VectorToString(m[i]);
    }
    out << ']';
    return out.str();
}

void SetPerspectiveCamera(CameraController& controller)
{
    Application::Get()->GetRenderer().ToggleReversedDepth(true);
    controller.SetCamera(MakeUnique<PerspectiveCamera>());
}

void SetOrthographicCamera(CameraController& controller)
{
    Application::Get()->GetRenderer().ToggleReversedDepth(false);
    controller.SetCamera(MakeUnique<OrthographicCamera>());
}

template<typename... Args>
void InvokePythonFunction(const py::object& func, Args&&... args)
{
    try
    {
        func(std::forward<Args>(args)...);
    }
    catch (py::error_already_set&)
    {
        ::PyErr_Print();
        std::string error;
        try
        {
            auto dict = py::import("__main__").attr("__dict__");
            py::object msg = dict["errorHandler"].attr("value");
            error = py::extract<std::string>(msg);
        }
        catch (python::error_already_set&)
        {
            error = "cannot get python error. Probably python module is not initialized correctly\n";
        }
        if (!error.empty()) error.pop_back(); // delete last '\n'
        ::PyErr_Clear();
        Application::Get()->GetConsole().Log("[error]: " + error);
    }
}

class PyApplication : public Application
{
public:
    py::object createCallback, updateCallback, destroyCallback;

    virtual void OnCreate() override 
    { 
        auto& scriptEngine = this->GetConsole().GetEngine();
        scriptEngine.MirrorOutStream(true);
        scriptEngine.MirrorErrorStream(true);

        if (this->createCallback)
            InvokePythonFunction(this->createCallback);
    }
    virtual void OnUpdate() override
    { 
        if(this->updateCallback) 
            InvokePythonFunction(this->updateCallback);
    }
    virtual void OnDestroy() override 
    { 
        if(this->destroyCallback) 
            InvokePythonFunction(this->destroyCallback);
    }
};

class PyScene : public Scene
{
public:
    py::object createCallback, loadCallback, unloadCallback, updateCallback, renderCallback, destroyCallback;

    PyScene(const std::string& name, const std::string& directory)
        : Scene(name, directory) { }

    virtual void OnCreate() override
    {
        if (this->createCallback)
            this->createCallback();
    }

    virtual void OnLoad() override
    {
        if (this->loadCallback)
            this->loadCallback();
    }

    virtual void OnUnload() override
    {
        if (this->unloadCallback)
            this->unloadCallback();
    }

    virtual void OnUpdate() override
    {
        if (this->updateCallback)
            this->updateCallback();
    }

    virtual void OnRender() override
    {
        if (this->renderCallback)
            this->renderCallback();
    }

    virtual void OnDestroy() override
    {
        if (this->destroyCallback)
            this->destroyCallback();
    }
};

class VertexBufferWrapper : public VertexBuffer, public py::wrapper<VertexBuffer>
{
public:
    virtual void Bind() const override
    {
        this->get_override("bind")();
    }

    virtual void Unbind() const override
    {
        this->get_override("unbind")();
    }

    virtual void Load(BufferData data, size_t count, UsageType type) override
    {
        this->get_override("load")(data, count, type);
    }

    virtual void BufferSubData(BufferData data, size_t count, size_t offset = 0) override
    {
        this->get_override("buffer")(data, count, offset);
    }

    virtual size_t GetSize() const override
    {
        return this->get_override("size")();
    }
};

class VertexArrayWrapper : public VertexArray, public py::wrapper<VertexArray>
{
public:
    virtual void Bind() const override
    {
        this->get_override("bind")();
    }

    virtual void Unbind() const override
    {
        this->get_override("unbind")();
    }

    virtual void AddBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout)
    {
        this->get_override("add_buffer")(buffer, layout);
    }

    virtual void AddInstancedBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout)
    {
        this->get_override("add_instanced_buffer")(buffer, layout);
    }

    virtual int GetAttributeCount() const override
    {
        return this->get_override("attribute_count")();
    }
};

class VertexBufferLayoutWrapper : public VertexBufferLayout, public py::wrapper<VertexBufferLayout>
{
public:
    virtual const ElementBuffer& GetElements() const override
    {
        return this->get_override("elements")();
    }

    virtual StrideType GetStride() const override
    {
        return this->get_override("stride")();
    }

    virtual void PushFloat(size_t count) override
    {
        this->get_override("push")(count);
    }
};

class IndexBufferWrapper : public IndexBuffer, public py::wrapper<IndexBuffer>
{
public:
    virtual void Bind() const override
    {
        this->get_override("bind")();
    }

    virtual void Unbind() const override
    {
        this->get_override("unbind")();
    }

    virtual void Load(IndexData data, size_t count) override
    {
        this->get_override("load")(data, count);
    }

    virtual size_t GetCount() const override
    {
        return this->get_override("count")();
    }

    virtual size_t GetIndexTypeId() const override
    {
        return this->get_override("type_id")();
    }
};

class TextureWrapper : public Texture, public py::wrapper<Texture>
{
    virtual void Load(const std::string& filepath, TextureWrap wrap = TextureWrap::REPEAT, bool genMipmaps = true, bool flipImage = true) override
    {
        this->get_override("load")(filepath, wrap, genMipmaps, flipImage);
    }

    virtual void LoadMipmaps(RawDataPointer* data, size_t mipmaps, int width, int height, TextureWrap wrap = TextureWrap::REPEAT) override
    {
        this->get_override("load_mipmaps")(data, mipmaps, width, height, wrap);
    }

    virtual void LoadDepth(int width, int height, TextureWrap wrap = TextureWrap::CLAMP_TO_EDGE) override
    {
        this->get_override("load_depth")(width, height, wrap);
    }

    virtual void LoadMultisample(int width, int height, TextureFormat format, int samples, TextureWrap wrap = TextureWrap::REPEAT) override
    {
        this->get_override("load_multisample")(width, height, format, samples, wrap);
    }

    virtual void Load(RawDataPointer data, int width, int height, TextureFormat format = TextureFormat::RGBA, TextureWrap wrap = TextureWrap::REPEAT, bool genMipmaps = true) override
    {
        this->get_override("load_raw")(data, width, height, format, wrap, genMipmaps);
    }

    virtual void Bind(IBindable::IdType id) const override
    {
        this->get_override("bind")(id);
    }

    virtual const std::string& GetPath() const override
    {
        return this->get_override("path")();
    }

    virtual size_t GetWidth() const override
    {
        return this->get_override("width")();
    }

    virtual size_t GetHeight() const override
    {
        return this->get_override("height")();
    }

    virtual size_t GetChannelCount() const override
    {
        return this->get_override("channels")();
    }

    virtual unsigned int GetTextureType() const override
    {
        return this->get_override("type")();
    }

    virtual void Bind() const override
    {
        this->get_override("bind")();
    }

    virtual void Unbind() const override
    {
        this->get_override("unbind")();
    }
};

class ShaderWrapper : public Shader, public py::wrapper<Shader>
{
public:
    virtual void Load(const std::string& vertex, const std::string& geometry, const std::string& fragment) override
    {
        this->get_override("load")(vertex, geometry, fragment);
    }

    virtual void Load(const std::string& vertex, const std::string& fragment) override
    {
        this->get_override("load")(vertex, fragment);
    }

    virtual void LoadFromString(const std::string& vertex, const std::string& geometry, const std::string& fragment) override
    {
        this->get_override("load_source")(vertex, geometry, fragment);
    }

    virtual void LoadFromString(const std::string& vertex, const std::string& fragment) override
    {
        this->get_override("load_source")(vertex, fragment);
    }

    virtual void SetUniformFloat(const std::string& name, float f) const override
    {
        this->get_override("set_float")(name, f);
    }

    virtual void SetUniformVec2(const std::string& name, const Vector2& vec) const override
    {
        this->get_override("set_vec2")(name, vec);
    }

    virtual void SetUniformVec3(const std::string& name, const Vector3& vec) const override
    {
        this->get_override("set_vec3")(name, vec);
    }

    virtual void SetUniformVec4(const std::string& name, const Vector4& vec) const override
    {
        this->get_override("set_vec4")(name, vec);
    }

    virtual void SetUniformMat4(const std::string& name, const Matrix4x4& matrix) const override
    {
        this->get_override("set_mat4")(name, matrix);
    }

    virtual void SetUniformMat3(const std::string& name, const Matrix3x3& matrix) const override
    {
        this->get_override("set_mat3")(name, matrix);
    }

    virtual void SetUniformInt(const std::string& name, int i) const override
    {
        this->get_override("set_int")(name, i);
    }

    virtual void Bind() const override
    {
        this->get_override("bind")();
    }

    virtual void Unbind() const override
    {
        this->get_override("unbind")();
    }
};

class IBindableWrapper : public IBindable, public py::wrapper<IBindable>
{
public:
    virtual void Bind() const override
    {
        this->get_override("bind")();
    }

    virtual void Unbind() const override
    {
        this->get_override("unbind")();
    }
};

class IMovableWrapper : public IMovable, public py::wrapper<IMovable>
{
public:
    virtual IMovable& Translate(float x, float y, float z) override
    {
        return (IMovable&)this->get_override("move")(x, y, z);
    }

    virtual IMovable& TranslateForward(float dist) override
    {
        return (IMovable&)this->get_override("move_forward")(dist);
    }

    virtual IMovable& TranslateRight(float dist) override
    {
        return (IMovable&)this->get_override("move_right")(dist);
    }

    virtual IMovable& TranslateUp(float dist) override
    {
        return (IMovable&)this->get_override("move_up")(dist);
    }

    virtual IMovable& Rotate(float horz, float vert)
    {
        return (IMovable&)this->get_override("rotate")(horz, vert);
    }

    virtual const Vector3& GetForwardVector() const override
    {
        return this->get_override("vec_forward")();
    }

    virtual const Vector3& GetUpVector() const override
    {
        return this->get_override("vec_up")();
    }

    virtual const Vector3& GetRightVector() const override
    {
        return this->get_override("vec_right")();
    }
};

class ICameraWrapper : public ICamera, public py::wrapper<ICamera>
{
public:
    virtual const Matrix4x4& GetViewMatrix() const override
    {
        return this->get_override("view_matrix")();
    }

    virtual const Matrix4x4& GetProjectionMatrix() const override
    {
        return this->get_override("projection_matrix")();
    }

    virtual const Matrix4x4& GetMatrix() const override
    {
        return this->get_override("matrix")();
    }

    virtual void SetViewMatrix(const Matrix4x4& view) override
    {
        this->get_override("set_view")(view);
    }

    virtual void SetAspectRatio(float w, float h = 1.0f) override
    {
        this->get_override("aspect_ratio")(w, h);
    }

    virtual float GetAspectRatio() const override
    {
        return this->get_override("aspect_ratio")();
    }

    virtual void SetZNear(float zNear) override
    {
        this->get_override("znear")(zNear);
    }

    virtual void SetZFar(float zFar) override
    {
        this->get_override("zfar")(zFar);
    }

    virtual float GetZNear() const override
    {
        return this->get_override("znear")();
    }

    virtual float GetZFar() const override
    {
        return this->get_override("zfar")();
    }

    virtual void SetZoom(float zoom) override
    {
        this->get_override("zoom")(zoom);
    }

    virtual float GetZoom() const override
    {
        return this->get_override("zoom")();
    }
};

struct FilePathToPyString
{
    static PyObject* convert(const FilePath& path)
    {
        return ::PyUnicode_FromString(path.string().c_str());
    }
};

struct StringToPyString
{
    static PyObject* convert(const std::string& s)
    {
        return ::PyUnicode_FromString(s.c_str());
    }
};

struct FilePathFromPyString
{
    FilePathFromPyString()
    {
        py::converter::registry::push_back(
            &convertible,
            &construct,
            py::type_id<FilePath>());
    }

    static void* convertible(PyObject* obj_ptr)
    {
        if (!PyUnicode_Check(obj_ptr)) return nullptr;
        return obj_ptr;
    }

    static void construct(
        PyObject* obj_ptr,
        py::converter::rvalue_from_python_stage1_data* data)
    {
        const char* value = ::_PyUnicode_AsString(obj_ptr);
        if (value == 0) py::throw_error_already_set();
        void* storage = (
            (py::converter::rvalue_from_python_storage<FilePath>*)
            data)->storage.bytes;
        new (storage) FilePath(value);
        data->convertible = storage;
    }
};

void InitFilePathWrapper()
{
    py::to_python_converter<
        FilePath,
        FilePathToPyString>();

    py::to_python_converter<
        std::string,
        StringToPyString>();

    FilePathFromPyString();
}

void RemoveEventWrapper(Application& app, const std::string& name)
{
    app.GetEventDispatcher().RemoveEventListener(name);
}

Mesh* LoadMeshWrapper(Scene& scene, const std::string& path)
{
    std::string name = Format(FMT_STRING("pyMesh_{0}"), Application::Get()->GenerateResourceId());
    return scene.LoadMesh(name, path);
}

Script* LoadScriptWrapper(Scene& scene, const std::string& path)
{
    std::string name = Format(FMT_STRING("pyScript_{0}"), Application::Get()->GenerateResourceId());
    return scene.LoadScript(name, path);
}

Shader* LoadShaderWrapper2(Scene& scene, const std::string& vertex, const std::string& fragment)
{
    std::string name = Format(FMT_STRING("pyShader_{0}"), Application::Get()->GenerateResourceId());
    return scene.LoadShader(name, vertex, fragment);
}

Shader* LoadShaderWrapper3(Scene& scene, const std::string& vertex, const std::string& geometry, const std::string& fragment)
{
    std::string name = Format(FMT_STRING("pyShader_{0}"), Application::Get()->GenerateResourceId());
    return scene.LoadShader(name, vertex, geometry, fragment);
}

Texture* LoadTextureWrapper(Scene& scene, const std::string& path)
{
    std::string name = Format(FMT_STRING("pyTexture_{0}"), Application::Get()->GenerateResourceId());
    return scene.LoadTexture(name, path);
}

void SetShaderWrapper(MxObject& object, const std::string& vertex, const std::string& fragment)
{
    object.ObjectShader = LoadShaderWrapper2(Application::Get()->GetCurrentScene(), vertex, fragment);
}

void SetTextureWrapper(MxObject& object, const std::string& texture)
{
    object.ObjectTexture = LoadTextureWrapper(Application::Get()->GetCurrentScene(), texture);
}

template<typename T, typename... Args>
MxObject& AddPrimitiveWrapper(Scene& scene, const std::string& name, Args... args)
{
    return scene.AddObject(name, MakeUnique<T>(args...));
}

Texture* MakeTextureIntWrapper(int r, int g, int b)
{
    return Colors::MakeTexture((uint8_t)Clamp(r, 0, 255), Clamp(g, 0, 255), Clamp(b, 0, 255));
}

void MakeInstancedWrapper(MxObject& object, size_t count)
{
    object.MakeInstanced(count);
}

std::string GetDirectoryWrapper(Scene& scene)
{
    return scene.GetDirectory().string();
}

std::string GetNameWrapper(Scene& scene)
{
    return scene.GetName();
}

void AspectRatioWrapper(ICamera& camera, float aspect)
{
    camera.SetAspectRatio(aspect);
}

Scene& CreatePySceneWrapper(Application& app, const std::string& name, const std::string& directory)
{
    return app.CreateScene(name, MakeUnique<PyScene>(name, directory));
}

void ConsoleBindWrapper(const std::string& handle, KeyCode key)
{
    ConsoleBinding(handle).Bind(key);
}

void AppCloseBindWrapper(const std::string& handle, KeyCode key)
{
    AppCloseBinding(handle).Bind(key);
}

void SetSurfaceWrapper(Surface& surface, py::object func, float xsize, float ysize, float step)
{
    try
    {
        auto wrap = [&func](float x, float y) -> float
        {
            auto result = func(x, y);
            return py::extract<float>(result);
        };
        surface.SetSurface(wrap, xsize, ysize, step);
    }
    catch (std::exception&)
    {
        Logger::Instance().Warning("MxEngine::SetSurface", "error while generating surface in python function");
        surface.SetSurface([](float, float) { return 0.0f; }, xsize, ysize, step);
    }
}

Skybox* GetSkyboxWrapper(Scene& scene)
{
    return scene.SceneSkybox.get();
}

void ShaderVertFragWrapper(const std::string& vertex, const std::string& fragment)
{
    Shader** shader = &Application::Get()->GetRenderer().ObjectShader;
    ShaderBinding("PyShaderBinding", shader).Bind(vertex, fragment);
}

void ShaderVertGeomFragWrapper(const std::string& vertex, const std::string& geometry, const std::string& fragment)
{
    Shader** shader = &Application::Get()->GetRenderer().ObjectShader;
    ShaderBinding("PyShaderBinding", shader).Bind(vertex, geometry, fragment);
}

template<typename Event>
void AddEventListenerWrapper(Application& app, const std::string& name, py::object callback)
{
    app.GetEventDispatcher().AddEventListener<Event>(name, 
        [name, callback = std::move(callback)](Event& e)
        {
            InvokePythonFunction(callback, e);
        });
}

void SetContextPointersWrapper(uint64_t applicationPointer, uint64_t graphicFactoryPointer)
{
    Application::Set(reinterpret_cast<Application*>(applicationPointer));
    Graphics::Instance() = reinterpret_cast<GraphicFactory*>(graphicFactoryPointer);
}

UniqueRef<PyApplication>& StaticAppWrapper()
{
    static auto app = MakeUnique<PyApplication>();
    return app;
}

Application* CreatePyApplication()
{
    if (Application::Get() != nullptr)
    {
        return Application::Get();
    }
    else
    {
        return StaticAppWrapper().get();
    }
}

void DestroyPyApplication()
{
    StaticAppWrapper().reset();
}

#define RefGetter(...) py::make_function(__VA_ARGS__, py::return_internal_reference())
#define StaticVar(...) py::make_function(__VA_ARGS__, py::return_value_policy<py::reference_existing_object>())
#define NewObject(...) py::make_function(__VA_ARGS__, py::return_value_policy<py::manage_new_object>())

BOOST_PYTHON_MODULE(mx_engine)
{
    InitFilePathWrapper();

    py::def("MxEngineSetContextPointers", SetContextPointersWrapper);
    py::def("get_context", StaticVar(Application::Get));
    py::def("create_application", StaticVar(CreatePyApplication));
    py::def("destroy_application", DestroyPyApplication);
    py::def("degrees", DegreesVec<Vector3>);
    py::def("radians", RadiansVec<Vector3>);

    using ExecuteScriptStrFunc = void (Application::*)(const std::string&);
    using ExecuteScriptFileFunc = void (Application::*)(Script&);
    py::class_<Application, boost::noncopyable>("application", py::no_init)
        .def("create_context", &Application::CreateContext)
        .def("create_scene", RefGetter(CreatePySceneWrapper))
        .def("execute", (ExecuteScriptFileFunc)&Application::ExecuteScript)
        .def("execute", (ExecuteScriptStrFunc)&Application::ExecuteScript)
        .def("load_scene", &Application::LoadScene)
        .def("get_scene", RefGetter(&Application::GetScene))
        .def("scene_exists", &Application::SceneExists)
        .def("delete_scene", &Application::DestroyScene)
        .def("run", &Application::Run)
        .def("remove_event", RemoveEventWrapper)
        .def("exit", &Application::CloseApplication)
        .def("dt", &Application::GetTimeDelta)
        .def("listen_update", AddEventListenerWrapper<UpdateEvent>)
        .def("listen_key", AddEventListenerWrapper<KeyEvent>)
        .def("listen_close", AddEventListenerWrapper<AppDestroyEvent>)
        .def("listen_mouse", AddEventListenerWrapper<MouseMoveEvent>)
        .def("listen_fps", AddEventListenerWrapper<FpsUpdateEvent>)
        .def("listen_render", AddEventListenerWrapper<RenderEvent>)
        .def("listen_resize", AddEventListenerWrapper<WindowResizeEvent>)
        .def("use_lighting", &Application::ToggleLighting)
        .def("use_debug_meshes", &Application::ToggleDebugDraw)
        .add_property("set_msaa", &Application::SetMSAASampling)
        .add_property("global", RefGetter(&Application::GetGlobalScene))
        .add_property("is_running", &Application::IsRunning)
        .add_property("scene", RefGetter(&Application::GetCurrentScene))
        .add_property("renderer", RefGetter(&Application::GetRenderer))
        .add_property("log", RefGetter(&Application::GetLogger))
        ;

    py::class_<PyApplication, boost::noncopyable, py::bases<Application>>("pyapplication", py::no_init)
        .def_readwrite("on_create", &PyApplication::createCallback)
        .def_readwrite("on_update", &PyApplication::updateCallback)
        .def_readwrite("on_destroy", &PyApplication::destroyCallback)
        ;

    py::class_<Script, boost::noncopyable>("script", py::no_init)
        .add_property("text", RefGetter(&Script::GetContent))
        .def("update", &Script::UpdateContents)
        ;

    py::class_<UpdateEvent>("update_event", py::no_init)
        .def_readonly("dt", &UpdateEvent::TimeDelta)
        ;

    py::class_<RenderEvent>("render_event", py::no_init)
        ;

    py::class_<MouseMoveEvent>("mouse_move_event", py::no_init)
        .def_readonly("pos", &MouseMoveEvent::position)
        ;

    py::class_<KeyEvent>("key_event", py::no_init)
        .def("is_held", &KeyEvent::IsHeld)
        .def("is_pressed", &KeyEvent::IsPressed)
        .def("is_released", &KeyEvent::IsReleased)
        ;

    py::class_<WindowResizeEvent>("resize_event", py::no_init)
        .def_readonly("old", &WindowResizeEvent::Old)
        .def_readonly("new", &WindowResizeEvent::New)
        ;

    py::class_<FpsUpdateEvent>("fps_update_event", py::no_init)
        .def_readonly("fps", &FpsUpdateEvent::FPS)
        ;

    py::class_<AppDestroyEvent>("app_destroy_event", py::no_init)
        ;

    py::class_<Scene, boost::noncopyable>("scene", py::no_init)
        .def("clear", &Scene::Clear)
        .def("load_mesh", RefGetter(LoadMeshWrapper))
        .def("load_script", RefGetter(LoadScriptWrapper))
        .def("load_shader", RefGetter(LoadShaderWrapper2))
        .def("load_shader", RefGetter(LoadShaderWrapper3))
        .def("load_texture", RefGetter(LoadTextureWrapper))
        .def("load_object", RefGetter(&Scene::CreateObject))
        .def("copy_object", RefGetter(&Scene::CopyObject))
        .def("get_object", RefGetter(&Scene::GetObject))
        .def("has_object", &Scene::HasObject)
        .def("get_mesh", RefGetter(&Scene::GetResource<Mesh>))
        .def("get_script", RefGetter(&Scene::GetResource<Script>))
        .def("get_shader", RefGetter(&Scene::GetResource<Shader>))
        .def("get_texture", RefGetter(&Scene::GetResource<Texture>))
        .def("delete_object", RefGetter(&Scene::DestroyObject))
        .def("add_cube", RefGetter(AddPrimitiveWrapper<Cube>))
        .def("add_sphere", RefGetter(AddPrimitiveWrapper<Sphere, size_t>))
        .def("add_grid", RefGetter(AddPrimitiveWrapper<Grid, size_t>))
        .def_readonly("global_light", &Scene::GlobalLight)
        .def_readonly("point_lights", &Scene::PointLights)
        .def_readonly("spot_lights", &Scene::SpotLights)
        .def_readonly("skybox", RefGetter(GetSkyboxWrapper))
        .def_readonly("viewport", &Scene::Viewport)
        .add_property("directory", GetDirectoryWrapper, &Scene::SetDirectory)
        .add_property("name", GetNameWrapper)
        ;

    py::class_<PyScene, boost::noncopyable, py::bases<Scene>>("pyscene", py::no_init)
        .def_readwrite("on_create", &PyScene::OnCreate)
        .def_readwrite("on_load", &PyScene::OnLoad)
        .def_readwrite("on_unload", &PyScene::OnUnload)
        .def_readwrite("on_update", &PyScene::OnUpdate)
        .def_readwrite("on_render", &PyScene::OnRender)
        .def_readwrite("on_destroy", &PyScene::OnDestroy)
        ;

    py::class_<Skybox, boost::noncopyable>("skybox", py::no_init)
        .def("rotate_x", &Skybox::RotateX)
        .def("rotate_y", &Skybox::RotateY)
        .def("rotate_z", &Skybox::RotateZ)
        .def_readwrite("texture", &Skybox::SkyboxTexture)
        .add_property("rotation", RefGetter(&Skybox::GetRotation))
        .add_property("matrix", RefGetter(&Skybox::GetRotationMatrix))
        .add_property("shader", RefGetter(&Skybox::GetShader))
        ;

    py::enum_<TextureFormat>("tex_format")
        .value("RGB", TextureFormat::RGB)
        .value("RGBA", TextureFormat::RGBA)
        .value("RGB16", TextureFormat::RGB16)
        .value("RGBA16", TextureFormat::RGBA16)
        .value("RGB16F", TextureFormat::RGB16F)
        .value("RGBA16F", TextureFormat::RGBA16F)
        .value("RGBA32F", TextureFormat::RGBA32F)
        .value("RGB32F", TextureFormat::RGB32F)
        ;

    py::enum_<TextureWrap>("tex_wrap")
        .value("MIRRORED_REPEAT", TextureWrap::MIRRORED_REPEAT)
        .value("CLAMP_TO_EDGE", TextureWrap::CLAMP_TO_EDGE)
        .value("REPEAT", TextureWrap::REPEAT)
        ;

    py::enum_<KeyCode>("keycode")
        .value("UNKNOWN", KeyCode::UNKNOWN)
        .value("SPACE", KeyCode::SPACE)
        .value("APOSTROPHE", KeyCode::APOSTROPHE)
        .value("COMMA", KeyCode::COMMA)
        .value("MINUS", KeyCode::MINUS)
        .value("PERIOD", KeyCode::PERIOD)
        .value("SLASH", KeyCode::SLASH)
        .value("D0", KeyCode::D0)
        .value("D1", KeyCode::D1)
        .value("D2", KeyCode::D2)
        .value("D3", KeyCode::D3)
        .value("D4", KeyCode::D4)
        .value("D5", KeyCode::D5)
        .value("D6", KeyCode::D6)
        .value("D7", KeyCode::D7)
        .value("D8", KeyCode::D8)
        .value("D9", KeyCode::D9)
        .value("SEMICOLON", KeyCode::SEMICOLON)
        .value("EQUAL", KeyCode::EQUAL)
        .value("A", KeyCode::A)
        .value("B", KeyCode::B)
        .value("C", KeyCode::C)
        .value("D", KeyCode::D)
        .value("E", KeyCode::E)
        .value("F", KeyCode::F)
        .value("G", KeyCode::G)
        .value("H", KeyCode::H)
        .value("I", KeyCode::I)
        .value("J", KeyCode::J)
        .value("K", KeyCode::K)
        .value("L", KeyCode::L)
        .value("M", KeyCode::M)
        .value("N", KeyCode::N)
        .value("O", KeyCode::O)
        .value("P", KeyCode::P)
        .value("Q", KeyCode::Q)
        .value("R", KeyCode::R)
        .value("S", KeyCode::S)
        .value("T", KeyCode::T)
        .value("U", KeyCode::U)
        .value("V", KeyCode::V)
        .value("W", KeyCode::W)
        .value("X", KeyCode::X)
        .value("Y", KeyCode::Y)
        .value("Z", KeyCode::Z)
        .value("LEFT_BRACKET", KeyCode::LEFT_BRACKET)
        .value("BACKSLASH", KeyCode::BACKSLASH)
        .value("RIGHT_BRACKET", KeyCode::RIGHT_BRACKET)
        .value("GRAVE_ACCENT", KeyCode::GRAVE_ACCENT)
        .value("WORLD_1", KeyCode::WORLD_1)
        .value("WORLD_2", KeyCode::WORLD_2)
        .value("ESCAPE", KeyCode::ESCAPE)
        .value("ENTER", KeyCode::ENTER)
        .value("TAB", KeyCode::TAB)
        .value("BACKSPACE", KeyCode::BACKSPACE)
        .value("INSERT", KeyCode::INSERT)
        .value("DELETE", KeyCode::DELETE)
        .value("RIGHT", KeyCode::RIGHT)
        .value("LEFT", KeyCode::LEFT)
        .value("DOWN", KeyCode::DOWN)
        .value("UP", KeyCode::UP)
        .value("PAGE_UP", KeyCode::PAGE_UP)
        .value("PAGE_DOWN", KeyCode::PAGE_DOWN)
        .value("HOME", KeyCode::HOME)
        .value("END", KeyCode::END)
        .value("CAPS_LOCK", KeyCode::CAPS_LOCK)
        .value("SCROLL_LOCK", KeyCode::SCROLL_LOCK)
        .value("NUM_LOCK", KeyCode::NUM_LOCK)
        .value("PRINT_SCREEN", KeyCode::PRINT_SCREEN)
        .value("PAUSE", KeyCode::PAUSE)
        .value("F1", KeyCode::F1)
        .value("F2", KeyCode::F2)
        .value("F3", KeyCode::F3)
        .value("F4", KeyCode::F4)
        .value("F5", KeyCode::F5)
        .value("F6", KeyCode::F6)
        .value("F7", KeyCode::F7)
        .value("F8", KeyCode::F8)
        .value("F9", KeyCode::F9)
        .value("F10", KeyCode::F10)
        .value("F11", KeyCode::F11)
        .value("F12", KeyCode::F12)
        .value("F13", KeyCode::F13)
        .value("F14", KeyCode::F14)
        .value("F15", KeyCode::F15)
        .value("F16", KeyCode::F16)
        .value("F17", KeyCode::F17)
        .value("F18", KeyCode::F18)
        .value("F19", KeyCode::F19)
        .value("F20", KeyCode::F20)
        .value("F21", KeyCode::F21)
        .value("F22", KeyCode::F22)
        .value("F23", KeyCode::F23)
        .value("F24", KeyCode::F24)
        .value("F25", KeyCode::F25)
        .value("KP_0", KeyCode::KP_0)
        .value("KP_1", KeyCode::KP_1)
        .value("KP_2", KeyCode::KP_2)
        .value("KP_3", KeyCode::KP_3)
        .value("KP_4", KeyCode::KP_4)
        .value("KP_5", KeyCode::KP_5)
        .value("KP_6", KeyCode::KP_6)
        .value("KP_7", KeyCode::KP_7)
        .value("KP_8", KeyCode::KP_8)
        .value("KP_9", KeyCode::KP_9)
        .value("KP_DECIMAL", KeyCode::KP_DECIMAL)
        .value("KP_DIVIDE", KeyCode::KP_DIVIDE)
        .value("KP_MULTIPLY", KeyCode::KP_MULTIPLY)
        .value("KP_SUBTRACT", KeyCode::KP_SUBTRACT)
        .value("KP_ADD", KeyCode::KP_ADD)
        .value("KP_ENTER", KeyCode::KP_ENTER)
        .value("KP_EQUAL", KeyCode::KP_EQUAL)
        .value("LEFT_SHIFT", KeyCode::LEFT_SHIFT)
        .value("LEFT_CONTROL", KeyCode::LEFT_CONTROL)
        .value("LEFT_ALT", KeyCode::LEFT_ALT)
        .value("LEFT_SUPER", KeyCode::LEFT_SUPER)
        .value("RIGHT_SHIFT", KeyCode::RIGHT_SHIFT)
        .value("RIGHT_CONTROL", KeyCode::RIGHT_CONTROL)
        .value("RIGHT_ALT", KeyCode::RIGHT_ALT)
        .value("RIGHT_SUPER", KeyCode::RIGHT_SUPER)
        .value("MENU", KeyCode::MENU)
        ;

    py::class_<LoggerImpl, boost::noncopyable>("logger", py::no_init)
        .def("error", &LoggerImpl::Error)
        .def("warning", &LoggerImpl::Warning)
        .def("debug", &LoggerImpl::Debug)
        .def("stacktrace", &LoggerImpl::StackTrace)
        .def("use_error", RefGetter(&LoggerImpl::UseError))
        .def("use_warning", RefGetter(&LoggerImpl::UseWarning))
        .def("use_debug", RefGetter(&LoggerImpl::UseDebug))
        .def("use_stacktrace", RefGetter(&LoggerImpl::UseStackTrace))
        ;

    using ScaleF3 = Matrix4x4(*)(const Matrix4x4&, const Vector3&);
    using ScaleF1 = Matrix4x4(*)(const Matrix4x4&, float);

    py::def("view_matrix", MakeViewMatrix);
    py::def("perspective_matrix", MakePerspectiveMatrix);
    py::def("orthographic_matrix", MakeOrthographicMatrix);
    py::def("normalize", Normalize<Vector2>);
    py::def("normalize", Normalize<Vector3>);
    py::def("normalize", Normalize<Vector4>);
    py::def("length", Length<Vector2>);
    py::def("length", Length<Vector3>);
    py::def("length", Length<Vector4>);
    py::def("length2", Length2<Vector2>);
    py::def("length2", Length2<Vector3>);
    py::def("length2", Length2<Vector4>);
    py::def("translate", Translate);
    py::def("scale", (ScaleF1)Scale);
    py::def("scale", (ScaleF3)Scale);
    py::def("rotate", Rotate);
    py::def("mat4", ToMatrix);
    py::def("qua", MakeQuaternion);
    py::def("euler", MakeEulerAngles);
    py::def("transpose", Transpose<4, 4, float>);
    py::def("inverse", Inverse<4, 4, float>);
    py::def("clamp", Clamp<int>);
    py::def("clamp", Clamp<float>);
    py::def("clamp", Clamp<Vector2>);
    py::def("clamp", Clamp<Vector3>);
    py::def("clamp", Clamp<Vector4>);
    py::def("radians", Radians<float>);
    py::def("degrees", Degrees<float>);

    py::class_<Vector4>("vec4", py::init<float>())
        .def(py::init<float, float, float, float>())
        .def("__str__", VectorToString<Vector4>)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self * float())
        .def(float() * py::self)
        .def(py::self / float())
        .def(-py::self)
        .def(+py::self)
        .def("__getitem__", VectorIndexGetWrapper<Vector4>)
        .def("__setitem__", VectorIndexSetWrapper<Vector4>)
        .def_readwrite("x", &Vector4::x)
        .def_readwrite("y", &Vector4::y)
        .def_readwrite("z", &Vector4::z)
        .def_readwrite("w", &Vector4::w)
        ;

	py::class_<Vector3>("vec3", py::init<float>())
        .def(py::init<float, float, float>())
        .def("__str__", VectorToString<Vector3>)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self * float())
        .def(float() * py::self)
        .def(py::self / float())
        .def(-py::self)
        .def(+py::self)
        .def("__getitem__", VectorIndexGetWrapper<Vector3>)
        .def("__setitem__", VectorIndexSetWrapper<Vector3>)
        .def_readwrite("x", &Vector3::x)
        .def_readwrite("y", &Vector3::y)
        .def_readwrite("z", &Vector3::z)
        ;

	py::class_<Vector2>("vec2", py::init<float>())
        .def(py::init<float, float>())
        .def("__str__", VectorToString<Vector2>)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self * float())
        .def(float() * py::self)
        .def(py::self / float())
        .def(-py::self)
        .def(+py::self)
        .def("__getitem__", VectorIndexGetWrapper<Vector2>)
        .def("__setitem__", VectorIndexSetWrapper<Vector2>)
        .def_readwrite("x", &Vector2::x)
        .def_readwrite("y", &Vector2::y)
        ;

    py::class_<Matrix4x4>("mat4", py::init<float>())
        .def("__str__", MatrixToString<Matrix4x4>)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self * float())
        .def(float() * py::self)
        .def(py::self / float())
        .def(-py::self)
        .def(+py::self)
        .def("__getitem__", RefGetter(MatrixIndexGetWrapper<Matrix4x4>))
        .def("__setitem__", MatrixIndexSetWrapper<Matrix4x4>)
        ;

    py::class_<Matrix3x3>("mat3", py::init<float>())
        .def("__str__", MatrixToString<Matrix3x3>)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self* float())
        .def(float()* py::self)
        .def(py::self / float())
        .def(-py::self)
        .def(+py::self)
        .def("__getitem__", RefGetter(MatrixIndexGetWrapper<Matrix3x3>))
        .def("__setitem__", MatrixIndexSetWrapper<Matrix3x3>)
        ;

    using ResizeFunc = void(std::vector<float>::*)(size_t);
    using PushBackFunc = void(std::vector<float>::*)(const float&);

    py::class_<std::vector<float>>("vec_float", py::init())
        .def("resize", (ResizeFunc)&std::vector<float>::resize)
        .def("__getitem__", StdVectorGetWrapper<float>)
        .def("__setitem__", RefGetter(StdVectorSetWrapper<float>))
        .def("clear", &std::vector<float>::clear)
        .def("push", (PushBackFunc)&std::vector<float>::push_back)
        .def("pop", &std::vector<float>::pop_back)
        .def("__len__", &std::vector<float>::size)
        .def("size", &std::vector<float>::size)
        ;

    py::class_<IBindableWrapper, boost::noncopyable>("bindable", py::no_init)
        .def("bind", py::pure_virtual(&IBindable::Bind))
        .def("unbind", py::pure_virtual(&IBindable::Unbind))
        .add_property("handle", &IBindable::GetNativeHandler)
        ;

    using LoadVsFs = void(Shader::*)(const std::string&, const std::string&);
    using LoadVsGsFs = void(Shader::*)(const std::string&, const std::string&, const std::string&);

    py::class_<ShaderWrapper, py::bases<IBindable>, boost::noncopyable>("shader", py::init())
        .def("load", py::pure_virtual((LoadVsFs)&Shader::Load))
        .def("load", py::pure_virtual((LoadVsGsFs)&Shader::Load))
        .def("load_source", py::pure_virtual((LoadVsFs)&Shader::LoadFromString))
        .def("load_source", py::pure_virtual((LoadVsGsFs)&Shader::LoadFromString))
        .def("set_int", py::pure_virtual(&Shader::SetUniformInt))
        .def("set_float", py::pure_virtual(&Shader::SetUniformFloat))
        .def("set_vec2", py::pure_virtual(&Shader::SetUniformVec2))
        .def("set_vec3", py::pure_virtual(&Shader::SetUniformVec3))
        .def("set_vec4", py::pure_virtual(&Shader::SetUniformVec4))
        .def("set_mat3", py::pure_virtual(&Shader::SetUniformMat3))
        .def("set_mat4", py::pure_virtual(&Shader::SetUniformMat4))
        ;

    using LoadTextureFile = void(Texture::*)(const std::string&, TextureWrap, bool, bool);
    using LoadTextureRaw = void(Texture::*)(Texture::RawDataPointer, int, int, TextureFormat, TextureWrap, bool);
    using BindTextureId = void(Texture::*)(Texture::IdType);

    py::class_<TextureWrapper, py::bases<IBindable>, boost::noncopyable>("texture", py::init())
        .def("load", py::pure_virtual((LoadTextureFile)&Texture::Load))
        .def("load_raw", py::pure_virtual((LoadTextureRaw)&Texture::Load))
        .def("load_depth", py::pure_virtual(&Texture::LoadDepth))
        .def("load_multisample", py::pure_virtual(&Texture::LoadMultisample))
        .def("bind", py::pure_virtual((BindTextureId)&Texture::Bind))
        .add_property("width", &Texture::GetWidth)
        .add_property("height", &Texture::GetHeight)
        .add_property("path", RefGetter(&Texture::GetPath))
        .add_property("channels", &Texture::GetChannelCount)
        .add_property("type", &Texture::GetTextureType)
        ;

    py::class_<VertexBufferWrapper, py::bases<IBindable>, boost::noncopyable>("vertex_buffer", py::init())
        .def("load", py::pure_virtual(&VertexBuffer::Load))
        .def("buffer", py::pure_virtual(&VertexBuffer::BufferSubData))
        .add_property("size", &VertexBuffer::GetSize)
        ;

    py::class_<VertexBufferLayoutWrapper, boost::noncopyable>("vertex_buffer_layout", py::init())
        .def("push", py::pure_virtual(&VertexBufferLayout::PushFloat))
        .add_property("stride", &VertexBufferLayout::GetStride)
        .add_property("elements", RefGetter(&VertexBufferLayout::GetElements))
        ;

    py::class_<VertexArrayWrapper, py::bases<IBindable>, boost::noncopyable>("vertex_array", py::init())
        .def("add_buffer", py::pure_virtual(&VertexArray::AddBuffer))
        .def("add_instanced_buffer", py::pure_virtual(&VertexArray::AddInstancedBuffer))
        .add_property("attribute_count", &VertexArray::GetAttributeCount)
        ;

    py::class_<IndexBufferWrapper, py::bases<IBindable>, boost::noncopyable>("index_buffer", py::init())
        .def("load", py::pure_virtual(&IndexBuffer::Load))
        .add_property("count", &IndexBuffer::GetCount)
        .add_property("type_id", &IndexBuffer::GetIndexTypeId)
        ;

    py::enum_<Colors::Palette>("colors")
        .value("red", Colors::RED)
        .value("green", Colors::GREEN)
        .value("blue", Colors::BLUE)
        .value("yellow", Colors::YELLOW)
        .value("aqua", Colors::AQUA)
        .value("purple", Colors::PURPLE)
        .value("black", Colors::BLACK)
        .value("white", Colors::WHITE)
        .value("orange", Colors::ORANGE)
        .value("lime", Colors::LIME)
        .value("magenta", Colors::MAGENTA)
        .value("violet", Colors::VIOLET)
        .value("skyblue", Colors::SKYBLUE)
        .value("spring", Colors::SPRING)
        .value("grey", Colors::GREY)
        ;

    using TextureFloat = Texture* (*)(float, float, float);
    using TextureColor = Texture* (*)(Colors::Palette);
    using TextureVec3 = Texture* (*)(const Vector3&);

    py::def("make_texture", RefGetter(MakeTextureIntWrapper));
    py::def("make_texture", RefGetter((TextureFloat)Colors::MakeTexture));
    py::def("make_texture", RefGetter((TextureColor)Colors::MakeTexture));
    py::def("make_texture", RefGetter((TextureVec3)Colors::MakeTexture));

    using TranslateFunc3F = CameraController & (CameraController::*)(float, float, float);
    using TranslateFunc3V = CameraController & (CameraController::*)(const Vector3&);

    py::class_<RenderController, boost::noncopyable>("render_controller", py::no_init)
        .def("set_viewport", &RenderController::SetViewport)
        .def("use_reversed_depth", &RenderController::ToggleReversedDepth)
        .def("set_anisotropic", &RenderController::SetAnisotropicFiltering)
        .def_readwrite("object_shader", &RenderController::ObjectShader)
        .def_readwrite("depth_texture_shader", &RenderController::DepthTextureShader)
        .def_readwrite("depth_cubemap_shader", &RenderController::DepthCubeMapShader)
        .def_readwrite("default_texture", &RenderController::DefaultTexture)
        .add_property("bloom_iters", &RenderController::GetBloomIterations, &RenderController::SetBloomIterations)
        .add_property("pcf", &RenderController::GetPCFDIstance, &RenderController::SetPCFDistance)
        .add_property("exposure", &RenderController::GetHDRExposure, &RenderController::SetHDRExposure)
        .add_property("bloom", &RenderController::GetBloomWeight, &RenderController::SetBloomWeight)
        .add_property("dir_depth_size",
            &RenderController::GetDepthBufferSize<DirectionalLight>, 
            &RenderController::SetDepthBufferSize<DirectionalLight>)
        .add_property("spot_depth_size",
            &RenderController::GetDepthBufferSize<SpotLight>,
            &RenderController::SetDepthBufferSize<SpotLight>)
        ;

    py::class_<DirectionalLight, boost::noncopyable>("dir_light", py::init())
        .def_readwrite("ambient", &DirectionalLight::AmbientColor)
        .def_readwrite("diffuse", &DirectionalLight::DiffuseColor)
        .def_readwrite("specular", &DirectionalLight::SpecularColor)
        .def_readwrite("direction", &DirectionalLight::Direction)
        .def_readwrite("projection_size", &DirectionalLight::ProjectionSize)
        ;

    py::class_<PointLight, boost::noncopyable>("point_light", py::init())
        .def_readwrite("ambient", &PointLight::AmbientColor)
        .def_readwrite("diffuse", &PointLight::DiffuseColor)
        .def_readwrite("specular", &PointLight::SpecularColor)
        .add_property("factors", RefGetter(&PointLight::GetFactors), RefGetter(&PointLight::UseFactors))
        .def_readwrite("position", &PointLight::Position)
        ;

    py::class_<SpotLight, boost::noncopyable>("spot_light", py::init())
        .add_property("outer_angle", &SpotLight::GetOuterAngle, RefGetter(&SpotLight::UseOuterAngle))
        .add_property("inner_angle", &SpotLight::GetInnerAngle, RefGetter(&SpotLight::UseInnerAngle))
        .def_readwrite("ambient", &SpotLight::AmbientColor)
        .def_readwrite("diffuse", &SpotLight::DiffuseColor)
        .def_readwrite("specular", &SpotLight::SpecularColor)
        .def_readwrite("direction", &SpotLight::Direction)
        .def_readwrite("position", &SpotLight::Position)
        ;

    py::class_<Scene::LightContainer<PointLight>, boost::noncopyable>("point_light_list", py::no_init)
        .def("__getitem__", RefGetter(LightContainerIndexGetWrapper<PointLight>))
        .def("__len__", &Scene::LightContainer<PointLight>::GetCount)
        .def("resize", &Scene::LightContainer<PointLight>::SetCount)
        ;

    py::class_<Scene::LightContainer<SpotLight>, boost::noncopyable>("spot_light_list", py::no_init)
        .def("__getitem__", RefGetter(LightContainerIndexGetWrapper<SpotLight>))
        .def("__len__", &Scene::LightContainer<SpotLight>::GetCount)
        .def("resize", &Scene::LightContainer<SpotLight>::SetCount)
        ;

    py::class_<IMovableWrapper, boost::noncopyable>("movable", py::no_init)
        .def("move", RefGetter(&IMovable::Translate))
        .def("move_forward", RefGetter(&IMovable::TranslateForward))
        .def("move_right", RefGetter(&IMovable::TranslateRight))
        .def("move_up", RefGetter(&IMovable::TranslateUp))
        .def("rotate", RefGetter(&IMovable::Rotate))
        .add_property("vec_forward", RefGetter(&IMovable::GetForwardVector))
        .add_property("vec_right", RefGetter(&IMovable::GetRightVector))
        .add_property("vec_up", RefGetter(&IMovable::GetUpVector))
        ;

    py::class_<AABB>("AABB", py::init())
        .def_readwrite("min", &AABB::Min)
        .def_readwrite("max", &AABB::Max)
        .add_property("center", &AABB::GetCenter)
        .add_property("length", &AABB::Length)
        ;

    using SubMeshesGetFunc = std::vector<SubMesh>& (Mesh::*)();
    py::class_<Mesh, boost::noncopyable>("mesh", py::no_init)
        .add_property("AABB", RefGetter(&Mesh::GetAABB))
        .add_property("submeshes", RefGetter((SubMeshesGetFunc)&Mesh::GetRenderObjects))
        .add_property("lod", &Mesh::GetLOD, &Mesh::SetLOD)
        ;

    using SubMeshesGetMeshFunc = SubMesh& (std::vector<SubMesh>::*)(size_t);
    py::class_<std::vector<SubMesh>, boost::noncopyable>("submesh_list", py::no_init)
        .def("__getitem__", RefGetter(StdVectorGetRefWrapper<SubMesh>))
        .def("__len__", &std::vector<SubMesh>::size)
        ;

    using GetMaterialFunc = Material& (SubMesh::*)();
    using GetTransformFunc = Transform& (SubMesh::*)();
    py::class_<SubMesh, boost::noncopyable>("submesh", py::no_init)
        .add_property("material", RefGetter((GetMaterialFunc)&SubMesh::GetMaterial))
        .add_property("transform", RefGetter((GetTransformFunc)&SubMesh::GetTransform))
        .add_property("color", RefGetter(&SubMesh::GetRenderColor), &SubMesh::SetRenderColor)
        .add_property("name", RefGetter(&SubMesh::GetName))
        .add_property("has_texture", &SubMesh::UsesTexture)
        .add_property("has_normals", &SubMesh::UsesNormals)
        ;

    py::class_<Material, boost::noncopyable>("material", py::no_init)
        .def_readwrite("Ns", &Material::Ns)
        .def_readwrite("Ni", &Material::Ni)
        .def_readwrite("d", &Material::d)
        .def_readwrite("Tr", &Material::Tr)
        .def_readwrite("Tf", &Material::Tf)
        .def_readwrite("Ka", &Material::Ka)
        .def_readwrite("Kd", &Material::Kd)
        .def_readwrite("Ks", &Material::Ks)
        .def_readwrite("Ke", &Material::Ke)
        .def_readwrite("illum", &Material::illum)
        .def_readwrite("f_Ka", &Material::f_Ka)
        .def_readwrite("f_Kd", &Material::f_Kd)
        .def_readwrite("refl", &Material::reflection)
        .def_readwrite("disp", &Material::displacement)
        ;

    using CameraFunc = ICamera & (CameraController::*)();

    py::class_<CameraController, py::bases<IMovable>, boost::noncopyable>("camera_controller", py::init())
        .def("has_camera", &CameraController::HasCamera)
        .add_property("camera", RefGetter((CameraFunc)&CameraController::GetCamera))
        .add_property("camera_matrix", RefGetter(&CameraController::GetMatrix))
        .add_property("move_speed", &CameraController::GetMoveSpeed, &CameraController::SetMoveSpeed)
        .add_property("rotate_speed", &CameraController::GetRotateSpeed, &CameraController::SetRotateSpeed)
        .add_property("position", RefGetter(&CameraController::GetPosition), &CameraController::SetPosition)
        .add_property("direction", RefGetter(&CameraController::GetDirection), &CameraController::SetDirection)
        .add_property("up", RefGetter(&CameraController::GetUp), &CameraController::SetUp)
        .add_property("zoom", &CameraController::GetZoom, &CameraController::SetZoom)
        .add_property("horizontal_angle", &CameraController::GetHorizontalAngle)
        .add_property("vertical_angle", &CameraController::GetVerticalAngle)
        .def("move", RefGetter((TranslateFunc3V)&CameraController::Translate))
        .def("move_x", RefGetter(&CameraController::TranslateX))
        .def("move_y", RefGetter(&CameraController::TranslateY))
        .def("move_z", RefGetter(&CameraController::TranslateZ)) 
        .def("orthographic", SetOrthographicCamera)
        .def("perspective", SetPerspectiveCamera)
        ;

    py::class_<ICameraWrapper, boost::noncopyable>("camera", py::no_init)
        .def("set_view", py::pure_virtual(&ICamera::SetViewMatrix))
        .add_property("matrix", RefGetter(&ICamera::GetMatrix))
        .add_property("view_matrix", RefGetter(&ICamera::GetViewMatrix))
        .add_property("projection_matrix", RefGetter(&ICamera::GetProjectionMatrix))
        .add_property("aspect", &ICamera::GetAspectRatio, AspectRatioWrapper)
        .add_property("znear",&ICamera::GetZNear, &ICamera::SetZNear)
        .add_property("zfar", &ICamera::GetZFar, &ICamera::SetZFar)
        .add_property("zoom", &ICamera::GetZoom, &ICamera::SetZoom)
        ;

    py::class_<PerspectiveCamera, py::bases<ICamera>>("perspective_camera", py::init())
        .add_property("fov", &PerspectiveCamera::GetFOV, &PerspectiveCamera::SetFOV)
        ;

    py::class_<OrthographicCamera, py::bases<ICamera>>("orthographic_camera", py::init())
        .def("set_projection", &OrthographicCamera::SetProjection)
        ;

    using RotateQua = Transform& (Transform::*)(const Quaternion&);
    using Rotate4F = Transform & (Transform::*)(float, const Vector3&);
    using Scale3F = Transform & (Transform::*)(const Vector3&);
    using Scale1F = Transform & (Transform::*)(float);
    using Matrix4Get = const Matrix4x4 & (Transform::*)() const;
    using Matrix3Get = const Matrix3x3 & (Transform::*)() const;

    py::class_<Transform>("transform", py::init())
        .add_property("position", RefGetter(&Transform::GetPosition), RefGetter(&Transform::SetPosition))
        .add_property("rotation", RefGetter(&Transform::GetRotation), RefGetter((RotateQua)&Transform::SetRotation))
        .add_property("scale", RefGetter(&Transform::GetScale), RefGetter((Scale3F)&Transform::SetScale))
        .add_property("euler", RefGetter(&Transform::GetEulerRotation))
        .add_property("matrix", RefGetter((Matrix4Get)&Transform::GetMatrix))
        .add_property("normal_matrix", RefGetter((Matrix3Get)&Transform::GetNormalMatrix))
        .def("scale_xyz", RefGetter((Scale1F)&Transform::Scale))
        .def("scale_x", RefGetter(&Transform::ScaleX))
        .def("scale_y", RefGetter(&Transform::ScaleY))
        .def("scale_z", RefGetter(&Transform::ScaleZ))
        .def("rotate_x", RefGetter(&Transform::RotateX))
        .def("rotate_y", RefGetter(&Transform::RotateY))
        .def("rotate_z", RefGetter(&Transform::RotateZ))
        .def("move", RefGetter(&Transform::Translate))
        .def("move_x", RefGetter(&Transform::TranslateX))
        .def("move_y", RefGetter(&Transform::TranslateY))
        .def("move_z", RefGetter(&Transform::TranslateZ))
        .def("rotate_euler", RefGetter((Rotate4F)&Transform::SetRotation));
        ;

    using ScaleFunc1F = MxObject & (MxObject::*)(float);
    using ScaleFuncVec = MxObject & (MxObject::*)(const Vector3&);
    using TranslateFunc3 = MxObject & (MxObject::*)(float, float, float);
    using RotateMoveFunc = MxObject & (MxObject::*)(float);
    using InstanceListFunc = Instancing<MxObject>::InstanceList & (MxObject::*)();
    using MeshFunc = Mesh* (MxObject::*)();

    py::class_<MxObject, py::bases<IMovable>, boost::noncopyable>("mx_object")
        .def("instanciate", &MxObject::Instanciate)
        .def("scale", RefGetter(&MxObject::Scale))
        .def("hide", &MxObject::Hide)
        .def("show", &MxObject::Show)
        .def("set_texture", SetTextureWrapper)
        .def("set_shader", SetShaderWrapper)
        .def("instanciate", &MxObject::Instanciate)
        .def("make_instanced", MakeInstancedWrapper)
        .def("set_autobuffering", &MxObject::SetAutoBuffering)
        .def("buffer_instances", &MxObject::BufferInstances)
        .def_readwrite("use_lod", &MxObject::UseLOD)
        .def_readwrite("transform", &MxObject::ObjectTransform)
        .def_readwrite("texture", &MxObject::ObjectTexture)
        .def_readwrite("shader", &MxObject::ObjectShader)
        .def_readwrite("move_speed", &MxObject::TranslateSpeed)
        .def_readwrite("rotate_speed", &MxObject::RotateSpeed)
        .def_readwrite("scale_speed", &MxObject::ScaleSpeed)
        .add_property("buffer_count", &MxObject::GetBufferCount)
        .add_property("AABB", RefGetter(&MxObject::GetAABB))
        .add_property("mesh", RefGetter((MeshFunc)&MxObject::GetMesh), &MxObject::SetMesh)
        .add_property("instances", RefGetter((InstanceListFunc)&MxObject::GetInstances))
        .add_property("render_color", RefGetter(&MxObject::GetRenderColor), &MxObject::SetRenderColor)
        ;

    py::class_<Surface, py::bases<MxObject>, boost::noncopyable>("surface", py::no_init)
        .def("set", SetSurfaceWrapper)
        ;

    py::class_<MxInstance, boost::noncopyable>("mx_instance", py::no_init)
        .def_readwrite("transform", &MxInstance::Model)
        .add_property("color", RefGetter(&MxInstance::GetColor), &MxInstance::SetColor)
        .def("hide", &MxInstance::Hide)
        .def("show", &MxInstance::Show)
        ;

    py::class_<std::vector<MxInstance>, boost::noncopyable>("instance_list", py::no_init)
        .def("__getitem__", RefGetter(StdVectorGetRefWrapper<MxInstance>))
        .def("__setitem__", StdVectorSetWrapper<MxInstance>)
        .def("__len__", &std::vector<MxInstance>::size)
        .def("size", &std::vector<MxInstance>::size)
        ;

    py::def("bind_console", ConsoleBindWrapper);
    py::def("bind_close", AppCloseBindWrapper);
    py::def("bind_shader", ShaderVertFragWrapper);
    py::def("bind_shader", ShaderVertGeomFragWrapper);

    using Move4Func = InputControlBinding & (InputControlBinding::*)(KeyCode, KeyCode, KeyCode, KeyCode);
    using Move6Func = InputControlBinding & (InputControlBinding::*)(KeyCode, KeyCode, KeyCode, KeyCode, KeyCode, KeyCode);

    py::class_<InputControlBinding>("control_binder", py::init<std::string, IMovable&>())
        .def("bind_move", RefGetter((Move4Func)&InputControlBinding::BindMovement))
        .def("bind_move", RefGetter((Move6Func)&InputControlBinding::BindMovement))
        .def("bind_rotate", RefGetter(&InputControlBinding::BindRotation))
        .def("bind_rotate_vert", RefGetter(&InputControlBinding::BindVerticalRotation))
        .def("bind_rotate_horz", RefGetter(&InputControlBinding::BindHorizontalRotation))
        ;

    py::class_<LightBinding<SpotLight>>("spot_lights_binder", py::init<Scene::LightContainer<SpotLight>&>())
        .def("bind", &LightBinding<SpotLight>::BindAll)
        .def("unbind", &LightBinding<SpotLight>::UnbindAll)
        ;

    py::class_<LightBinding<PointLight>>("point_lights_binder", py::init<Scene::LightContainer<PointLight>&>())
        .def("bind", &LightBinding<PointLight>::BindAll)
        .def("unbind", &LightBinding<PointLight>::UnbindAll)
        ;
}