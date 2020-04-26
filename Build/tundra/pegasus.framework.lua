require 'tundra.syntax.glob'
require 'tundra.syntax.bison'
require 'tundra.syntax.flex'
local npath = require 'tundra.native.path'
local path     = require "tundra.path"

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
  ConfigInvariant = true,

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
    local gen_c = "$(OBJECTROOT)$(SEP)"..data.OutputCFile
    local gen_h = "$(OBJECTROOT)$(SEP)"..data.OutputHeaderFile
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
  ConfigInvariant = true,

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
      out_src = "$(OBJECTROOT)$(SEP)" .. data.OutputFile
    else
      local targetbase = "$(OBJECTROOT)$(SEP)bisongen_" .. path.get_filename_base(src)
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

local function BuildPegasusLib(name, srcFolder, srcFolderIsRecursive, deps, codegens)

    local SourceDir = "Source/Pegasus/" .. srcFolder
    local HeaderDir = "Include/Pegasus/" .. srcFolder
    local envs = {
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

        includes[#includes + 1] = "$(OBJECTROOT)$(SEP)bisongen/Source/"
        includes[#includes + 1] = "$(OBJECTROOT)$(SEP)flexgen/Source/"
    end

    return StaticLibrary {
        Name = name,
        Includes = includes,
        Sources = sources,
        Depends = deps,
        Env = envs
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

