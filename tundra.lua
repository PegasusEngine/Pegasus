local msvc_version = "msvc-vs2017"
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
            Tools = { { msvc_version; TargetArch="x86", HostArch="x86" } },
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
        LD = {
            {
                { "/MACHINE:x86", "/SUBSYSTEM:WINDOWS", "/LIBPATH:Lib" },
                Config = "win32-msvc-*"
            }
        },
        GENERATE_PDB = "1"
    },

    IdeGenerationHints = {
        Msvc = {
          -- Remap config names to MSVC platform names (affects things like header scanning & debugging)
          PlatformMappings = {
            ['win32-msvc'] = 'Win32',
          },
          -- Remap variant names to MSVC friendly names
          VariantMappings = {
            ['release']    = 'Release',
            ['debug']      = 'Debug',
            ['production'] = 'Production',
          },
        },
        
        -- Override solutions to generate and what units to put where.
        MsvcSolutions = {
          ['Pegasus.sln'] = {},          -- receives all the units due to empty set
        },
        
        -- Cause all projects to have "Build" ticked on them inside the MSVC Configuration Manager.
        -- As a result of this, you can choose a project as the "Startup Project",
        -- and when hitting "Debug" or "Run", the IDE will build that project before running it.
        -- You will want to avoid pressing "Build Solution" with this option turned on, because MSVC
        -- will kick off all project builds simultaneously.
        BuildAllByDefault = true,
    }
}
