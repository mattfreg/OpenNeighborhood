project "OpenNeighborhood"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{wks.location}/" .. TargetDir .. "/%{prj.name}")
    objdir ("%{wks.location}/" .. ObjDir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "src/pch.cpp"

    files {
        "src/**.h",
        "src/**.cpp",
        "vendor/stb_image/**.h",
        "vendor/stb_image/**.cpp"
    }

    includedirs {
        "src",
        "vendor",
        "%{wks.location}/XBDM/src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "vendor/nativefiledialog-extended/src/include"
    }

    defines {
        "GLFW_INCLUDE_NONE"
    }

    links {
        "GLFW",
        "Glad",
        "ImGui",
        "XBDM",
        "NativeFileDialogExtended"
    }

    filter "system:windows"
        postbuildcommands {
            "{COPY} assets %{cfg.targetdir}/assets"
        }

    filter "system:not windows"
        postbuildcommands {
            "{COPY} assets %{cfg.targetdir}"
        }

    filter "system:not macosx"
        linkgroups "on"

    filter "system:windows"
        systemversion "latest"
        links {
            "opengl32.lib"
        }

    filter "system:linux"
        systemversion "latest"
        links {
            "X11",
            "pthread",
            "dl"
        }

        -- I know it's not dynamic but NativeFileDialog links to gtk staticly
        -- and that's the only way I found to link everything successfully
        libdirs "vendor/nativefiledialog-extended/bin/%{cfg.buildcfg}/NativeFileDialogExtended"
        linkoptions "-lNativeFileDialogExtended `pkg-config --libs gtk+-3.0`"

    filter "system:macosx"
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
        runtime "Release"
        defines "RELEASE"
        optimize "on"
