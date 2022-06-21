NativeFileDialogExtended = {
  root = path.join(Dependencies.basePath, "NativeFileDialogExtended"),
}

function NativeFileDialogExtended:import()
  links { "NativeFileDialogExtended" }

  self:includes()
end

function NativeFileDialogExtended:includes()
  includedirs {
    path.join(self.root, "src", "include"),
  }
end

function NativeFileDialogExtended:project()
  local srcdir = path.join(self.root, "src")
  local includedir = path.join(self.root, "src", "include")

  project "NativeFileDialogExtended"
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"

  files {
    path.join(includedir, "nfd.h"),
    path.join(includedir, "nfd.hpp"),
  }

  self:includes()

  filter "system:windows"
    files {
      path.join(srcdir, "nfd_win.cpp"),
    }

  filter "system:linux"
    files {
      path.join(srcdir, "nfd_gtk.cpp"),
    }

    buildoptions "`pkg-config --cflags gtk+-3.0`"

  filter "system:macosx"
    files {
      path.join(srcdir, "nfd_cocoa.m"),
    }

    linkoptions "-framework AppKit"
end

table.insert(Dependencies, NativeFileDialogExtended)
