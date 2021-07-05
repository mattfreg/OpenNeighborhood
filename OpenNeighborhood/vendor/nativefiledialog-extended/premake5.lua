project "NativeFileDialogExtended"
	kind "StaticLib"
	language "C++"
	staticruntime "on"

	targetdir (TargetDir .. "/%{prj.name}")
	objdir (ObjDir .. "/%{prj.name}")

	files {
		"src/include/nfd.h",
		"src/include/nfd.hpp"
	}

	includedirs {
		"src/include"
	}

	filter "system:linux"
		pic "on"

		systemversion "latest"

		files {
			"src/nfd_gtk.cpp"
		}

	filter "system:windows"
		systemversion "latest"

		files {
			"src/nfd_win.cpp"
		}

	filter "system:macosx"
		files {
			"src/nfd_cocoa.m"
		}

		linkoptions {
			"-framework AppKit",
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
