mINI = {
  root = path.join(Dependencies.basePath, "mINI")
}

function mINI:import()
  links { "mINI" }

  self:includes()
end

function mINI:includes()
  includedirs (self.root)
end

function mINI:project()
  project "mINI"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    files {
      path.join(self.root, "ini.h"),
      path.join(self.root, "ini.cpp"),
    }
end

table.insert(Dependencies, mINI)
