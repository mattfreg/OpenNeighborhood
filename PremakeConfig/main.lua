include "../vendor/premake/solutionitems.lua"

workspace "OpenNeighborhood"
    location ".."
    architecture "x86_64"
    startproject "OpenNeighborhood"

    configurations { "Debug", "Release" }

    flags "MultiProcessorCompile"

    solutionitems {
        { ["EditorConfig"] = ".editorconfig" },
        { ["PremakeConfig"] = {
            "PremakeConfig/Glad.lua",
            "PremakeConfig/GLFW.lua",
            "PremakeConfig/ImGui.lua",
            "PremakeConfig/main.lua",
            "PremakeConfig/NativeFileDialogExtended.lua",
            "PremakeConfig/OpenNeighborhood.lua",
            "PremakeConfig/XBDM.lua"
        } }
    }

OutputDir = "%{cfg.buildcfg}"
TargetDir = "%{wks.location}/bin/%{OutputDir}"
ObjDir = "%{wks.location}/bin-int/%{OutputDir}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/OpenNeighborhood/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/OpenNeighborhood/vendor/Glad/include"

group "Dependencies"
    include "GLFW.lua"
    include "Glad.lua"
    include "ImGui.lua"
    include "NativeFileDialogExtended.lua"
group ""

include "OpenNeighborhood.lua"
include "XBDM.lua"
