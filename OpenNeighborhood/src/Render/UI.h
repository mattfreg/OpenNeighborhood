#pragma once

#include <ImGui/imgui.h>

class UI
{
public:
    static void Init();

    static void BeginFrame();
    static void EndFrame();

    static void Cleanup();

    inline static ImFont* GetBoldFont() { return s_OpenSansBold; }
    inline static ImFont* GetRegularFont() { return s_OpenSansRegular; }
    inline static ImFont* GetRegularBigFont() { return s_OpenSansRegularBig; }
private:
    static void SetDarkThemeColors();

    static ImFont* s_OpenSansBold;
    static ImFont* s_OpenSansRegular;
    static ImFont* s_OpenSansRegularBig;
};
