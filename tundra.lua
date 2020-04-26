Build
{
    Units = {
        "Build/tundra/pegasus.lua"
    },

    Configs = {
        {
            Name="win32-msvc",
            --Tools = { "msvc-vs2019" }
            Tools = { "msvc-vs2017" },
            SupportedHosts = { "windows" },
            DefaultOnHost = "windows",
            Includes = "CPPPATH",
        }
    },
    
    Env = {
        SRC_DIR = "Source"
    }
}
