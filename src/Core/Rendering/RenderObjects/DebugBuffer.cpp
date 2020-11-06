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

#include "DebugBuffer.h"
#include "Core/BoundingObjects/AABB.h"
#include "Core/BoundingObjects/BoundingBox.h"
#include "Core/BoundingObjects/BoundingSphere.h"
#include "Core/BoundingObjects/Cylinder.h"
#include "Core/BoundingObjects/Capsule.h"
#include "Core/BoundingObjects/Cone.h"
#include "Core/BoundingObjects/Frustrum.h"
#include "Core/BoundingObjects/Line.h"
#include "Core/BoundingObjects/Rectangle.h"
#include "Core/BoundingObjects/Circle.h"

namespace MxEngine
{
    void DebugBuffer::Init()
    {
        auto VBL = GraphicFactory::Create<VertexBufferLayout>();
        VBL->PushFloat(3); // position
        VBL->PushFloat(4); // color //-V112

        this->VBO = GraphicFactory::Create<VertexBuffer>(nullptr, 0, UsageType::DYNAMIC_DRAW);
        this->VAO = GraphicFactory::Create<VertexArray>();
        VAO->AddBuffer(*this->VBO, *VBL);
    }

    void DebugBuffer::Submit(const Line& line, const Vector4& color)
    {
        this->storage.push_back({ line.p1, color });
        this->storage.push_back({ line.p2, color });
    }

    void DebugBuffer::Submit(const AABB& box, const Vector4& color)
    {
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Min.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Min.x, box.Max.y, box.Max.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Min.y, box.Min.z), color });
        this->storage.push_back({ MakeVector3(box.Max.x, box.Max.y, box.Min.z), color });
    }

    void DebugBuffer::Submit(const BoundingBox& box, const Vector4 color)
    {
        std::array points = {
            MakeVector3(box.Min.x, box.Min.y, box.Min.z),
            MakeVector3(box.Max.x, box.Min.y, box.Min.z),
            MakeVector3(box.Min.x, box.Max.y, box.Min.z),
            MakeVector3(box.Min.x, box.Min.y, box.Max.z),
            MakeVector3(box.Max.x, box.Max.y, box.Max.z),
            MakeVector3(box.Min.x, box.Max.y, box.Max.z),
            MakeVector3(box.Max.x, box.Min.y, box.Max.z),
            MakeVector3(box.Max.x, box.Max.y, box.Min.z),
        };

        std::array lines = {
            Line{ points[0], points[1] },
            Line{ points[0], points[2] },
            Line{ points[0], points[3] },
            Line{ points[4], points[5] },
            Line{ points[4], points[6] },
            Line{ points[4], points[7] },
            Line{ points[2], points[5] },
            Line{ points[2], points[7] },
            Line{ points[6], points[1] },
            Line{ points[6], points[3] },
            Line{ points[3], points[5] },
            Line{ points[1], points[7] },
        };
        for (auto& line : lines)
        {
            this->storage.push_back({ box.Rotation * line.p1 + box.Center, color });
            this->storage.push_back({ box.Rotation * line.p2 + box.Center, color });
        }
    }

    auto InitializeSphere()
    {
        std::vector<Vector3> linesData;
        std::vector<Vector3> vertecies;
        constexpr size_t SpherePolygons = 8;
        vertecies.reserve((SpherePolygons + 1) * (SpherePolygons + 1));
        linesData.reserve((SpherePolygons + 1) * (SpherePolygons + 1) * 12);

        for (size_t m = 0; m <= SpherePolygons; m++)
        {
            for (size_t n = 0; n <= SpherePolygons; n++)
            {
                float x = std::sin(Pi<float>() * m / SpherePolygons) * std::cos(2 * Pi<float>() * n / SpherePolygons);
                float y = std::sin(Pi<float>() * m / SpherePolygons) * std::sin(2 * Pi<float>() * n / SpherePolygons);
                float z = std::cos(Pi<float>() * m / SpherePolygons);
                vertecies.emplace_back(x, y, z);
            }
        }

        for (size_t i = 0; i < SpherePolygons; i++)
        {
            size_t idx1 = i * (SpherePolygons + 1);
            size_t idx2 = idx1 + SpherePolygons + 1;

            for (size_t j = 0; j < SpherePolygons; j++, idx1++, idx2++)
            {
                if (i != 0)
                {
                    linesData.push_back(vertecies[idx1]); //-V525
                    linesData.push_back(vertecies[idx2]);
                    linesData.push_back(vertecies[idx2]);
                    linesData.push_back(vertecies[idx1 + 1]);
                    linesData.push_back(vertecies[idx1 + 1]);
                    linesData.push_back(vertecies[idx1]);
                }
                if (i + 1 != SpherePolygons)
                {
                    linesData.push_back(vertecies[idx1 + 1]);
                    linesData.push_back(vertecies[idx2]);
                    linesData.push_back(vertecies[idx2]);
                    linesData.push_back(vertecies[idx2 + 1]); //-V525
                    linesData.push_back(vertecies[idx2 + 1]);
                    linesData.push_back(vertecies[idx1 + 1]);
                }
            }
        }

        return linesData;
    }

    void DebugBuffer::Submit(const BoundingSphere& sphere, const Vector4 color)
    {
        static const auto sphereData = InitializeSphere();

        for (const Vector3& v : sphereData)
        {
            this->storage.push_back({ sphere.Radius * v * 1.1f + sphere.Center, color });
        }
    }

    void DebugBuffer::Submit(const Cone& cone, const Vector4& color)
    {
        auto radius = std::tan(Radians(cone.GetAngle() * 0.5f)) * cone.GetLength();
        auto normDir = Normalize(cone.Direction);
        if (cone.GetAngle() > 180.0f) normDir = -normDir;   
        auto direction = normDir * cone.GetLength();

        Vector3 v{ 0.0f };
        Vector3 u{ 0.0f };
        if (direction.y != 0.0f)
            v = MakeVector3(-normDir.y, normDir.x, 0.0f);
        else if (direction.z != 0.0f)
            v = MakeVector3(0.0f, -normDir.z, normDir.y);
        else
            v = MakeVector3(-normDir.z, 0.0f, normDir.x);

        v = Normalize(v);
        u = Normalize(Cross(normDir, v)); //-V519

        std::array circle = {
             v,
             Normalize(2.0f * v / 3.0f + u / 3.0f),
             Normalize(v / 3.0f + 2.0f * u / 3.0f),
             u,
             Normalize(2.0f * u / 3.0f - v / 3.0f),
             Normalize(u / 3.0f - 2.0f * v / 3.0f),
             -v,
             Normalize(-2.0f * v / 3.0f - u / 3.0f),
             Normalize(-v / 3.0f - 2.0f * u / 3.0f),
             -u,
             Normalize(v / 3.0f - 2.0f * u / 3.0f),
             Normalize(2.0f * v / 3.0f - u / 3.0f),
        };

        for (auto& point : circle)
        {
            point = radius * point + direction + cone.Origin;
        }

        for (size_t i = 0; i < circle.size(); i += 2)
        {
            this->storage.push_back({ cone.Origin, color });
            this->storage.push_back({ circle[i],   color });
        }
        for (size_t i = 1; i < circle.size(); i++)
        {
            this->storage.push_back({circle[i - 1], color});
            this->storage.push_back({circle[i],     color});
        }
        this->storage.push_back({ circle.back(),  color });
        this->storage.push_back({ circle.front(), color });
    }

    void DebugBuffer::Submit(const Frustrum& frustrum, const Vector4& color)
    {
        auto normDir = Normalize(frustrum.Direction);
        auto normUp = Normalize(frustrum.Up);

        auto normRight = Normalize(Cross(normDir, normUp)); //-V537
        auto right = normRight * frustrum.AspectRatio;
        auto scale = 2.0f;

        std::array plane = {
           scale * (std::tan(Radians(frustrum.GetAngle() * 0.5f)) * ( 2.0f * right + 2.0f * normUp) + normDir) + frustrum.Origin,
           scale * (std::tan(Radians(frustrum.GetAngle() * 0.5f)) * ( 2.0f * right - 2.0f * normUp) + normDir) + frustrum.Origin,
           scale * (std::tan(Radians(frustrum.GetAngle() * 0.5f)) * (-2.0f * right - 2.0f * normUp) + normDir) + frustrum.Origin,
           scale * (std::tan(Radians(frustrum.GetAngle() * 0.5f)) * (-2.0f * right + 2.0f * normUp) + normDir) + frustrum.Origin,
        };

        for (size_t i = 0; i < plane.size(); i++)
        {
            this->storage.push_back({ frustrum.Origin, color });
            this->storage.push_back({ plane[i],   color });
        }
        for (size_t i = 1; i < plane.size(); i++)
        {
            this->storage.push_back({ plane[i - 1], color });
            this->storage.push_back({ plane[i],     color });
        }
        this->storage.push_back({ plane.back(),  color });
        this->storage.push_back({ plane.front(), color });
    }

    void DebugBuffer::Submit(const Cylinder& cylinder, const Vector4& color)
    {
        constexpr std::array circle =
        {
            Vector2(0.0f, 1.0f),
            Vector2(0.5f, RootThree<float>() * 0.5f),
            Vector2(OneOverRootTwo<float>(), OneOverRootTwo<float>()),
            Vector2(RootThree<float>() * 0.5f, 0.5f),
            Vector2(1.0f, 0.0f),
            Vector2(RootThree<float>() * 0.5f, -0.5f),
            Vector2(OneOverRootTwo<float>(), -OneOverRootTwo<float>()),
            Vector2(0.5f, -RootThree<float>() * 0.5f),
            Vector2(0.0f, -1.0f),
            Vector2(-0.5f, -RootThree<float>() * 0.5f),
            Vector2(-OneOverRootTwo<float>(), -OneOverRootTwo<float>()),
            Vector2(-RootThree<float>() * 0.5f, -0.5f),
            Vector2(-1.0f, 0.0f),
            Vector2(-RootThree<float>() * 0.5f, 0.5f),
            Vector2(-OneOverRootTwo<float>(), OneOverRootTwo<float>()),
            Vector2(-0.5f, RootThree<float>() * 0.5f),
        };

        std::array<Vector3, circle.size()> upper;
        std::array<Vector3, circle.size()> lower;

        Vector3 center{ 0.0f };
        Vector3 offset{ 0.0f };
        for (size_t i = 0; i < circle.size(); i++)
        {
            switch (cylinder.Orientation)
            {
            case Cylinder::Axis::X:
                center = MakeVector3(0.0f, circle[i].x * cylinder.RadiusX, circle[i].y * cylinder.RadiusZ);
                offset.x = cylinder.Height * 0.5f;
                break;
            case Cylinder::Axis::Y:
                center = MakeVector3(circle[i].x * cylinder.RadiusX, 0.0f, circle[i].y * cylinder.RadiusZ);
                offset.y = cylinder.Height * 0.5f;
                break;
            case Cylinder::Axis::Z:
                center = MakeVector3(circle[i].x * cylinder.RadiusX, circle[i].y, 0.0f * cylinder.RadiusZ);
                offset.z = cylinder.Height * 0.5f;
                break;
            }
            upper[i] = center - offset;
            lower[i] = center + offset;

            upper[i] = cylinder.Rotation * upper[i] + cylinder.Center;
            lower[i] = cylinder.Rotation * lower[i] + cylinder.Center;
        }
        for (size_t i = 0; i < circle.size(); i++)
        {
            this->storage.push_back({ lower[i], color });
            this->storage.push_back({ upper[i], color });
        }
        for (size_t i = 1; i < circle.size(); i++)
        {
            this->storage.push_back({ lower[i - 1], color });
            this->storage.push_back({ lower[i],     color });
            this->storage.push_back({ upper[i - 1], color });
            this->storage.push_back({ upper[i],     color });
        }
        this->storage.push_back({ lower.front(), color });
        this->storage.push_back({ lower.back(),  color });
        this->storage.push_back({ upper.front(), color });
        this->storage.push_back({ upper.back(),  color });
    }

    void DebugBuffer::Submit(const Capsule& capsule, const Vector4& color)
    {
        std::array hemisphere = {
            Vector2(0.0f, 1.0f),
            Vector2(0.5f, RootThree<float>() * 0.5f),
            Vector2(OneOverRootTwo<float>(), OneOverRootTwo<float>()),
            Vector2(RootThree<float>() * 0.5f, 0.5f),
            Vector2(1.0f, 0.0f),
        };

        float halfHeight = capsule.Height * 0.5f;
        float radius = capsule.Radius;
        Quaternion rotation = capsule.Rotation;
        Vector3 position = capsule.Center;

        std::array<Vector3, hemisphere.size() * 4> ovale1, ovale2;
        size_t offset1 = 0, offset2 = hemisphere.size(), offset3 = 2 * hemisphere.size(), offset4 = 3 * hemisphere.size();
        for (size_t i = 0; i < hemisphere.size(); i++)
        {
            auto h = hemisphere[i];
            switch (capsule.Orientation)
            {
            case Capsule::Axis::X:
                ovale1[offset1 + i] = MakeVector3( h.x,  h.y, 0.0f);
                ovale2[offset1 + i] = MakeVector3( h.x, 0.0f,  h.y);
                ovale1[offset2 + i] = MakeVector3( h.y, -h.x, 0.0f);
                ovale2[offset2 + i] = MakeVector3( h.y, 0.0f, -h.x);
                ovale1[offset3 + i] = MakeVector3(-h.x, -h.y, 0.0f);
                ovale2[offset3 + i] = MakeVector3(-h.x, 0.0f, -h.y);
                ovale1[offset4 + i] = MakeVector3(-h.y,  h.x, 0.0f);
                ovale2[offset4 + i] = MakeVector3(-h.y, 0.0f,  h.x);
                break;
            case Capsule::Axis::Y:
                ovale1[offset1 + i] = MakeVector3(0.0f,  h.x, h.y);
                ovale2[offset1 + i] = MakeVector3( h.y,  h.x, 0.0f);
                ovale1[offset2 + i] = MakeVector3(0.0f,  h.y, -h.x);
                ovale2[offset2 + i] = MakeVector3(-h.x,  h.y, 0.0f);
                ovale1[offset3 + i] = MakeVector3(0.0f, -h.x, -h.y);
                ovale2[offset3 + i] = MakeVector3(-h.y, -h.x, 0.0f);
                ovale1[offset4 + i] = MakeVector3(0.0f, -h.y,  h.x);
                ovale2[offset4 + i] = MakeVector3( h.x, -h.y, 0.0f);
                break;
            case Capsule::Axis::Z:
                ovale1[offset1 + i] = MakeVector3(0.0f,  h.y,  h.x);
                ovale2[offset1 + i] = MakeVector3( h.y, 0.0f,  h.x);
                ovale1[offset2 + i] = MakeVector3(0.0f,  h.x,  h.y);
                ovale2[offset2 + i] = MakeVector3( h.x, 0.0f,  h.y);
                ovale1[offset3 + i] = MakeVector3(0.0f, -h.y, -h.x);
                ovale2[offset3 + i] = MakeVector3(-h.y, 0.0f, -h.x);
                ovale1[offset4 + i] = MakeVector3(0.0f,  h.x, -h.y);
                ovale2[offset4 + i] = MakeVector3( h.x, 0.0f, -h.y);
                break;
            }
        }    
        
        Vector3 heightVec{ 0.0f };
        switch (capsule.Orientation)
        {
        case Capsule::Axis::X:
            heightVec.x = halfHeight;
            break;
        case Capsule::Axis::Y:
            heightVec.y = halfHeight;
            break;
        case Capsule::Axis::Z:
            heightVec.z = halfHeight;
            break;
        }

        for (size_t i = 0; i < hemisphere.size() * 2; i++)
        {
            ovale1[i] = rotation * (ovale1[i] * radius + heightVec) + position;
            ovale2[i] = rotation * (ovale2[i] * radius + heightVec) + position;

        }
        for (size_t i = 0; i < hemisphere.size() * 2; i++)
        {
            ovale1[hemisphere.size() * 2 + i] = rotation * (ovale1[hemisphere.size() * 2 + i] * radius - heightVec) + position;
            ovale2[hemisphere.size() * 2 + i] = rotation * (ovale2[hemisphere.size() * 2 + i] * radius - heightVec) + position;
        }
        for (size_t i = 1; i < ovale1.size(); i++)
        {
            this->storage.push_back({ ovale1[i - 1], color });
            this->storage.push_back({ ovale1[i], color });
            this->storage.push_back({ ovale2[i - 1], color });
            this->storage.push_back({ ovale2[i], color });
        }
        this->storage.push_back({ ovale1.back(), color });
        this->storage.push_back({ ovale1.front(), color });
        this->storage.push_back({ ovale2.back(), color });
        this->storage.push_back({ ovale2.front(), color });
    }

    void DebugBuffer::Submit(const Rectangle& rectangle, const Vector4& color)
    {
        std::array points = {
            rectangle.Center + rectangle.Rotation * Vector3(-0.5f * rectangle.Width, 0.0f, +0.5f * rectangle.Height),      // Left top
            rectangle.Center + rectangle.Rotation * Vector3(+0.5f * rectangle.Width, 0.0f, +0.5f * rectangle.Height),      // Right top
            rectangle.Center + rectangle.Rotation * Vector3(+0.5f * rectangle.Width, 0.0f, -0.5f * rectangle.Height),      // Right bottom
            rectangle.Center + rectangle.Rotation * Vector3(-0.5f * rectangle.Width, 0.0f, -0.5f * rectangle.Height),      // Left bottom
        };

        for (size_t i = 0; i < points.size() - 1; i++) {
            auto pointFrom = points[i];
            auto pointTo = points[i + 1];

            this->storage.push_back({ pointFrom, color });
            this->storage.push_back({ pointTo, color });
        }

        this->storage.push_back({ points.back(), color });
        this->storage.push_back({ points.front(), color });
    }

    void DebugBuffer::Submit(const Circle& circle, const Vector4& color)
    {
        constexpr std::array circlePoints =
        {
            Vector3(0.0f, 0.0f, 1.0f),
            Vector3(0.5f, 0.0f, RootThree<float>() * 0.5f),
            Vector3(OneOverRootTwo<float>(), 0.0f, OneOverRootTwo<float>()),
            Vector3(RootThree<float>() * 0.5f, 0.0f, 0.5f),
            Vector3(1.0f, 0.0f, 0.0f),
            Vector3(RootThree<float>() * 0.5f, 0.0f, -0.5f),
            Vector3(OneOverRootTwo<float>(), 0, -OneOverRootTwo<float>()),
            Vector3(0.5f, 0.0f, -RootThree<float>() * 0.5f),
            Vector3(0.0f, 0.0f, -1.0f),
            Vector3(-0.5f, 0.0f, -RootThree<float>() * 0.5f),
            Vector3(-OneOverRootTwo<float>(), 0.0f, -OneOverRootTwo<float>()),
            Vector3(-RootThree<float>() * 0.5f, 0.0f, -0.5f),
            Vector3(-1.0f, 0.0f, 0.0f),
            Vector3(-RootThree<float>() * 0.5f, 0.0f, 0.5f),
            Vector3(-OneOverRootTwo<float>(), 0.0f, OneOverRootTwo<float>()),
            Vector3(-0.5f, 0.0f, RootThree<float>() * 0.5f),
        };

        std::array<Vector3, circlePoints.size()> circleArray;
        for (size_t i = 0; i < circlePoints.size(); i++)
        {
            circleArray[i] = circle.Center + circle.Rotation * circlePoints[i] * circle.Radius;
        }

        for (size_t i = 0; i < circleArray.size() - 1; i++) {
            auto pointFrom = circleArray[i];
            auto pointTo = circleArray[i + 1];

            this->storage.push_back({ pointFrom, color });
            this->storage.push_back({ pointTo, color });
        }

        this->storage.push_back({ circleArray.back(), color });
        this->storage.push_back({ circleArray.front(), color });
    }

    void DebugBuffer::ClearBuffer()
    {
        this->storage.clear();
    }

    void DebugBuffer::SubmitBuffer()
    {
        size_t size = this->GetSize() * sizeof(Point) / sizeof(float);
        if (size <= this->VBO->GetSize())
            this->VBO->BufferSubData((float*)this->storage.data(), size);
        else
            this->VBO->Load((float*)this->storage.data(), size, UsageType::DYNAMIC_DRAW);
    }

    size_t DebugBuffer::GetSize() const
    {
        return this->storage.size();
    }

    VertexArrayHandle DebugBuffer::GetVAO() const
    {
        return this->VAO;
    }
}