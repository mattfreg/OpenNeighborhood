require('vstudio')

premake.api.register {
  name = "solutionitems",
  scope = "workspace",
  kind = "list:keyed:list:string",
}

premake.override(premake.vstudio.sln2005, "projects", function(base, wks)
  for _, folder in ipairs(wks.solutionitems) do
    for name, files in pairs(folder) do
      premake.push('Project("{2150E333-8FDC-42A3-9474-1A3956D46DE8}") = "' .. name .. '", "' .. name .. '", "{' .. os.uuid("Solution Items:" .. wks.name) .. '}"')
      premake.push("ProjectSection(SolutionItems) = preProject")

      for _, file in ipairs(files) do
        premake.w(file .. " = " .. file)
      end

      premake.pop("EndProjectSection")
      premake.pop("EndProject")
    end
  end
  base(wks)
end)
