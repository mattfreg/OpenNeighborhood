#ifdef _WIN32
    #pragma once

    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#else
    #include <unistd.h>
#endif

#include <iostream>
#include <sstream>
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <sys/stat.h>
#include <queue>
#include <filesystem>
#include <utility>
#include <set>
#include <string.h>
#include <stdarg.h>
#include <ctime>

#ifdef DEBUG
    #include "Core/Log.h"
#endif
