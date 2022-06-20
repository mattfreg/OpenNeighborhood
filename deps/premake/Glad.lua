Glad = {
  root = path.join(Dependencies.basePath, "Glad"),
}

function Glad:import()
  links { "Glad" }

  self:includes()
end

function Glad:includes()
  includedirs {
    path.join(self.root, "include"),
  }
end

function Glad:project()
  project "Glad"
  kind "StaticLib"
  language "C"

  files {
    path.join(self.root, "include", "glad", "glad.h"),
    path.join(self.root, "include", "KHR", "khrplatform.h"),
    path.join(self.root, "src", "glad.c"),
  }

  self:includes()
end

table.insert(Dependencies, Glad)
