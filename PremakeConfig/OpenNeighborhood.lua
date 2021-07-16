project "OpenNeighborhood"
    location "%{wks.location}/%{prj.name}"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir "%{TargetDir}/%{prj.name}"
    objdir "%{ObjDir}/%{prj.name}"

    pchheader "pch.h"
    pchsource "../%{prj.name}/src/pch.cpp"

    files {
        "%{prj.location}/src/**.h",
        "%{prj.location}/src/**.cpp",
        "%{prj.location}/vendor/stb_image/**.h",
        "%{prj.location}/vendor/stb_image/**.cpp"
    }

    includedirs {
        "%{prj.location}/src",
        "%{prj.location}/vendor",
        "%{wks.location}/XBDM/src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{prj.location}/vendor/NativeFileDialogExtended/src/include"
    }

    defines "GLFW_INCLUDE_NONE"

    links {
        "GLFW",
        "Glad",
        "ImGui",
        "XBDM",
        "NativeFileDialogExtended"
    }

    filter "system:windows"
        postbuildcommands "{COPY} assets %{cfg.targetdir}/assets"

    filter "system:not windows"
        postbuildcommands "{COPY} assets %{cfg.targetdir}"

    filter "system:not macosx"
        linkgroups "on"

    filter "system:windows"
        systemversion "latest"
        links "opengl32.lib"

        defines "_CRT_SECURE_NO_WARNINGS"

    filter "system:linux"
        kind "ConsoleApp"
        systemversion "latest"
        links {
            "X11",
            "pthread",
            "dl"
        }

        -- I know it's not dynamic but NativeFileDialog links to gtk staticly
        -- and that's the only way I found to link everything successfully
        libdirs "%{TargetDir}/NativeFileDialogExtended"
        linkoptions "-lNativeFileDialogExtended `pkg-config --libs gtk+-3.0`"

    filter "system:macosx"
        kind "WindowedApp"
        linkoptions {
            "-framework Cocoa",
            "-framework IOKit",
            "-framework CoreFoundation"
        }

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "RELEASE"
        runtime "Release"
        optimize "on"

    filter { "configurations:Debug", "system:windows" }
        kind "ConsoleApp"

    filter { "configurations:Release", "system:windows" }
        kind "WindowedApp"
        linkoptions "/entry:mainCRTStartup"
