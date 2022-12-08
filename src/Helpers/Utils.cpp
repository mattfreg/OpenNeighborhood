#include "pch.h"
#include "Helpers/Utils.h"

#include "Core/Assert.h"

namespace Utils
{

std::filesystem::path GetExecDir()
{
    const int MAX_SIZE = 200;
    char path[MAX_SIZE] = { 0 };

#ifdef PLATFORM_WINDOWS
    GetModuleFileNameA(NULL, path, MAX_SIZE);
#else
    size_t read = readlink("/proc/self/exe", path, MAX_SIZE);
    (void)read;
    ASSERT(static_cast<int>(read) != -1, "Couldn't get the execution directory!");
#endif

    std::filesystem::path execFilePath(path);

    return execFilePath.parent_path();
}

std::string DirName(const std::string &fullPath)
{
    size_t lastSeparatorIndex = fullPath.find_last_of('\\');

    if (lastSeparatorIndex == std::string::npos || lastSeparatorIndex == 0)
        return "\\";

    // Only keep what was before the last '\', so the parent directory
    return fullPath.substr(0, lastSeparatorIndex);
}

std::string BaseName(const std::string &fullPath)
{
    size_t lastSeparatorIndex = fullPath.find_last_of('\\');

    if (lastSeparatorIndex == std::string::npos)
        return fullPath;

    // Only keep what is after the last '\', so the file name
    return fullPath.substr(lastSeparatorIndex + 1, fullPath.size() - lastSeparatorIndex);
}

}
