project "ImGui"
    location "%{wks.location}/OpenNeighborhood/vendor/%{prj.name}"
    kind "StaticLib"
    language "C++"
    staticruntime "on"

    targetdir "%{TargetDir}/%{prj.name}"
    objdir "%{ObjDir}/%{prj.name}"

    files {
        "%{prj.location}/*.h",
        "%{prj.location}/*.cpp",
        "%{prj.location}/backends/imgui_impl_glfw.cpp",
        "%{prj.location}/backends/imgui_impl_glfw.h",
        "%{prj.location}/backends/imgui_impl_opengl3.cpp",
        "%{prj.location}/backends/imgui_impl_opengl3.h"
    }

    includedirs {
        "%{prj.location}",
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
