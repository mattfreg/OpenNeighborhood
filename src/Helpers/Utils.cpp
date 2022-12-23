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

std::vector<std::string> StringSplit(const std::string &string, const std::string &separator)
{
    std::vector<std::string> result;
    std::string stringCopy = string;

    if (separator.empty())
        return result;

    for (;;)
    {
        size_t pos = stringCopy.find(separator);

        // If separator is not in stringCopy, just push what is left of stringCopy
        // into the vector and return it
        if (pos == std::string::npos)
        {
            result.push_back(stringCopy);
            return result;
        }

        std::string token = stringCopy.substr(0, pos);

        result.push_back(token);

        stringCopy.erase(0, pos + separator.size());
    }
}

}
