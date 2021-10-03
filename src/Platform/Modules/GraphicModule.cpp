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

#include "GraphicModule.h"
#include "Core/Config/GlobalConfig.h"
#include "Utilities/Logging/Logger.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/ImGui/ImGuiBase.h"
#include "Utilities/FileSystem/FileManager.h"
#include <VulkanAbstractionLayer/VulkanContext.h>
#include <VulkanAbstractionLayer/ImGuiContext.h>

namespace MxEngine
{
    struct GraphicModuleImpl
    {
        VulkanAbstractionLayer::Window* Window = nullptr;
        VulkanAbstractionLayer::VulkanContext* Vulkan = nullptr;
        ImGuiContext* ImGui = nullptr;
    };

    void GraphicModule::Init()
    {
        impl = new GraphicModuleImpl;
    }

    GraphicModuleImpl* GraphicModule::GetImpl()
    {
        return impl;
    }

    void GraphicModule::Clone(GraphicModuleImpl* context)
    {
        impl = context;
        if(impl->ImGui != nullptr)
            ImGui::SetCurrentContext(impl->ImGui);
        if (impl->Window != nullptr)
            impl->Window->SetContext(impl->Window->GetNativeHandle());
        if (impl->Vulkan != nullptr)
            VulkanAbstractionLayer::SetCurrentVulkanContext(*impl->Vulkan);
    }
    
    static void GraphicModuleErrorCallback(const std::string& message)
    {
        MXLOG_FATAL("MxEngine::GraphicModule", ToMxString(message));
    }

    static void GraphicModuleInfoCallback(const std::string& message)
    {
        MXLOG_INFO("MxEngine::GraphicModule", ToMxString(message));
    }

    void GraphicModule::OnWindowCreate(WindowHandle window)
    {
        impl->Window = static_cast<VulkanAbstractionLayer::Window*>(window);
        impl->Window->OnResize([](VulkanAbstractionLayer::Window& window, Vector2 newSize)
            {
                GraphicModule::GetImpl()->Vulkan->RecreateSwapchain((uint32_t)newSize.x, (uint32_t)newSize.y);
            });

        VulkanAbstractionLayer::VulkanContextCreateOptions vulkanOptions;
        vulkanOptions.ApplicationName = GlobalConfig::GetWindowTitle().c_str();
        vulkanOptions.EngineName = "MxEngine";
        vulkanOptions.Extensions = impl->Window->GetRequiredExtensions();
        vulkanOptions.VulkanApiMajorVersion = 1;
        vulkanOptions.VulkanApiMinorVersion = 2;
        if (GlobalConfig::GetBuildType() != BuildType::SHIPPING)
            vulkanOptions.Layers = { "VK_LAYER_KHRONOS_validation" };
        vulkanOptions.ErrorCallback = GraphicModuleErrorCallback;
        vulkanOptions.InfoCallback = GraphicModuleInfoCallback;

        impl->Vulkan = new VulkanAbstractionLayer::VulkanContext(vulkanOptions);
        VulkanAbstractionLayer::SetCurrentVulkanContext(*impl->Vulkan);

        VulkanAbstractionLayer::ContextInitializeOptions contextOptions;
        contextOptions.ErrorCallback = GraphicModuleErrorCallback;
        contextOptions.InfoCallback = GraphicModuleInfoCallback;
        contextOptions.PreferredDeviceType = VulkanAbstractionLayer::DeviceType::DISCRETE_GPU;
        impl->Vulkan->InitializeContext(impl->Window->CreateWindowSurface(*impl->Vulkan), contextOptions);
    }

    void GraphicModule::OnFrameBegin()
    {
        if(impl->Vulkan->IsRenderingEnabled())
            impl->Vulkan->StartFrame();

        if (impl->ImGui == nullptr && ImGui::GetCurrentContext() != nullptr)
            impl->ImGui = ImGui::GetCurrentContext();

        if (impl->ImGui != nullptr)
            VulkanAbstractionLayer::ImGuiVulkanContext::StartFrame();
    }

    void GraphicModule::OnWindowDestroy(WindowHandle window)
    {
        if (impl->ImGui != nullptr)
            VulkanAbstractionLayer::ImGuiVulkanContext::Destroy();

        if (impl->Vulkan != nullptr)
            delete impl->Vulkan;

        impl->Window = nullptr;
        impl->Vulkan = nullptr;
        impl->ImGui = nullptr;
    }

    void GraphicModule::OnFrameEnd()
    {
        if (impl->Vulkan->IsRenderingEnabled())
            impl->Vulkan->EndFrame();

        if (impl->ImGui != nullptr)
            VulkanAbstractionLayer::ImGuiVulkanContext::EndFrame();
    }

    void GraphicModule::Destroy()
    {
        delete impl;
    }
}