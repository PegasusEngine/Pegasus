local msvc_version = "msvc-vs2017"
local qt_version = "5.8"

Build
{
    Units = {
        "Build/tundra/pegasus.framework.lua",
        "Build/tundra/pegasus.units.lua"
    },

    Passes = {
        CodeGeneration = { Name = "CodeGeneration" , BuildOrder = 1 },
        BuildCode = { Name = "BuildCode", BuildOrder = 2 },
        Deploy = { Name = "Deploy", BuildOrder = 3 }
    },

    Configs = {
        {
            Name="win32-msvc",
            Tools = { { msvc_version; TargetArch="x64", HostArch="x64" } },
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
                Config = "win32-*-*-*"
            }
        },
        BISONOPT = { "-d" },
        FLEX = {
            {
                "$(TOOLS_DIR)$(SEP)FlexAndBison$(SEP)Win32$(SEP)win_flex.exe";
                Config = "win32-*-*-*"
            }
        },
        FLEXOPT = {
            {
                "--wincompat ";
                Config = "win32-*-*-*"
            }
        },

        CXXOPTS_OPT = {
            {
                { "/O2", "/MD" },
                Config = "win32-msvc-*-*"
            }
        },
        CXXOPTS = {
            {
                { "/std:c++17", "/WX", "/W3" },
                Config = "win32-msvc-*-*"
            },
            {
                { "/Od", "/MDd" },
                Config = "win32-msvc-debug-*"
            }
        },
        CPPDEFS_OPT = { "_PEGASUS_OPT" },
        CPPDEFS = {
            {
                Config = "*-*-*-dev",
                { "_PEGASUS_DEV", "_PEGASUSAPP_DLL" }
            },
            {
                Config = "*-*-*-rel",
                { "_PEGASUS_REL" }
            },
            {
                Config = "*-*-debug-*",
                { "_PEGASUS_DEBUG", "_DEBUG"  }
            },
        },

        CP_CMD = {
            {
                "cp",
                Config = "win32-*-*-*"
            }
        },

        LD = {
            {
                { "/MACHINE:x64", "/SUBSYSTEM:WINDOWS", "/LIBPATH:Lib" },
                Config = "win32-msvc-*-*"
            }
        },

        RUNTIMELIBS_ROOT = "Lib$(SEP)RuntimeLibs$(SEP)",

        QT_VER = qt_version,
        QT_ROOT = {
            {
                "Lib$(SEP)Qt$(SEP)$(QT_VER)$(SEP)",
                Config = "win32-*-*-*"
            }
        },
        QT_LIBS = {
            {
                "$(QT_ROOT)$(SEP)lib$(SEP)",
                Config = "win32-*-*-*"
            }
        },
        QT_BINS = {
            {
                "$(QT_ROOT)$(SEP)bin$(SEP)",
                Config = "win32-*-*-*"
            }
        },
        QTMOCCMD = {
            {
                "$(QT_BINS)moc.exe",
                Config = "win32-*-*-*"
            }
        },
        QTUICCMD = {
            {
                "$(QT_BINS)uic.exe",
                Config = "win32-*-*-*"
            }
        },
        QTRCCCMD  = {
            {
                "$(QT_BINS)rcc.exe",
                Config = "win32-*-*-*"
            }
        },
        QT_INCLUDE = {
            {
                "$(QT_ROOT)$(SEP)include$(SEP)",
                Config = "win32-*-*-*"
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
            ['opt-dev']    = 'opt-dev',
            ['debug-dev']  = 'debug-dev',
            ['opt-rel']    = 'opt-rel',
            ['debug-rel']  = 'debug-rel'
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
    },

    Variants = { "debug", "opt" },
    SubVariants = { "dev", "rel" }
    
}
