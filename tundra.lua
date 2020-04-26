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
        TOOLS_DIR = "Tools",
        BISON = {
            {
                "$(TOOLS_DIR)$(SEP)FlexAndBison$(SEP)Win32$(SEP)win_bison.exe";
                Config = "win32-*-*"
            }
        },
        BISONOPT = { "-d" },
        FLEX = {
            {
                "$(TOOLS_DIR)$(SEP)FlexAndBison$(SEP)Win32$(SEP)win_flex.exe";
                Config = "win32-*-*"
            }
        },
        FLEXOPT = {
            {
                "--wincompat ";
                Config = "win32-*-*"
            }
        },
        CXXOPTS = {
            {
                { "/std:c++17", "/WX", "/W3" },
                Config = "win32-msvc-*"
            }
        },
    }
}
