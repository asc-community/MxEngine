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
#include "VulkanAbstractionLayer/VulkanContext.h"
#include "VulkanAbstractionLayer/ImGuiContext.h"

namespace MxEngine
{
    struct GraphicModuleImpl
    {
        VulkanAbstractionLayer::Window* Window = nullptr;
        VulkanAbstractionLayer::VulkanContext* Vulkan = nullptr;
        ImGuiContext* ImGui = nullptr;
    };

    void ImGuiStyleColorsMxEngineCustom()
    {
        constexpr auto ColorFromBytes = [](uint8_t r, uint8_t g, uint8_t b)
        {
            return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
        };

        auto& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        const ImVec4 bgColor = ColorFromBytes(37, 37, 38);
        const ImVec4 lightBgColor = ColorFromBytes(82, 82, 85);
        const ImVec4 veryLightBgColor = ColorFromBytes(90, 90, 95);

        const ImVec4 panelColor = ColorFromBytes(51, 51, 55);
        const ImVec4 panelHoverColor = ColorFromBytes(29, 151, 236);
        const ImVec4 panelActiveColor = ColorFromBytes(0, 119, 200);

        const ImVec4 textColor = ColorFromBytes(255, 255, 255);
        const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
        const ImVec4 borderColor = ColorFromBytes(78, 78, 78);

        colors[ImGuiCol_Text] = textColor;
        colors[ImGuiCol_TextDisabled] = textDisabledColor;
        colors[ImGuiCol_TextSelectedBg] = panelActiveColor;
        colors[ImGuiCol_WindowBg] = bgColor;
        colors[ImGuiCol_ChildBg] = bgColor;
        colors[ImGuiCol_PopupBg] = bgColor;
        colors[ImGuiCol_Border] = borderColor;
        colors[ImGuiCol_BorderShadow] = borderColor;
        colors[ImGuiCol_FrameBg] = panelColor;
        colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
        colors[ImGuiCol_FrameBgActive] = panelActiveColor;
        colors[ImGuiCol_TitleBg] = bgColor;
        colors[ImGuiCol_TitleBgActive] = bgColor;
        colors[ImGuiCol_TitleBgCollapsed] = bgColor;
        colors[ImGuiCol_MenuBarBg] = panelColor;
        colors[ImGuiCol_ScrollbarBg] = panelColor;
        colors[ImGuiCol_ScrollbarGrab] = lightBgColor;
        colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
        colors[ImGuiCol_ScrollbarGrabActive] = veryLightBgColor;
        colors[ImGuiCol_CheckMark] = panelActiveColor;
        colors[ImGuiCol_SliderGrab] = panelHoverColor;
        colors[ImGuiCol_SliderGrabActive] = panelActiveColor;
        colors[ImGuiCol_Button] = panelColor;
        colors[ImGuiCol_ButtonHovered] = panelHoverColor;
        colors[ImGuiCol_ButtonActive] = panelHoverColor;
        colors[ImGuiCol_Header] = lightBgColor;
        colors[ImGuiCol_HeaderHovered] = panelHoverColor;
        colors[ImGuiCol_HeaderActive] = panelActiveColor;
        colors[ImGuiCol_Separator] = borderColor;
        colors[ImGuiCol_SeparatorHovered] = borderColor;
        colors[ImGuiCol_SeparatorActive] = borderColor;
        colors[ImGuiCol_ResizeGrip] = bgColor;
        colors[ImGuiCol_ResizeGripHovered] = panelColor;
        colors[ImGuiCol_ResizeGripActive] = lightBgColor;
        colors[ImGuiCol_PlotLines] = panelActiveColor;
        colors[ImGuiCol_PlotLinesHovered] = panelHoverColor;
        colors[ImGuiCol_PlotHistogram] = panelActiveColor;
        colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
        colors[ImGuiCol_ModalWindowDarkening] = bgColor;
        colors[ImGuiCol_DragDropTarget] = bgColor;
        colors[ImGuiCol_NavHighlight] = bgColor;
        colors[ImGuiCol_DockingPreview] = panelActiveColor;
        colors[ImGuiCol_Tab] = bgColor;
        colors[ImGuiCol_TabActive] = panelActiveColor;
        colors[ImGuiCol_TabUnfocused] = bgColor;
        colors[ImGuiCol_TabUnfocusedActive] = panelActiveColor;
        colors[ImGuiCol_TabHovered] = panelHoverColor;

        style.WindowRounding = 0.0f;
        style.ChildRounding = 0.0f;
        style.FrameRounding = 0.0f;
        style.GrabRounding = 0.0f;
        style.PopupRounding = 0.0f;
        style.ScrollbarRounding = 0.0f;
        style.TabRounding = 0.0f;
    }

    void InitializeImGui()
    {
        MAKE_SCOPE_PROFILER("ImGui::Init");
        MAKE_SCOPE_TIMER("MxEngine::GraphicModule", "ImGui::Init()");

        auto& imguiIO = ImGui::GetIO();
        imguiIO.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
        imguiIO.ConfigDockingAlwaysTabBar = true;

        switch (GlobalConfig::GetEditorStyle())
        {
        case EditorStyle::CLASSIC:
            ImGui::StyleColorsClassic();
            break;
        case EditorStyle::DARK:
            ImGui::StyleColorsDark();
            break;
        case EditorStyle::LIGHT:
            ImGui::StyleColorsLight();
            break;
        case EditorStyle::MXENGINE:
            ImGuiStyleColorsMxEngineCustom();
            break;
        }
    }

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
    }

    void GraphicModule::OnWindowDestroy(WindowHandle window)
    {
        delete impl->Vulkan;
        impl->Window = nullptr;
        impl->Vulkan = nullptr;
        impl->ImGui = nullptr;
    }

    void GraphicModule::OnFrameEnd()
    {
        if (impl->Vulkan->IsRenderingEnabled())
            impl->Vulkan->EndFrame();
    }

    void GraphicModule::Destroy()
    {
        delete impl;
    }
}