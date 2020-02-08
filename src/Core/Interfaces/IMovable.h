#pragma once

namespace MxEngine
{
    struct IMovable
    {
        virtual IMovable& Translate(float x, float y, float z) = 0;
        virtual IMovable& TranslateForward(float dist) = 0;
        virtual IMovable& TranslateRight(float dist) = 0;
        virtual IMovable& TranslateUp(float dist) = 0;
        virtual IMovable& Rotate(float horz, float vert) = 0;

        virtual void SetForwardVector(const Vector3& forward) = 0;
        virtual void SetUpVector(const Vector3& up) = 0;
        virtual void SetRightVector(const Vector3& right) = 0;

        virtual const Vector3& GetForwardVector() const = 0;
        virtual const Vector3& GetUpVector() const = 0;
        virtual const Vector3& GetRightVector() const = 0;

        virtual ~IMovable() = default;
    };
}