require 'tundra.syntax.glob'
require 'tundra.syntax.bison'
require 'tundra.syntax.flex'
require 'tundra.syntax.qt'
require 'tundra.syntax.files'

local scanner = require "tundra.scanner"
local npath = require 'tundra.native.path'
local path     = require "tundra.path"

local SourceRootFolder = "Source/Pegasus/"
local HeaderRootFolder = "Include/Pegasus/"

local RootEnvs = {
    CXXOPTS = {
      {
          Config = "win32-msvc-*-*",
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

local function GenRootIdeHints(rootFolder)
   return {
        Msvc = {
            SolutionFolder = rootFolder
        }
    }
end

local function ChangeExtension(fileList, newExtension)
    local outList = {}
    for i, v in ipairs(fileList) do
        outList[i] = npath.get_filename_dir(v) .. "/" .. npath.get_filename_base(v) .. "." .. newExtension
    end
    return outList
end

DefRule {
  Name = "PegasusFlex",
  Command = "",

  Blueprint = {
    Source           = { Required = true, Type = "string" },
    OutputCFile      = { Required = true, Type = "string" },
    Prefix           = { Required = true, Type  = "string" },
    OutputHeaderFile = { Required = false, Type = "string" },
    Pass             = { Required = true, Type = "pass", }
  },

  Setup = function (env, data)
    local src = data.Source
    local base_name = path.drop_suffix(src)
    local gen_c = "$(OBJECTDIR)$(SEP)"..data.OutputCFile
    local gen_h = "$(OBJECTDIR)$(SEP)"..data.OutputHeaderFile
    return {
      InputFiles = { src },
      OutputFiles = { gen_c, gen_h },
      Command = "$(FLEX) $(FLEXOPT) -P " .. data.Prefix .. " --outfile=".. gen_c .. " --header-file=" ..gen_h.. " $(<)",
    }
  end,
}


DefRule {
  Name = "PegasusBison",
  Command = "", -- Replaced on a per-instance basis.

  Blueprint = {
    Source       = { Required = true, Type  = "string" },
    Prefix       = { Required = true, Type  = "string" },
    OutputFile   = { Required = false, Type = "string" },
    TokenDefines = { Required = false, Type = "boolean" },
    Pass         = { Required = true, Type = "pass" },
  },

  Setup = function (env, data)
    local src = data.Source
    local out_src
    if data.OutputFile then
      out_src = "$(OBJECTDIR)$(SEP)" .. data.OutputFile
    else
      local targetbase = "$(OBJECTDIR)$(SEP)bisongen_" .. path.get_filename_base(src)
      out_src = targetbase .. ".c"
    end
    local defopt = ""
    local outputs = { out_src }
    if data.TokenDefines then
      local out_hdr = path.drop_suffix(out_src) .. ".h"
      defopt = "--defines=" .. out_hdr
      outputs[#outputs + 1] = out_hdr
    end
    return {
      Pass = data.Pass,
      Label = "Bison $(@)",
      Command = "$(BISON) $(BISONOPT) -p " ..data.Prefix .. " " .. defopt .. " --output-file=$(@:[1]) $(<)",
      InputFiles = { src },
      OutputFiles = outputs,
    }
  end,
}

DefRule {
  Name = "PegasusQtMoc",
  Command = "",
  Blueprint = {
    Source = { Required = true, Type = "string" },
    DestFolder = { Required = true, Type = "string" }
  },

  Setup = function (env, data)
    local src = data.Source
    -- We follow these conventions:
    --   If the source file is a header, we do a traditional header moc:
    --     - input: foo.h, output: moc_foo.cpp
    --     - moc_foo.cpp is then compiled separately together with (presumably) foo.cpp
    --   If the source file is a cpp file, we do things a little differently:
    --     - input: foo.cpp, output foo.moc
    --     - foo.moc is then manually included at the end of foo.cpp
    local base_name = path.get_filename_base(src) 
    local base_path = npath.get_filename_dir(src)
    local pfx = 'moc_'
    local ext = '.cpp'
    print(src)
    if path.get_extension(src) == ".cpp" then
      pfx = ''
      ext = '.moc'
    end
    local outputFileName = "$(OBJECTDIR)$(SEP)" .. data.DestFolder .. "$(SEP)" .. base_path .. "$(SEP)" .. pfx .. base_name .. ext
    return {
      InputFiles = { src },
      OutputFiles = { outputFileName },
      Command = "$(QTMOCCMD) " .. src .. " > "..outputFileName,
    }
  end,
}

DefRule {
  Name = "PegasusQtUic",
  Command = "",
  Blueprint = {
    Source = { Required = true, Type = "string" },
    DestFolder = { Required = true, Type = "string" }
  },

  Setup = function (env, data)
    local src = data.Source
    local base_name = path.get_filename_base(src) 
    local base_path = npath.get_filename_dir(src)
    local pfx = 'ui_'
    local ext = '.h'
    local outputFileName = "$(OBJECTDIR)$(SEP)" .. data.DestFolder .. "$(SEP)" .. base_path .. "$(SEP)" .. pfx .. base_name .. ext
    return {
      InputFiles = { src },
      OutputFiles = { outputFileName },
      Command = "$(QTUICCMD) "..src.." > "..outputFileName
    }
  end,
}

local function BisonFlexCodeGen(SourceDir, srcFolderIsRecursive, codegens)
    local flexSources = Glob {
          Dir = SourceDir,
          Extensions = { ".l" },
          Recursive = srcFolderIsRecursive
    }
    local flexOutput = ChangeExtension (flexSources, "lexer.cpp")
    local flexHeaders = ChangeExtension (flexSources, "lexer.hpp")
 
    local bisonSources = Glob {
        Dir = SourceDir,
        Extensions = { ".y" },
        Recursive = srcFolderIsRecursive
    }
    local bisonOutput = ChangeExtension (bisonSources, "parser.cpp")
 
    local prefix = codegens["prefix"]

    local sources = {}
    for i,flexSrc in ipairs(flexSources) do
        sources[#sources + 1] = PegasusFlex {
            Source = flexSrc,
            OutputCFile = "flexgen$(SEP)"..flexOutput[i],
            OutputHeaderFile = "flexgen$(SEP)" .. flexHeaders[i],
            Prefix = prefix,
            Pass = "CodeGeneration"
        }
    end
    
    for i,bisonSrc in ipairs(bisonSources) do
        sources[#sources + 1] = PegasusBison {
            Source = bisonSrc,
            OutputFile = "bisongen$(SEP)" .. bisonOutput[i],
            Prefix = prefix,
            Pass = "CodeGeneration"
        }
    end
    
    local includes = {"$(OBJECTDIR)$(SEP)bisongen/Source/", "$(OBJECTDIR)$(SEP)flexgen/Source/"}

    return { includesList = includes, sourcesList = sources }
end

local function BuildPegasusLib(name, srcFolder, srcFolderIsRecursive, deps, codegens)

    local SourceDir = SourceRootFolder .. srcFolder
    local HeaderDir = HeaderRootFolder .. srcFolder

    local envs = RootEnvs

    local sources = {
            Glob {
                Dir = SourceDir,
                Extensions = { ".cpp", ".h", ".hpp" },
                Recursive = srcFolderIsRecursive 
            },
            Glob {
                Dir = HeaderDir,
                Extensions = { ".h", ".hpp" },
                Recursive = srcFolderIsRecursive 
            }
    }

    local includes = { "Include" }

    if codegens then
        local bisonFlexResult = BisonFlexCodeGen(SourceDir, srcFolderIsRecursive, codegens)
        for _, v in ipairs(bisonFlexResult.includesList) do
            includes[#includes + 1] = v
        end
        for _, v in ipairs(bisonFlexResult.sourcesList) do
            sources[#sources + 1] = v
        end
    end

    return StaticLibrary {
        Name = name,
        Pass = "BuildCode",
        Includes = includes,
        Sources = sources,
        Depends = deps,
        Env = envs,
        IdeGenerationHints = GenRootIdeHints("Pegasus")
    }
end


function _G.BuildPegasusApp(appName, pegasus_modules)

    local sources = Glob {
        Dir = "Source/Apps/" .. appName,
        Extensions = { ".cpp", ".h" }
    }

    local sourcesHeaders = Glob {
        Dir = "Include/Apps/" .. appName,
        Extensions = { ".h" }
    }

    for _, v in ipairs(sourcesHeaders) do
        sources[#sources + 1] = v
    end

    local includes = {
        "include/",
        "include/Pegasus/",
        "include/Apps/" .. appName
    }

    SharedLibrary {
        Name = appName,
        Pass = "BuildCode",
        Sources = sources,
        Includes = includes,
        Depends = pegasus_modules,
        Env = RootEnvs,
        Config = "*-*-*-dev",
        IdeGenerationHints = GenRootIdeHints("Apps")
    }

    Program {
        Name = appName .. "_Standalone",
        Pass = "BuildCode",
        Sources = sources,
        Includes = includes,
        Depends = pegasus_modules,
        Env = RootEnvs,
        Config = "*-*-*-rel",
        IdeGenerationHints = GenRootIdeHints("Apps")
    }
end

function _G.BuildPegasusLibs(
    pegasus_modules, pegasus_modules_dependencies,
    pegasus_modules_src_folders, pegasus_modules_codegens)

    for i, moduleName in ipairs(pegasus_modules) do
        local deps = pegasus_modules_dependencies[moduleName]
        local srcFolderRecursePair = pegasus_modules_src_folders[moduleName]
        local srcFolder = (srcFolderRecursePair == nil) and moduleName or srcFolderRecursePair[1]
        local isRecursive = (srcFolderRecursePair == nil) and true or srcFolderRecursePair[2]
        print(moduleName)
        BuildPegasusLib(
            moduleName,
            srcFolder,
            isRecursive,
            (deps == nil) and {} or deps, --only pass dependencies if they exist
            pegasus_modules_codegens[moduleName] -- for the most part only a few projects have flex/bison
        )
        Default(moduleName)
    end
end

function _G.BuildPegasusLauncher()
    local sources = Glob {
          Dir = "Source/Launcher/",
          Extensions = { ".cpp", ".h" },
          Recursive = true
    }
    
    Program {
        Name = "Launcher",
        Pass = "BuildCode",
        Sources = sources,
        Includes = "Include",
        Libs = { "user32.lib" },
        Config = "*-*-*-dev",
        Env = RootEnvs,
        IdeGenerationHints = GenRootIdeHints("Launcher") 
    }
    Default("Launcher")
end

function _G.BuildPegasusApps(pegasus_apps, pegasus_modules)
    for i, appName in ipairs(pegasus_apps) do
        BuildPegasusApp(appName, pegasus_modules)
        Default(appName)
        Default(appName .. "_Standalone")
    end
end


function _G.BuildQtApp(appName, appSrcRoot, qt_static_modules, qt_dynamic_modules, dependencies)
    local genDirSrc = "$(OBJECTDIR)$(SEP)qtgen$(SEP)"

    local includes = {
        ".",
        "include",
        appSrcRoot,
        genDirSrc,
        genDirSrc .. "$(SEP)Source",
        "$(QT_INCLUDE)"
    }
    local libs = {
        {
            { "user32.lib", "kernel32.lib", "comdlg32.lib", "shell32.lib", "ole32.lib", "oleaut32.lib" },
            Config = "win32-*-*-*"
        }
    }
    local dllLibs = {}
    local dllDest = {}

    for _, v in ipairs(qt_static_modules) do
        libs[#libs + 1] = "$(QT_LIBS)"..v..".lib"
    end

    for _, v in ipairs(qt_dynamic_modules) do
        includes[#includes + 1] = "$(QT_INCLUDE)Qt"..v
        libs[#libs + 1] = "$(QT_LIBS)Qt5"..v..".lib"
        dllLibs[#dllLibs + 1] = "$(QT_BINS)Qt5"..v..".dll"
        dllDest[#dllDest + 1] = "$(OBJECTDIR)$(SEP)Qt5"..v..".dll"
    end

    local sources = {
        Glob {
                Dir = appSrcRoot,
                Extensions = { ".cpp", ".h" }
        }
    }

    local mocInputs = Glob {
        Dir = appSrcRoot,
        Extensions = { ".h", ".cpp" }
    }

    for _, v in ipairs(mocInputs) do
        sources[#sources + 1] = PegasusQtMoc { Pass = "CodeGeneration", DestFolder = "qtgen", Source = v }
        includes[#includes + 1] = genDirSrc .. "$(SEP)" .. npath.get_filename_dir(v)
    end

    local uiInputs = Glob {
        Dir = appSrcRoot,
        Extensions = { ".ui" }
    }

    for _, v in ipairs(uiInputs) do
        sources[#sources + 1] = PegasusQtUic { Pass = "CodeGeneration", DestFolder = "qtgen", Source = v }
        includes[#includes + 1] = genDirSrc .. "$(SEP)" .. npath.get_filename_dir(v)
    end

    for i, v in ipairs(dllLibs) do
        print("COPY TO:" .. dllDest[i])
        local dllDeployed = CopyFile { Source = v, Target = dllDest[i], Pass="BuildCode" }
        dependencies[#dependencies + 1] = dllDeployed
    end

    Program {
        Name = appName,
        Pass = "BuildCode",
        Sources = sources,
        Includes = includes,
        Config = "*-*-*-dev",
        Depends = dependencies,
        Libs = libs,
        Defines = {
            "UNICODE", "WIN32"
        },
        Env = {
            CXXOPTS = {
                {
                    Config = "win32-*-*-*",
                    {
                        "/MD",
                        "/Zc:wchar_t-",
                        "/FISource/Editor/Assertion.h",
                        "/FISource/Editor/Log.h"
                    }
                }
            }
        },
        IdeGenerationHints = GenRootIdeHints(appName)
    }

    Default(appName)

end
