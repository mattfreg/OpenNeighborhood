#pragma once

#define MINI_CASE_SENSITIVE
#include <mINI/ini.h>

#include "Elements/Xbox.h"

class ConfigManager
{
public:
    using Config = mINI::INIStructure;
    using ConfigFile = mINI::INIFile;

    static void AddXbox(const Xbox &xbox);

    static const Config &GetConfig();

private:
    static std::filesystem::path s_ConfigFilePath;
    static ConfigFile s_ConfigFile;
    static Config s_Config;
};
