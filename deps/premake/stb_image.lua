stb_image = {
  root = path.join(Dependencies.basePath, "stb_image")
}

function stb_image:import()
  links { "stb_image" }

  self:includes()
end

function stb_image:includes()
  includedirs (self.root)
end

function stb_image:project()
  project "stb_image"
    kind "StaticLib"
    language "C++"

    files {
      path.join(self.root, "stb_image.h"),
      path.join(self.root, "stb_image.cpp"),
    }

    self:includes()
end

table.insert(Dependencies, stb_image)
