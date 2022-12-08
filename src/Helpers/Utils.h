#pragma once

#define BIT(x) (1 << x)

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename... Args>
constexpr Ref<T> CreateRef(Args &&...args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

namespace Utils
{

std::filesystem::path GetExecDir();

std::string DirName(const std::string &fullPath);

std::string BaseName(const std::string &fullPath);

}
