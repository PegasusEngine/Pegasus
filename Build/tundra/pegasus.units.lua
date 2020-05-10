-- core static module lists

local pegasus_modules  = {
    "Allocator",
    "Application",
    "AssetLib", 
    "BlockScript",
    "Core",
    "Graph",
    "Math",
    "Memory",
    "Mesh",
    "Pegasus",
    "PropertyGrid",
    "Render",
    "RenderSystems",
    "Shader",
    "Sound",
    "Texture",
    "Timeline",
    "Utils",
    "Window",
}

-- core dependencies for each module
local pegasus_modules_dependencies  = {
    Application =  { "Allocator", "AssetLib", "BlockScript", "Core", "Graph", "Math", "Memory", "Mesh", "PropertyGrid", "RenderSystems", "Shader", "Sound", "Texture", "Timeline", "Utils", "Window" },
    AssetLib  = { "Allocator", "Core", "Memory", "Utils" },
    BlockScript = { "Allocator", "Core", "Memory", "Utils" },
    Core = { "Allocator" },
    Graph = { "Allocator", "Core", "Math", "Memory", "PropertyGrid", "Utils" },
    Math = { "Core" },
    Memory = { "Allocator", "Core" },
    Mesh = { "Allocator", "AssetLib", "Core", "Graph", "Math", "Memory", "PropertyGrid", "Utils" },
    Pegasus = { "Allocator", "Application", "AssetLib", "BlockScript", "Core", "Graph", "Math", "Memory", "Mesh", "PropertyGrid", "Render", "RenderSystems", "Shader", "Sound", "Texture", "Timeline", "Utils", "Window" },
    PropertyGrid = { "Allocator", "AssetLib", "Core", "Math", "Memory", "Utils" },
    Render = { "Allocator", "Core", "Graph", "Math", "Memory", "Mesh", "PropertyGrid", "Shader", "Texture", "Utils" },
    RenderSystems = { "Allocator", "AssetLib", "BlockScript", "Core", "Graph", "Math", "Memory", "Mesh", "PropertyGrid", "Shader", "Texture", "Utils" },
    Shader = { "Allocator", "Core", "Graph", "Math", "PropertyGrid" },
    Sound = { "Allocator", "Core" },
    Texture = { "Allocator", "AssetLib", "Core", "Graph", "Math", "Memory", "PropertyGrid", "Utils" },
    Timeline = { "Allocator", "AssetLib", "BlockScript", "Core", "Graph", "Math", "PropertyGrid", "Sound", "Window" },
    Utils = { "Allocator", "Core", "Math" },
    Window = { "Allocator", "Core", "Memory", "Render" }
}

--folder and recursive flag overrides.
local pegasus_modules_src_folders = {
    Pegasus = { "", false }
}

local pegasus_modules_codegens = {
    AssetLib = {
        prefix="AS_"
    },
    BlockScript = {
        prefix = "BS_"
    }
}

local qt_dynamic_modules = {
    "Core",
    "Widgets",
    "Gui",
    "Concurrent",
    "Xml",
    "XmlPatterns"
}

local qt_static_modules = {
    "qtmain",
}

-- default applications
local pegasus_apps = {
    "TestApp1"
}

_G.BuildPegasusLibs(
    pegasus_modules,
    pegasus_modules_dependencies,
    pegasus_modules_src_folders,
    pegasus_modules_codegens)

_G.BuildPegasusLauncher()

_G.BuildPegasusApps(pegasus_apps, pegasus_modules)

_G.BuildPegasusUtility("BlockScriptCLI", { "BlockScript", "Core", "Memory", "Utils", "Math", "Allocator" } )
_G.BuildPegasusUtility("BlockScriptTests", { "BlockScript", "Core", "Memory", "Utils", "Math", "Allocator" } )
_G.BuildPegasusUtility("UnitTests", pegasus_modules )

_G.BuildQtApp("Editor", "Source/Editor", qt_static_modules, qt_dynamic_modules, pegasus_modules)
