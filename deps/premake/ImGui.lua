ImGui = {
  root = path.join(Dependencies.basePath, "ImGui"),
}

function ImGui:import()
  links { "ImGui" }

  self:includes()
end

function ImGui:includes()
  includedirs {
    self.root,
    GLFW:includes(),
    Glad:includes(),
  }
end

function ImGui:project()
  local backendsdir = path.join(self.root, "backends")

  project "ImGui"
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"

  files {
    path.join(self.root, "*.h"),
    path.join(self.root, "*.cpp"),
    path.join(backendsdir, "imgui_impl_glfw.cpp"),
    path.join(backendsdir, "imgui_impl_glfw.h"),
    path.join(backendsdir, "imgui_impl_opengl3.cpp"),
    path.join(backendsdir, "imgui_impl_opengl3.h"),
  }

  self:includes()
end

table.insert(Dependencies, ImGui)
