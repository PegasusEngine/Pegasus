Build
{
    Units = {
        "Build/tundra/pegasus.framework.lua",
        "Build/tundra/pegasus.units.lua"
    },

    Passes = {
        CodeGeneration = { Name = "Code Generation" , BuildOrder = 1 }
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
        BISON = "win_bison.exe",
        BISONOPT = { "-d" },
        FLEX = "win_flex.exe",
        FLEXOPT = { "--wincompat " },
        CXXOPTS = {
            {
                { "/std:c++17", "/WX", "/W3" },
                Config = "win32-msvc-*"
            }
        },
    }
}
