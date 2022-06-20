require("../tools/premake/solutionitems")

Dependencies = {
  basePath = path.join(".", "deps"),
}

function Dependencies.load()
  local dir = path.join(Dependencies.basePath, "premake", "*.lua")
  local deps = os.matchfiles(dir)

  for _, dep in pairs(deps) do
    dep = dep:gsub(".lua", "")
    require(dep)
  end
end

function Dependencies.imports()
  for i, proj in pairs(Dependencies) do
    if type(i) == "number" then
      proj:import()
    end
  end
end

function Dependencies.projects()
  for i, proj in pairs(Dependencies) do
    if type(i) == "number" then
      proj:project()
    end
  end
end

Dependencies.load()

workspace "OpenNeighborhood"
  startproject "OpenNeighborhood"
  location (path.join(".", "build"))
  targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
  objdir "%{wks.location}/obj"

  architecture "x86_64"

  configurations {
    "Debug",
    "Release",
  }

  local premakeconfigdir = path.join("..", "deps", "premake")

  solutionitems {
    { ["EditorConfig"] = path.join("..", ".editorconfig") },
    { ["PremakeConfig"] = {
      path.join(premakeconfigdir, "Glad.lua"),
      path.join(premakeconfigdir, "GLFW.lua"),
      path.join(premakeconfigdir, "ImGui.lua"),
      path.join(premakeconfigdir, "NativeFileDialogExtended.lua"),
      path.join(premakeconfigdir, "XBDM.lua"),
      path.join(premakeconfigdir, "mINI.lua"),
      path.join(premakeconfigdir, "stb_image.lua"),
      path.join("..", "premake5.lua"),
    } }
  }

  systemversion "latest"

  filter "configurations:Debug"
    defines "DEBUG"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    defines "RELEASE"
    runtime "Release"
    optimize "on"

project "OpenNeighborhood"
  local srcdir = path.join(".", "src")
  local depsdir = path.join(".", "deps")

  language "C++"
  cppdialect "C++17"

  pchheader "pch.h"
  pchsource (path.join(srcdir, "pch.cpp"))

  files {
    path.join(srcdir, "**.h"),
    path.join(srcdir, "**.cpp"),
  }

  includedirs {
    srcdir,
    depsdir,
  }

  Dependencies.imports()

  defines "GLFW_INCLUDE_NONE"

  filter "system:windows"
    postbuildcommands ("{COPY} " .. path.join("..", "assets") .. " %{cfg.targetdir}/assets")

  filter "system:not windows"
    postbuildcommands ("{COPY} " .. path.join("..", "assets") .. " %{cfg.targetdir}")

  filter "system:not macosx"
    linkgroups "on"

  filter "system:windows"
    links "opengl32.lib"

    defines "_CRT_SECURE_NO_WARNINGS"

  filter "system:linux"
    kind "ConsoleApp"
    links {
      "X11",
      "pthread",
      "dl",
    }

  filter "system:macosx"
    kind "WindowedApp"
    linkoptions {
      "-framework Cocoa",
      "-framework IOKit",
      "-framework CoreFoundation",
    }

  filter { "configurations:Debug", "system:windows" }
    kind "ConsoleApp"

  filter { "configurations:Release", "system:windows" }
    kind "WindowedApp"
    linkoptions "/entry:mainCRTStartup"

group "Dependencies"
Dependencies.projects()
