require 'tundra.syntax.glob'

local function PegasusLib(name, srcFolder, srcFolderIsRecursive, deps)
    return StaticLibrary {
        Name = name,
        Includes = { "Include" },
        Sources = {
            Glob {
                Dir = "Source/Pegasus/" .. srcFolder,
                Extensions = { ".cpp", ".h" },
                Recursive = srcFolderIsRecursive 
            },
            Glob {
                Dir = "Include/Pegasus/" .. srcFolder,
                Extensions = { ".h" },
                Recursive = srcFolderIsRecursive 
            }
        },
        Depends = deps,
        Env = {
            CXXOPTS = {
              {
                  Config = "win32-msvc-*",
                  {
                      "/FIPegasus/Preprocessor.h",
                      "/FIPegasus/PegasusInternal.h",
                      "/EHsc",
                      "/wd5033", --ignore register keyword deprecation warning
                      "/wd4514", -- ignore unreferenced symbols warning
                  }
              },
            },
        }
    }
end

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

--folder and recursive flag
local pegasus_modules_src_folders = {
    Pegasus = { "", false }
}

for i, moduleName in ipairs(pegasus_modules) do
    local deps = pegasus_modules_dependencies[moduleName]
    local srcFolderRecursePair = pegasus_modules_src_folders[moduleName]
    local srcFolder = (srcFolderRecursePair == nil) and moduleName or srcFolderRecursePair[1]
    local isRecursive = (srcFolderRecursePair == nil) and true or srcFolderRecursePair[2]
    PegasusLib(
        moduleName,
        srcFolder,
        isRecursive,
        (deps == nil) and {} or deps --only pass dependencies if they exist
     )
    Default(moduleName)
end

