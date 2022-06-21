XBDM = {
  root = path.join(Dependencies.basePath, "XBDM"),
}

function XBDM:import()
  links { "XBDM" }

  self:includes()
end

function XBDM:includes()
  includedirs {
    path.join(self.root, "include"),
    path.join(self.root, "src"),
  }
end

function XBDM:project()
  project "XBDM"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    local includedir = path.join(self.root, "include")
    local srcdir = path.join(self.root, "src")

    pchheader "pch.h"
    pchsource (path.join(srcdir, "pch.cpp"))

    files {
      path.join(includedir, "**.h"),
      path.join(srcdir, "**.h"),
      path.join(srcdir, "**.cpp"),
    }

    self:includes()

    filter "system:windows"
      links "Ws2_32.lib"
end

table.insert(Dependencies, XBDM)
