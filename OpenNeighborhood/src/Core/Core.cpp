#include "pch.h"
#include "Core/Core.h"

std::filesystem::path GetExecDir()
{
	const int MAX_SIZE = 200;
	char path[MAX_SIZE] = { 0 };

#ifdef PLATFORM_WINDOWS
	GetModuleFileNameA(NULL, path, MAX_SIZE);
#else
	size_t read = readlink("/proc/self/exe", path, MAX_SIZE);
	ASSERT(read != -1, "Couldn't get the execution directory!");
#endif

	std::filesystem::path execFilePath(path);
	return execFilePath.parent_path();
}
