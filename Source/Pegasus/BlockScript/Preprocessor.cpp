/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Preprocessor.cpp
//! \author Kleber Garcia
//! \date   January 31, 2015
//! \brief  Blockscript Internal Compiler Preprocessor directive

#include "Pegasus/BlockScript/Preprocessor.h"

using namespace Pegasus::BlockScript;

Preprocessor::Preprocessor()
:    mCommand(Preprocessor::PP_CMD_NONE),
     mStringArg(nullptr),
     mCodeArg(nullptr)
{
}

Preprocessor::~Preprocessor()
{
}

void Preprocessor::IncludeCmd()
{
    mCommand = Preprocessor::PP_CMD_INCLUDE;
}

void Preprocessor::DefineCmd()
{
    mCommand = Preprocessor::PP_CMD_DEFINE;
}

void Preprocessor::PushString(const char* str)
{
    mStringArg = str;
}

void Preprocessor::PushCode(const char* str)
{
    mCodeArg = str;
}

bool Preprocessor::FlushCommand(const char** errString)
{
    errString = nullptr;
    bool result = false;
    switch (mCommand)
    {
    case Preprocessor::PP_CMD_INCLUDE:
        {
            //*errString = "error including file";
            result = true;
            break;
        }
    case Preprocessor::PP_CMD_DEFINE:
        {
            //*errString = "";
            int i = 0;
            result = true;
            break;
        }
    default:
        break;
    }
    mCommand = Preprocessor::PP_CMD_NONE;
    mStringArg = nullptr;
    mCodeArg = nullptr;
    return result;
}


