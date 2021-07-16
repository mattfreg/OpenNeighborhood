project "Glad"
    location "%{wks.location}/OpenNeighborhood/vendor/%{prj.name}"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir "%{TargetDir}/%{prj.name}"
    objdir "%{ObjDir}/%{prj.name}"

    files {
        "%{prj.location}/include/glad/glad.h",
        "%{prj.location}/include/KHR/khrplatform.h",
        "%{prj.location}/src/glad.c"
    }

    includedirs "%{prj.location}/include"

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
