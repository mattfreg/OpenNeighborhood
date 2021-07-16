project "NativeFileDialogExtended"
    location "%{wks.location}/OpenNeighborhood/vendor/%{prj.name}"
    kind "StaticLib"
    language "C++"
    staticruntime "on"

    targetdir "%{TargetDir}/%{prj.name}"
    objdir "%{ObjDir}/%{prj.name}"

    files {
        "%{prj.location}/src/include/nfd.h",
        "%{prj.location}/src/include/nfd.hpp"
    }

    includedirs "%{prj.location}/src/include"

    filter "system:linux"
        pic "on"

        systemversion "latest"

        files "%{prj.location}/src/nfd_gtk.cpp"

        buildoptions "`pkg-config --cflags gtk+-3.0`"

    filter "system:windows"
        systemversion "latest"

        files "%{prj.location}/src/nfd_win.cpp"

    filter "system:macosx"
        files "%{prj.location}/src/nfd_cocoa.m"

        linkoptions "-framework AppKit"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
