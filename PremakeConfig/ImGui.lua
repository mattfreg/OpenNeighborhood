project "ImGui"
    location "%{wks.location}/OpenNeighborhood/vendor/%{prj.name}"
    kind "StaticLib"
    language "C++"
    staticruntime "on"

    targetdir "%{TargetDir}/%{prj.name}"
    objdir "%{ObjDir}/%{prj.name}"

    files {
        "%{prj.location}/imconfig.h",
        "%{prj.location}/imgui.h",
        "%{prj.location}/imgui.cpp",
        "%{prj.location}/imgui_custom_widgets.h",
        "%{prj.location}/imgui_custom_widgets.cpp",
        "%{prj.location}/imgui_draw.cpp",
        "%{prj.location}/imgui_impl_glfw.cpp",
        "%{prj.location}/imgui_impl_glfw.h",
        "%{prj.location}/imgui_impl_opengl3.cpp",
        "%{prj.location}/imgui_impl_opengl3.h",
        "%{prj.location}/imgui_internal.h",
        "%{prj.location}/imgui_widgets.cpp",
        "%{prj.location}/imstb_rectpack.h",
        "%{prj.location}/imstb_textedit.h",
        "%{prj.location}/imstb_truetype.h",
        "%{prj.location}/imgui_demo.cpp"
    }

    includedirs {
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}"
    }

    filter "system:windows"
        systemversion "latest"

    filter "system:linux"
        pic "on"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
