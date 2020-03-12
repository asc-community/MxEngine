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

#define BOOST_PYTHON_STATIC_LIB
#include <boost/python.hpp>
#include <sstream>

namespace py = boost::python;

using namespace MxEngine;

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
auto VectorIndexGetWrapper(T& v, int index)
{
    if (index >= 0 && index < v.length()) {
        return v[index];
    }
    else {
        ::PyErr_SetString(::PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
        return 0.0f;
    }
}

void Matrix4IndexSetWrapper(Matrix4x4& m, int index, const Vector4& value)
{
    if (index >= 0 && index < m.length()) {
        m[index] = value;
    }
    else {
        ::PyErr_SetString(::PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
    }
}

auto Matrix4IndexGetWrapper(Matrix4x4& m, int index)
{
    if (index >= 0 && index < m.length()) {
        return m[index];
    }
    else {
        ::PyErr_SetString(::PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
        return Vector4(0.0f);
    }
}

std::string Vector4ToString(const Vector4& v)
{
    std::stringstream out;
    out << '[' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ']';
    return out.str();
}

std::string Vector3ToString(const Vector3& v)
{
    std::stringstream out;
    out << '[' << v.x << ", " << v.y << ", " << v.z << ']';
    return out.str();
}

std::string Vector2ToString(const Vector2& v)
{
    std::stringstream out;
    out << '[' << v.x << ", " << v.y << ']';
    return out.str();
}

std::string Matrix4ToString(const Matrix4x4& m)
{
    std::stringstream out;
    out << '[';
    for (int i = 0; i < m.length(); i++)
    {
        if (i != 0) out << ", ";
        out << Vector4ToString(m[i]);
    }
    out << ']';
    return out.str();
}

void SetPerspectiveCamera(CameraController& controller)
{
    controller.SetCamera(MakeUnique<PerspectiveCamera>());
}

void SetOrthographicCamera(CameraController& controller)
{
    controller.SetCamera(MakeUnique<OrthographicCamera>());
}

class ICameraWrapper : public ICamera, public py::wrapper<ICamera>
{
public:
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

CameraController& ViewportWrapper(Application& app) { return app.GetRenderer().ViewPort; }

void RemoveEventWrapper(Application& app, const std::string& name)
{
    app.GetEventDispatcher().RemoveEventListener(name);
}

MxObject& LoadObjectWrapper(Application& app, const std::string& name, const std::string& path)
{
    return app.CreateObject(name, path);
}

void SetShaderWrapper(MxObject& object, const std::string& vertex, const std::string& fragment)
{
    object.ObjectShader = Application::Get()->CreateShader(vertex, fragment);
}

void SetTextureWrapper(MxObject& object, const std::string& texture)
{
    object.ObjectTexture = Application::Get()->CreateTexture(texture);
}

void AddEventListenerWrapper(Application& app, const std::string& name, py::object callback)
{
    app.GetEventDispatcher().AddEventListener<UpdateEvent>(name, 
        [name, callback = std::move(callback)](UpdateEvent&)
        {
            try
            {
                callback();
            }
            catch (py::error_already_set&)
            {
                ::PyErr_Print();
                auto dict = py::import("__main__").attr("__dict__");
                py::object msg = dict["errorHandler"].attr("value");
                std::string error = py::extract<std::string>(msg);
                if (!error.empty()) error.pop_back(); // delete last '\n'
                ::PyErr_Clear();

                Application::Get()->GetConsole().Log("[error]: " + error);
                Application::Get()->GetEventDispatcher().RemoveEventListener(name);
            }
        });
}

void SetContextPointersWrapper(uint64_t applicationPointer, uint64_t graphicFactoryPointer)
{
    Application::Set(reinterpret_cast<Application*>(applicationPointer));
    Graphics::Instance() = reinterpret_cast<GraphicFactory*>(graphicFactoryPointer);
}

#define RefGetter(method) py::make_function(method, py::return_internal_reference())
#define StaticVar(method) py::make_function(method, py::return_value_policy<py::reference_existing_object>())
#define NewObject(method) py::make_function(method, py::return_value_policy<py::manage_new_object>())

BOOST_PYTHON_MODULE(mx_engine)
{
    py::def("MxEngineSetContextPointers", SetContextPointersWrapper);
    py::def("get_context", StaticVar(Application::Get));

    py::class_<Application, boost::noncopyable>("Application", py::no_init)
        .def("get", RefGetter(&Application::GetObject))
        .def("load", RefGetter(LoadObjectWrapper))
        .def("copy", RefGetter(&Application::CopyObject))
        .def("delete", &Application::DestroyObject)
        .def("remove_event", RemoveEventWrapper)
        .def("exit", &Application::CloseApplication)
        .def("dt", &Application::GetTimeDelta)
        .def("on_update", AddEventListenerWrapper)
        .add_property("viewport", RefGetter(ViewportWrapper))
        .add_property("log", RefGetter(&Application::GetLogger))
        ;

    py::class_<LoggerImpl, boost::noncopyable>("Logger", py::no_init)
        .def("error", &LoggerImpl::Error)
        .def("warning", &LoggerImpl::Warning)
        .def("debug", &LoggerImpl::Debug)
        .def("stacktrace", &LoggerImpl::StackTrace)
        .def("use_error", RefGetter(&LoggerImpl::UseError))
        .def("use_warning", RefGetter(&LoggerImpl::UseWarning))
        .def("use_debug", RefGetter(&LoggerImpl::UseDebug))
        .def("use_stacktrace", RefGetter(&LoggerImpl::UseStackTrace))
        ;

    py::class_<Vector4>("vec4", py::init<float>())
        .def(py::init<float, float, float, float>())
        .def("__str__", Vector4ToString)
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
        .def("__str__", Vector3ToString)
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
        .def("__str__", Vector2ToString)
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
        .def("__str__", Matrix4ToString)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self * float())
        .def(float() * py::self)
        .def(py::self / float())
        .def(-py::self)
        .def(+py::self)
        .def("__getitem__", Matrix4IndexGetWrapper)
        .def("__setitem__", Matrix4IndexSetWrapper)
        ;

    using TranslateFunc3F = CameraController & (CameraController::*)(float, float, float);
    using TranslateFunc3V = CameraController & (CameraController::*)(const Vector3&);

    py::class_<CameraController, boost::noncopyable>("CameraController", py::init())
        .def("has_camera", &CameraController::HasCamera)
        .add_property("camera", RefGetter(&CameraController::GetCamera))
        .add_property("camera_matrix", RefGetter(&CameraController::GetCameraMatrix))
        .add_property("move_speed", &CameraController::GetMoveSpeed, &CameraController::SetMoveSpeed)
        .add_property("rotate_speed", &CameraController::GetRotateSpeed, &CameraController::SetRotateSpeed)
        .add_property("position", RefGetter(&CameraController::GetPosition), &CameraController::SetPosition)
        .add_property("direction", RefGetter(&CameraController::GetDirection), &CameraController::SetDirection)
        .add_property("up", RefGetter(&CameraController::GetUp), &CameraController::SetUp)
        .add_property("zoom", &CameraController::GetZoom, &CameraController::SetZoom)
        .add_property("vec_forward", RefGetter(&CameraController::GetForwardVector), &CameraController::SetForwardVector)
        .add_property("vec_up", RefGetter(&CameraController::GetUpVector), &CameraController::SetUpVector)
        .add_property("vec_right", RefGetter(&CameraController::GetRightVector), &CameraController::SetRightVector)
        .add_property("horizontal_angle", &CameraController::GetHorizontalAngle)
        .add_property("vertical_angle", &CameraController::GetVerticalAngle)
        .def("translate", RefGetter((TranslateFunc3F)&CameraController::Translate))
        .def("translate", RefGetter((TranslateFunc3V)&CameraController::Translate))
        .def("translate_x", RefGetter(&CameraController::TranslateX))
        .def("translate_y", RefGetter(&CameraController::TranslateY))
        .def("translate_z", RefGetter(&CameraController::TranslateZ))
        .def("move_forward", RefGetter(&CameraController::TranslateForward))
        .def("move_up", RefGetter(&CameraController::TranslateUp))
        .def("move_right", RefGetter(&CameraController::TranslateRight))
        .def("rotate", RefGetter(&CameraController::Rotate))
        .def("orthographic", SetOrthographicCamera)
        .def("perspective", SetPerspectiveCamera)
        ;

    py::class_<ICameraWrapper, boost::noncopyable>("Camera")
        .def("set_view", &ICameraWrapper::SetViewMatrix)
        .add_property("matrix", RefGetter(&ICameraWrapper::GetMatrix))
        .add_property("aspect_ratio", &ICameraWrapper::GetAspectRatio, &ICameraWrapper::SetAspectRatio)
        .add_property("znear", &ICameraWrapper::GetZNear, &ICameraWrapper::SetZNear)
        .add_property("zfar", &ICameraWrapper::GetZFar, &ICameraWrapper::SetZFar)
        .add_property("zoom", &ICameraWrapper::GetZoom, &ICameraWrapper::SetZoom)
        ;

    py::class_<PerspectiveCamera, py::bases<ICamera>>("PerspectiveCamera")
        .def("set_view", &PerspectiveCamera::SetViewMatrix)
        .add_property("fov", &PerspectiveCamera::GetFOV, &PerspectiveCamera::SetFOV)
        .add_property("matrix", RefGetter(&PerspectiveCamera::GetMatrix))
        .add_property("aspect_ratio", &PerspectiveCamera::GetAspectRatio, &PerspectiveCamera::SetAspectRatio)
        .add_property("znear", &PerspectiveCamera::GetZNear, &PerspectiveCamera::SetZNear)
        .add_property("zfar", &PerspectiveCamera::GetZFar, &PerspectiveCamera::SetZFar)
        .add_property("zoom", &PerspectiveCamera::GetZoom, &PerspectiveCamera::SetZoom)
        ;

    py::class_<OrthographicCamera, py::bases<ICamera>>("OrthographicCamera")
        .def("set_view", &OrthographicCamera::SetViewMatrix)
        .def("set_projection", &OrthographicCamera::SetProjection)
        .add_property("matrix", RefGetter(&OrthographicCamera::GetMatrix))
        .add_property("aspect_ratio", &OrthographicCamera::GetAspectRatio, &OrthographicCamera::SetAspectRatio)
        .add_property("znear", &OrthographicCamera::GetZNear, &OrthographicCamera::SetZNear)
        .add_property("zfar", &OrthographicCamera::GetZFar, &OrthographicCamera::SetZFar)
        .add_property("zoom", &OrthographicCamera::GetZoom, &OrthographicCamera::SetZoom)
        ;

    // MxObject
    using ScaleFunc1F = MxObject & (MxObject::*)(float);
    using ScaleFuncVec = MxObject & (MxObject::*)(const Vector3&);
    using TranslateFunc3 = MxObject & (MxObject::*)(float, float, float);
    using RotateMoveFunc = MxObject & (MxObject::*)(float);
    using RotateFunc2F = MxObject & (MxObject::*)(float, float);

    // TODO: add all methods
    py::class_<MxObject, boost::noncopyable>("MxObject")
        .def("rotate", RefGetter((RotateFunc2F)&MxObject::Rotate))
        //.def("rotate_x", RefGetter(&MxObject::RotateX))
        //.def("rotate_y", RefGetter(&MxObject::RotateY))
        //.def("rotate_z", RefGetter(&MxObject::RotateZ))
        //.def("scale", RefGetter(((ScaleFunc1F)&MxObject::Scale)))
        //.def("scale", RefGetter(((ScaleFuncVec)&MxObject::Scale)))
        .def("translate", RefGetter(((TranslateFunc3)&MxObject::Translate)))
        //.def("translate_x", RefGetter(&MxObject::TranslateX))
        //.def("translate_y", RefGetter(&MxObject::TranslateY))
        //.def("translate_z", RefGetter(&MxObject::TranslateZ))
        .def("move_forward", RefGetter((RotateMoveFunc)&MxObject::TranslateForward))
        .def("move_right", RefGetter((RotateMoveFunc)&MxObject::TranslateRight))
        .def("move_up", RefGetter((RotateMoveFunc)&MxObject::TranslateUp))
        .def("hide", &MxObject::Hide)
        .def("show", &MxObject::Show)
        .def("set_texture", SetTextureWrapper)
        .def("set_shader", SetShaderWrapper)
        //.add_property("rotation", RefGetter(&MxObject::GetEulerRotation))
        //.add_property("translation", RefGetter(&MxObject::GetTranslation))
        //.add_property("get_scale", RefGetter(&MxObject::GetScale))
        .add_property("vec_forward", RefGetter(&MxObject::GetForwardVector), &MxObject::SetForwardVector)
        .add_property("vec_up", RefGetter(&MxObject::GetUpVector),&MxObject::SetUpVector)
        .add_property("vec_right", RefGetter(&MxObject::GetRightVector), &MxObject::SetRightVector)
        ;
}