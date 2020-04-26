require 'tundra.syntax.glob'

local function PegasusLib(name)
    return StaticLibrary {
        Name = name,
        Includes = { "Include" },
        Sources = {
            Glob {
                Dir = "Source/Pegasus/" .. name,
                Extensions = { ".cpp", ".h" }
            }
        },
        Env = {
            CXXOPTS = {
              { "/FIPegasus/Preprocessor.h"; Config = "win32-msvc-*" },
              { "/FIPegasus/PegasusInternal.h"; Config = "win32-msvc-*" },
            },
        }
    }
end

PegasusLib("Allocator")
PegasusLib("Core")
PegasusLib("Math")

Default "Allocator"
Default "Core"
Default "Math"
