GLFW = {
  root = path.join(Dependencies.basePath, "GLFW"),
}

function GLFW:import()
  links { "GLFW" }

  self:includes()
end

function GLFW:includes()
  includedirs (path.join(self.root, "include"))
end

function GLFW:project()
  local srcdir = path.join(self.root, "src")
  local includedir = path.join(self.root, "include")

  project "GLFW"
  kind "StaticLib"
  language "C"

  files {
    path.join(includedir, "GLFW", "glfw3.h"),
    path.join(includedir, "GLFW", "glfw3native.h"),
    path.join(srcdir, "glfw_config.h"),
    path.join(srcdir, "context.c"),
    path.join(srcdir, "init.c"),
    path.join(srcdir, "input.c"),
    path.join(srcdir, "monitor.c"),
    path.join(srcdir, "vulkan.c"),
    path.join(srcdir, "window.c"),
  }

  self:includes()

  filter "system:linux"
    files {
      path.join(srcdir, "x11_init.c"),
      path.join(srcdir, "x11_monitor.c"),
      path.join(srcdir, "x11_window.c"),
      path.join(srcdir, "xkb_unicode.c"),
      path.join(srcdir, "posix_time.c"),
      path.join(srcdir, "posix_thread.c"),
      path.join(srcdir, "glx_context.c"),
      path.join(srcdir, "egl_context.c"),
      path.join(srcdir, "osmesa_context.c"),
      path.join(srcdir, "linux_joystick.c"),
    }

    defines "_GLFW_X11"

  filter "system:windows"
    files {
      path.join(srcdir, "win32_init.c"),
      path.join(srcdir, "win32_joystick.c"),
      path.join(srcdir, "win32_monitor.c"),
      path.join(srcdir, "win32_time.c"),
      path.join(srcdir, "win32_thread.c"),
      path.join(srcdir, "win32_window.c"),
      path.join(srcdir, "wgl_context.c"),
      path.join(srcdir, "egl_context.c"),
      path.join(srcdir, "osmesa_context.c"),
    }

    defines {
      "_GLFW_WIN32",
      "_CRT_SECURE_NO_WARNINGS",
    }

  filter "system:macosx"
    files {
      path.join(srcdir, "cocoa_init.m"),
      path.join(srcdir, "cocoa_monitor.m"),
      path.join(srcdir, "cocoa_window.m"),
      path.join(srcdir, "cocoa_time.c"),
      path.join(srcdir, "posix_thread.c"),
      path.join(srcdir, "cocoa_joystick.m"),
      path.join(srcdir, "nsgl_context.m"),
      path.join(srcdir, "egl_context.c"),
      path.join(srcdir, "osmesa_context.c")
    }

    defines "_GLFW_COCOA"
end

table.insert(Dependencies, GLFW)
