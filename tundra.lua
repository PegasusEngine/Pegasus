Build
{
    Units = {
        "Build/tundra/pegasus.units.lua"
    },

    Configs = {
        {
            Name="win32-msvc",
            --Tools = { "msvc-vs2019" }
            Tools = { { "msvc-vs2017"; TargetArch="x86" } },
            SupportedHosts = { "windows" },
            DefaultOnHost = "windows",
            Includes = "CPPPATH",
        }
    },
    
    Env = {
        SRC_DIR = "Source",
        CXXOPTS = {
            {
                { "/std:c++17", "/WX", "/W3" },
                Config = "win32-msvc-*"
            }
        },
    }
}
