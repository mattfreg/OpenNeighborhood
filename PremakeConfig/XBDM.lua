project "XBDM"
    location "%{wks.location}/%{prj.name}"
    kind "StaticLib"
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
        "%{prj.location}/vendor/**.h",
        "%{prj.location}/vendor/**.cpp"
    }

    includedirs { "%{prj.location}/src", "%{prj.location}/vendor/dirent" }

    filter "system:windows"
        systemversion "latest"
        links "Ws2_32.lib"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
