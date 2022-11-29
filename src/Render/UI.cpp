#include "pch.h"
#include "Render/UI.h"

#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Core/App.h"
#include "Helpers/Utils.h"
#include "Helpers/ConsoleStore.h"
#include "Helpers/LocationMover.h"
#include "Helpers/ConfigManager.h"
#include "Elements/Drive.h"
#include "Elements/AddXboxButton.h"

ImFont *UI::s_OpenSansBold = nullptr;
ImFont *UI::s_OpenSansRegular = nullptr;
ImFont *UI::s_OpenSansRegularBig = nullptr;
bool UI::s_Confirm = false;
std::string UI::s_ConfirmMessage;
UI::ConfirmCallbackFn UI::s_ConfirmCallback;
bool UI::s_Success = true;
std::string UI::s_ErrorMessage;
bool UI::s_InputText = false;
std::string UI::s_InputTextHeader;
UI::InputTextCallbackFn UI::s_InputTextCallback;
char UI::s_InputTextBuffer[50] = { 0 };
bool UI::s_DisplaySuccessModal = false;
std::string UI::s_SuccessMessage;

void UI::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    std::string openSansBoldPath = Utils::GetExecDir().append("assets").append("fonts").append("opensans").append("OpenSans-Bold.ttf").string();
    std::string openSansRegularPath = Utils::GetExecDir().append("assets").append("fonts").append("opensans").append("OpenSans-Regular.ttf").string();
    s_OpenSansBold = io.Fonts->AddFontFromFileTTF(openSansBoldPath.c_str(), 20.0f);
    s_OpenSansRegular = io.Fonts->AddFontFromFileTTF(openSansRegularPath.c_str(), 20.0f);
    s_OpenSansRegularBig = io.Fonts->AddFontFromFileTTF(openSansRegularPath.c_str(), 24.0f);
    io.FontDefault = s_OpenSansRegular;

    SetDarkThemeColors();

    App &app = App::Get();
    GLFWwindow *window = reinterpret_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void UI::BeginFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UI::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::Cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UI::DisplayConfirmModal()
{
    if (s_Confirm)
    {
        ImGui::OpenPopup("Confirm");

        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }

    if (ImGui::BeginPopupModal("Confirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(s_ConfirmMessage.c_str());

        if (ImGui::Button("OK", ImVec2(120.0f, 0.0f)))
        {
            if (s_ConfirmCallback)
                s_ConfirmCallback();

            s_Confirm = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
        {
            s_Confirm = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void UI::DisplayInputTextModal()
{
    if (s_InputText)
    {
        ImGui::OpenPopup(s_InputTextHeader.c_str());

        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }

    if (ImGui::BeginPopupModal(s_InputTextHeader.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("", s_InputTextBuffer, sizeof(s_InputTextBuffer));

        if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            ImGui::SetKeyboardFocusHere(-1);

        if (ImGui::Button("OK", ImVec2(120.0f, 0.0f)))
        {
            if (s_InputTextCallback)
                s_InputTextCallback(s_InputTextBuffer);

            s_InputText = false;
            memset(s_InputTextBuffer, 0, sizeof(s_InputTextBuffer));
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
        {
            s_InputText = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void UI::DisplayErrorModal()
{
    if (!s_Success)
    {
        ImGui::OpenPopup("Error");

        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }

    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(s_ErrorMessage.c_str());

        if (ImGui::Button("OK", ImVec2(120.0f, 0.0f)))
        {
            s_Success = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void UI::DisplaySuccessModal()
{
    if (s_DisplaySuccessModal)
    {
        ImGui::OpenPopup("Success");

        ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }

    if (ImGui::BeginPopupModal("Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(s_SuccessMessage.c_str());

        if (ImGui::Button("OK", ImVec2(120.0f, 0.0f)))
        {
            s_DisplaySuccessModal = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

std::vector<Ref<Element>> UI::CreateDriveElements()
{
    XBDM::Console &console = ConsoleStore::GetConsole();
    std::vector<XBDM::Drive> drives;
    auto driveElements = std::vector<Ref<Element>>();

    bool success = ConsoleStore::Try([&]() { drives = console.GetDrives(); });

    if (!success)
        return driveElements;

    LocationMover::SetCurrentAppLocation(LocationMover::AppLocation::DriveList);

    driveElements.reserve(drives.size());

    for (auto &drive : drives)
        driveElements.emplace_back(CreateRef<Drive>(drive));

    return driveElements;
}

std::vector<Ref<Element>> UI::CreateRootElements()
{
    auto rootElements = std::vector<Ref<Element>>();

    rootElements.emplace_back(CreateRef<AddXboxButton>());

    ConfigManager::Config config = ConfigManager::GetConfig();

    for (auto &[xboxName, _] : config)
        if (config.get(xboxName).has("ip_address"))
            rootElements.emplace_back(CreateRef<Xbox>(xboxName, config.get(xboxName).get("ip_address")));

    LocationMover::SetCurrentAppLocation(LocationMover::AppLocation::Root);

    return rootElements;
}

void UI::SetDarkThemeColors()
{
    auto &colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);
    colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 0.35f);

    // Headers
    colors[ImGuiCol_Header] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

    // Buttons
    colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.3805f, 0.381f, 1.0f);
    colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.2805f, 0.281f, 1.0f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
}
